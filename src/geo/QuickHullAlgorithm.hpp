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



#pragma once

#include <Eigen/Dense>

#include <geo/ConvexPolytope.hpp>
#include <math/AffineSubspace.hpp>
#include <math/NatSet.hpp>

#include <forward_list>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace sfcpp {
namespace geo {

template <typename... Args>
std::function<void(Args...)> getDummyFunction() {
  return std::function<void(Args...)>([](Args...) {});
}

enum class FaceStatus { REMOVE, EXTEND, CONNECT, KEEP };

struct FaceInfo;

typedef FaceInfo *FaceInfoPtr;

static const double QH_EPSILON =
    1e-9;  // TODO: make relative instead of absolute

struct FaceInfo {
  math::NatSet vertexSet;

  std::unordered_set<FaceInfoPtr> parents;
  std::unordered_set<FaceInfoPtr> children;
  std::unordered_set<FaceInfoPtr> facesToConnect;

  // number of facets this face is belonging to
  uint32_t numAncestorFacets = 0;

  // number of ancestor facets where the currently processed point is on the
  // other side of the facet's hyperplane compared to the current polytope
  uint32_t p = 0;

  // number of ancestor facets where the currently processed point is inside the
  // facet's hyperplane
  uint32_t z = 0;

  bool shouldRemove = false;

  // for generation of Polytope
  size_t polytopeIndex = 0;  // corresponding index in polytope.faces[d] vector

  bool visited = false;

  virtual ~FaceInfo() {}

  FaceStatus getStatus() const {
    uint32_t n = numAncestorFacets - p - z;
    if (n > 0) {
      if (p > 0) {
        return FaceStatus::CONNECT;
      } else {
        return FaceStatus::KEEP;
      }
    } else {
      if (p > 0) {
        return FaceStatus::REMOVE;
      } else {
        return FaceStatus::EXTEND;
      }
    }
  }

  static std::function<void(FaceInfoPtr)> dummyFunction;

  bool isVertex() const { return vertexSet.size() == 1; }
};

struct FacetInfo : public FaceInfo {
  Eigen::VectorXd outerNormalVector;

  // dot product of any point of the face with outerNormalVector
  double offset = 0.0;

  std::vector<uint32_t> outsideSet;

  virtual ~FacetInfo() {}

  double signedDistance(Eigen::VectorXd v) const {
    return v.dot(outerNormalVector) - offset;
  }

  int distanceSign(Eigen::VectorXd v) {
    double signedDist = signedDistance(v);
    return signedDist > QH_EPSILON ? 1 : (signedDist < -QH_EPSILON ? -1 : 0);
  }
};

template <typename Set, typename T>
void removeFromSet(Set &set, T t) {
  auto it = set.find(t);
  if (it != set.end()) {
    set.erase(it);
  }
}

typedef FacetInfo *FacetInfoPtr;

/**
 * A custom implementation of the QuickHull algorithm that computes all faces of
 * the given polytope and also works if the facets are non-simplicial (which is
 * important for cube-based SFC, for example). This algorithm works in arbitrary
 * dimensions >= 2.
 */
class QuickHullAlgorithm {
  std::vector<Eigen::VectorXd> vertices;

  // an inner point of the polytope used compute the direction of facet normal
  // vectors
  Eigen::VectorXd innerPoint;
  size_t dim;

  std::unordered_set<FaceInfoPtr> vertexFaces;
  std::unordered_set<FacetInfoPtr> facets;
  std::vector<FacetInfoPtr> aboveFacets, insideFacets, newFacets;

  std::queue<FacetInfoPtr> unprocessedFacets;
  std::unordered_set<FaceInfoPtr> connectFaces;
  std::unordered_set<FaceInfoPtr> newConnectFaces;

  std::shared_ptr<ConvexPolytope> polytope;

  void visitChildrenWithFlag(
      FaceInfoPtr face, bool visitedValue,
      std::function<void(FaceInfoPtr)> const &func = FaceInfo::dummyFunction);
  void visitChildren(FaceInfoPtr face,
                     std::function<void(FaceInfoPtr)> const &func);

  void visitParentsWithFlag(
      FaceInfoPtr face, bool visitedValue,
      std::function<void(FaceInfoPtr)> const &func = FaceInfo::dummyFunction);

  template <typename Collection>
  void visitChildrenForEach(Collection const &collection,
                            std::function<void(FaceInfoPtr)> const &func) {
    for (auto elem : collection) {
      visitChildrenWithFlag(elem, true, func);
      visitChildrenWithFlag(elem, false);
    }
  }

  template <typename Collection>
  void visitChildrenOnce(Collection const &collection,
                         std::function<void(FaceInfoPtr)> const &func) {
    for (auto elem : collection) {
      visitChildrenWithFlag(elem, true, func);
    }
    for (auto elem : collection) {
      visitChildrenWithFlag(elem, false);
    }
  }

  void initializeSimplex();

  void computeNeighbors(uint32_t pointIndex, FacetInfoPtr facet);

  void prepareConnect(FaceInfoPtr face,
                      std::unordered_set<FaceInfoPtr> const &otherFaces);

  void deleteFaceRecursively(FaceInfoPtr face);
  void deleteChildrenRecursively(FaceInfoPtr face);

  void addPoint(uint32_t pointIndex, FacetInfoPtr facet);

  void compute();

  void addToPolytopeRecursively(FaceInfoPtr face, size_t d = 0);

  template <typename T>
  void setFacetNormal(FacetInfoPtr facet, T const &vertexIndices) {
    if (vertexIndices.size() == 0) {
      throw std::runtime_error(
          "QuickHullAlgorithm::setFacetNormal(): vertexIndices.size() == 0");
    }
    auto it = vertexIndices.begin();
    auto firstPoint = this->vertices[*it];
    math::AffineSubspace affSpace(firstPoint);

    ++it;
    for (; it != vertexIndices.end(); ++it) {
      affSpace.addVector(this->vertices[*it]);
    }

    auto vec = affSpace.orthogonalVector().normalized();

    if (vec.dot(firstPoint - innerPoint) < 0) {
      vec *= -1;
    }

    facet->outerNormalVector = vec;
    facet->offset = vec.dot(firstPoint);
  }

 public:
  QuickHullAlgorithm(Eigen::MatrixXd points);

  ~QuickHullAlgorithm();

  std::shared_ptr<ConvexPolytope> createPolytope();
};

} /* namespace geo */
} /* namespace sfcpp */
