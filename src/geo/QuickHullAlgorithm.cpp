/* Copyright 2017 The sfcpp Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/



#include "QuickHullAlgorithm.hpp"

namespace sfcpp {
namespace geo {

std::function<void(FaceInfoPtr)> FaceInfo::dummyFunction = getDummyFunction<FaceInfoPtr>();

QuickHullAlgorithm::QuickHullAlgorithm(Eigen::MatrixXd points)
    : vertices(points.cols()),
      innerPoint(Eigen::VectorXd::Zero(points.rows())),
      dim(points.rows()),
      vertexFaces(),
      facets(),
      aboveFacets(),
      insideFacets(),
      newFacets(),
      unprocessedFacets(),
      connectFaces(),
      newConnectFaces(),
      polytope(new ConvexPolytope(points.rows())) {
  for (int i = 0; i < points.cols(); ++i) {
    vertices[i] = points.col(i);
  }
}

QuickHullAlgorithm::~QuickHullAlgorithm() {
  for (auto facet : facets) {
    deleteChildrenRecursively(facet);
  }
}

void QuickHullAlgorithm::visitChildrenWithFlag(FaceInfoPtr face, bool visitedValue,
                                               std::function<void(FaceInfoPtr)> const &func) {
  if (face->visited == visitedValue) {
    return;
  }

  func(face);
  face->visited = !face->visited;
  for (auto child : face->children) {
    visitChildrenWithFlag(child, visitedValue, func);
  }
}

void QuickHullAlgorithm::visitChildren(FaceInfoPtr face,
                                       const std::function<void(FaceInfoPtr)> &func) {
  visitChildrenWithFlag(face, true, func);
  visitChildrenWithFlag(face, false);
}

void QuickHullAlgorithm::visitParentsWithFlag(FaceInfoPtr face, bool visitedValue,
                                              std::function<void(FaceInfoPtr)> const &func) {
  if (face->visited == visitedValue) {
    return;
  }

  func(face);
  face->visited = !face->visited;
  for (auto parent : face->parents) {
    visitParentsWithFlag(parent, visitedValue, func);
  }
}

void QuickHullAlgorithm::initializeSimplex() {
  if (vertices.size() < dim + 1) {
    throw std::runtime_error("QuickHullAlgorithm::initializeSimplex(): vertices.size() < dim + 1");
  }

  math::NatSet simplexVertices;
  math::AffineSubspace subspace(vertices[0]);
  simplexVertices.insert(0);
  std::forward_list<math::NatSet::NatType> unprocessedVertices;

  size_t remainingDimensions = dim;

  // find dim+1 affinely independent vertices and add the rest to
  // unprocessedVertices
  for (size_t i = 1; i < vertices.size(); ++i) {
    if (remainingDimensions > 0 && !subspace.containsVector(vertices[i])) {
      subspace.addIndependentVector(vertices[i]);
      simplexVertices.insert(i);
      --remainingDimensions;
    } else {
      unprocessedVertices.push_front(i);
    }
  }

  if (remainingDimensions > 0) {
    throw std::runtime_error(
        "QuickHullAlgorithm::initializeSimplex(): not enough affinely "
        "independent vertices");
  }

  // compute barycenter of initial simplex
  for (uint32_t idx : simplexVertices) {
    innerPoint += vertices[idx];
  }
  innerPoint *= 1.0 / (dim + 1);

  std::vector<std::unordered_map<math::NatSet, FaceInfoPtr, math::NatSetHash>> initialFaces(dim);

  // create faces of dimension 0
  for (uint32_t idx : simplexVertices) {
    auto ptr = new FaceInfo();
    ptr->vertexSet.insert(idx);
    ptr->numAncestorFacets = dim;
    initialFaces[0].insert({ptr->vertexSet, ptr});
    vertexFaces.insert(ptr);
  }

  // create faces of dimension >= 1
  for (size_t d = 1; d < dim; ++d) {
    // create faces of dimension d
    for (auto it = initialFaces[d - 1].begin(); it != initialFaces[d - 1].end(); ++it) {
      auto ptr = it->second;

      for (uint32_t idx : simplexVertices) {
        if (!ptr->vertexSet.contains(idx)) {
          auto vertexSet = ptr->vertexSet;
          vertexSet.insert(idx);
          auto parentIt = initialFaces[d].find(vertexSet);
          if (parentIt == initialFaces[d].end()) {
            auto parent =
                (d + 1 == dim) ? static_cast<FaceInfoPtr>(new FacetInfo()) : new FaceInfo();
            parent->vertexSet = vertexSet;
            parent->numAncestorFacets = dim - d;
            parent->children.insert(ptr);
            ptr->parents.insert(parent);
            initialFaces[d].insert({vertexSet, parent});
          } else {
            auto parent = parentIt->second;
            parent->children.insert(ptr);
            ptr->parents.insert(parent);
          }
        }
      }
    }
  }

  // initialize facets and put them in facets set and unprocessedFacets queue
  for (auto val : initialFaces[dim - 1]) {
    auto facet = dynamic_cast<FacetInfoPtr>(val.second);

    // compute normal vector and offset
    setFacetNormal(facet, facet->vertexSet);

    facets.insert(facet);
    unprocessedFacets.push(facet);
  }

  // TODO: might be able to remove that if we don't use vertices in the loops above but only the
  // first hashmap entry

  // clear vertex sets for non-corners because they are filled at the end of the
  // algorithm when the corners may have changed
  for (size_t d = 1; d < dim; ++d) {
    for (auto it = initialFaces[d].begin(); it != initialFaces[d].end(); ++it) {
      it->second->vertexSet.clear();
    }
  }

  // initialize outside sets
  for (auto i : unprocessedVertices) {
    for (auto it = facets.begin(); it != facets.end(); ++it) {
      if ((*it)->distanceSign(vertices[i]) > 0) {
        (*it)->outsideSet.push_back(i);
        break;
      }
    }
  }
}

void QuickHullAlgorithm::computeNeighbors(uint32_t pointIndex, FacetInfoPtr facet) {
  std::queue<FacetInfoPtr> unprocessedNeighborFacets;
  std::unordered_set<FacetInfoPtr> visitedFacets;
  unprocessedNeighborFacets.push(facet);
  visitedFacets.insert(facet);
  auto point = vertices[pointIndex];

  while (!unprocessedNeighborFacets.empty()) {
    auto facet = unprocessedNeighborFacets.front();
    unprocessedNeighborFacets.pop();
    auto sign = facet->distanceSign(point);
    if (sign == -1) {
      continue;
    }

    if (sign == 0) {
      insideFacets.push_back(facet);
    } else {
      aboveFacets.push_back(facet);
    }

    for (auto face : facet->children) {
      for (auto parentFace : face->parents) {
        auto parentFacet = dynamic_cast<FacetInfoPtr>(parentFace);
        if (visitedFacets.find(parentFacet) == visitedFacets.end()) {
          unprocessedNeighborFacets.push(parentFacet);  // not found, so push
          visitedFacets.insert(parentFacet);
        }
      }
    }
  }
}

void QuickHullAlgorithm::prepareConnect(FaceInfoPtr face,
                                        std::unordered_set<FaceInfoPtr> const &otherFaces) {
  for (auto otherFace : otherFaces) {
    auto status = otherFace->getStatus();
    if (status == FaceStatus::CONNECT) {
      otherFace->facesToConnect.insert(face);
      newConnectFaces.insert(otherFace);
    } else if (status == FaceStatus::REMOVE) {
      // check parents for extend
      for (auto parent : otherFace->parents) {
        auto parentStatus = parent->getStatus();
        if (parentStatus == FaceStatus::EXTEND) {
          parent->children.insert(face);
          face->parents.insert(parent);
          // face->numAncestorFacets += parent->numAncestorFacets;
          // TODO: verify if it is actually true that there can only be one
          // extend grandparent of a connect face
          break;  // this is necessary because the parent might have been
          // visited several times otherwise
        }
      }
    }
  }
}

void QuickHullAlgorithm::deleteFaceRecursively(FaceInfoPtr face) {
  if (face->isVertex()) {
    removeFromSet(vertexFaces, face);
  }

  for (auto parent : face->parents) {
    removeFromSet(parent->children, face);
  }

  for (auto child : face->children) {
    removeFromSet(child->parents, face);
    // don't care about extend, since p and numAncestorFacets have already been decreased
    if (child->shouldRemove) {
      deleteFaceRecursively(child);
    }

    /*else {
      child->numAncestorFacets--;
    }*/
  }

  delete face;
}

void QuickHullAlgorithm::deleteChildrenRecursively(FaceInfoPtr face) {
  for (auto child : face->children) {
    removeFromSet(child->parents, face);
    if (child->parents.empty()) {
      deleteChildrenRecursively(child);
    }
  }

  delete face;
}

/**
 * Todos:
 * - possibly rename connectFaces to possibleConnectFaces etc.?
 * - create convenience function for computing vertexSet via visitChildren()
 * - refactoring of addPoint()
 * - testing of single methods
 * - convenience function for group visits
 * - computeNeighbors might also use the visited flag (but then would have to set it to false again)
 * - re-test initializeSimplex()?
 */

void QuickHullAlgorithm::addPoint(uint32_t pointIndex, FacetInfoPtr facet) {
  // computeNeighbor facets and add them to aboveFacets and insideFacets
  computeNeighbors(pointIndex, facet);

  // Compute p/z values for all children of visible faces
  // for all facets in aboveFacets, increase the p value of their children
  // if it is a corner, add it to pCorners
  visitChildrenForEach(aboveFacets, [this](FaceInfoPtr face) {
    face->p++;
    if (face->isVertex()) {
      connectFaces.insert(face);
    }
  });
  // for all facets in insideFacets, increase the z value of their children
  visitChildrenForEach(insideFacets, [](FaceInfoPtr face) { face->z++; });

  auto newCorner = new FaceInfo();
  newCorner->vertexSet.insert(pointIndex);
  vertexFaces.insert(newCorner);

  // not needed because of adjustment below:
  // newCorner->numAncestorFacets = insideFacets.size();

  // prepare connection of corners
  prepareConnect(newCorner, connectFaces);

  // connect faces but not hyperfacets
  for (size_t d = 1; d + 1 < dim; ++d) {
    connectFaces.clear();
    connectFaces.swap(newConnectFaces);

    for (auto face : connectFaces) {
      auto currentConnectFaces = face->parents;
      FaceInfoPtr newFace = new FaceInfo();
      newFace->children.swap(face->facesToConnect);
      newFace->children.insert(face);
      for (auto child : newFace->children) {
        child->parents.insert(newFace);
      }

      // prepareConnect(newFace, face->parents);  // TODO: hier ist auch newFace rechts mit drin
      prepareConnect(newFace, currentConnectFaces);
    }
  }

  // connect hyperfacets
  for (auto face : newConnectFaces) {
    // create Facet
    auto newFacet = new FacetInfo();
    newFacet->children.swap(face->facesToConnect);
    newFacet->children.insert(face);
    for (auto child : newFacet->children) {
      child->parents.insert(newFacet);
    }

    std::vector<size_t> facetVertexIndices;

    // track all corners
    visitChildren(newFacet, [&facetVertexIndices](FaceInfoPtr face) {
      face->numAncestorFacets++;
      if (face->isVertex()) {
        facetVertexIndices.push_back(face->vertexSet.anElement());
      }
    });

    setFacetNormal(newFacet, facetVertexIndices);

    newFacets.push_back(newFacet);
    facets.insert(newFacet);
    unprocessedFacets.push(newFacet);
  }

  visitChildrenOnce(aboveFacets, [](FaceInfoPtr face) {
    if (face->getStatus() == FaceStatus::REMOVE) {
      face->shouldRemove = true;
    }
  });

  visitChildrenForEach(aboveFacets, [](FaceInfoPtr face) {
    face->numAncestorFacets--;
    face->p--;
  });

  /**
   * for each point p in the removed faces' visible sets:
   *   assign p to one of the new faces, if possible (not to the inside
   * 	 facets, because it is already known that it won't be above)
   * remove old faces (if not already done)
   */
  for (auto f : aboveFacets) {
    for (auto pointIndex : f->outsideSet) {
      bool found = false;
      for (auto fnew : newFacets) {
        if (fnew->distanceSign(vertices[pointIndex]) > 0) {
          fnew->outsideSet.push_back(pointIndex);
          found = true;
          break;
        }
      }
      if (!found) {
        for (auto fnew : insideFacets) {
          if (fnew->distanceSign(vertices[pointIndex]) > 0) {
            fnew->outsideSet.push_back(pointIndex);
            found = true;
            break;
          }
        }
      }
    }

    // delete facet
    removeFromSet(facets, f);
    /*visitChildren(f, [](FaceInfoPtr face) {
      face->numAncestorFacets--;
      face->p--;
    });*/
    deleteFaceRecursively(f);
    // don't delete facet from unprocessedFacets because this is already taken
    // care of in compute()
  }

  // reset p and z values
  // first resetting all facets and then setting visited to false again is
  // more efficient than directly resetting
  visitChildrenForEach(insideFacets, [](FaceInfoPtr face) {
    if (face->z == 0) {
      face->numAncestorFacets++;
    } else {
      face->z--;
    }
  });

  // Clear lists
  aboveFacets.clear();
  insideFacets.clear();
  newFacets.clear();
  connectFaces.clear();
  newConnectFaces.clear();
}

void QuickHullAlgorithm::compute() {
  initializeSimplex();

  /*
   * For each facet in queue:
   *   If outside set is empty, continue
   *   Select the furthest point p of the outside set
   *	 addPoint(p, F)
   */
  while (!unprocessedFacets.empty()) {
    auto facet = unprocessedFacets.front();
    unprocessedFacets.pop();

    // check if facet has already been deleted
    if (facets.find(facet) == facets.end()) {
      continue;
    }

    if (!facet->outsideSet.empty()) {
      double maxDist = -1.0;
      uint32_t maxIndex = 0;
      for (auto it = facet->outsideSet.begin(); it != facet->outsideSet.end(); ++it) {
        uint32_t index = *it;
        double dist = facet->signedDistance(vertices[index]);
        if (dist > maxDist) {
          maxDist = dist;
          maxIndex = index;
        }
      }

      addPoint(maxIndex, facet);
    }
  }

  // compute the vertex sets inside the faces
  for (auto vertexFace : vertexFaces) {
    auto vertexIndex = vertexFace->vertexSet.anElement();
    visitParentsWithFlag(vertexFace, true,
                         [vertexIndex](FaceInfoPtr face) { face->vertexSet.insert(vertexIndex); });
    visitParentsWithFlag(vertexFace, false);
  }
}

void QuickHullAlgorithm::addToPolytopeRecursively(FaceInfoPtr face, size_t d) {
  size_t index = polytope->faces[d].size();
  ConvexPolytope::Face polytopeFace;
  polytopeFace.vertices = face->vertexSet;
  polytopeFace.dim = d;

  // visit parents
  for (auto parent : face->parents) {
    if (!parent->visited) {
      addToPolytopeRecursively(parent, d + 1);
    }

    polytopeFace.parentIndexes.push_back(parent->polytopeIndex);
    polytope->faces[d + 1][parent->polytopeIndex].childIndexes.push_back(index);
  }

  face->polytopeIndex = index;

  polytope->faces[d].push_back(polytopeFace);

  face->visited = true;
}

std::shared_ptr<ConvexPolytope> QuickHullAlgorithm::createPolytope() {
  compute();

  // generation from each vertex
  for (auto vertexFace : vertexFaces) {
    addToPolytopeRecursively(vertexFace);
  }

  // also generate the interior of the polytope
  ConvexPolytope::Face interiorFace;
  interiorFace.dim = dim;
  for (size_t i = 0; i < polytope->faces[dim - 1].size(); ++i) {
    interiorFace.childIndexes.push_back(i);
    polytope->faces[dim - 1][i].parentIndexes.push_back(0);
  }
  for (auto &polytopeVertex : polytope->faces[0]) {
    interiorFace.vertices.insert(polytopeVertex.vertices.anElement());
  }
  polytope->faces[dim].push_back(interiorFace);

  return polytope;
}

} /* namespace geo */
} /* namespace sfcpp */
