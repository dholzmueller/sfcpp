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



#include "CurveInformation.hpp"

#include <files/files.hpp>
#include <math/CompletionAlgorithm.hpp>
#include <math/NatSet.hpp>
#include <strings/strings.hpp>

#include <iostream>
#include <queue>

namespace sfcpp {
namespace sfc {

static const size_t UNDEFINED = static_cast<size_t>(-1);

bool CurveInformation::tryFindAdjacentFacets(GeometricTreeNode& first,
                                             GeometricTreeNode& second,
                                             size_t& firstFacet,
                                             size_t& secondFacet) {
  double eps = 1e-9;  // TODO(holzmudd): adjust
  double sqeps = eps * eps;

  math::NatSet firstVertexSet;
  math::NatSet secondVertexSet;

  for (int firstCol = 0; firstCol < first.points.cols(); ++firstCol) {
    for (int secondCol = 0; secondCol < second.points.cols(); ++secondCol) {
      double sqnorm =
          (first.points.col(firstCol) - second.points.col(secondCol))
              .squaredNorm();

      if (sqnorm < sqeps) {
        firstVertexSet.insert(firstCol);
        secondVertexSet.insert(secondCol);
      }
    }
  }

  return polytopeStructures[first.state]->tryFindFace(
             spec->d - 1, firstVertexSet, firstFacet) &&
         polytopeStructures[second.state]->tryFindFace(
             spec->d - 1, secondVertexSet, secondFacet);
}

struct TreeNodeExample {
  size_t state = 0;
  Eigen::MatrixXd points;

  friend bool operator==(TreeNodeExample const& first,
                         TreeNodeExample const& second) {
    return first.state == second.state;
  }
};

class TreeNodeStateHash {
 public:
  size_t operator()(GeometricTreeNode const& ex) const { return ex.state; }
};

class TreeNodeStateEquals {
 public:
  bool operator()(GeometricTreeNode const& first,
                  GeometricTreeNode const& second) const {
    return first.state == second.state;
  }
};

struct TreeNodePairExample {
  GeometricTreeNode first;
  GeometricTreeNode second;
  size_t firstFacet = 0;
  size_t secondFacet = 0;

  TreeNodePairExample(GeometricTreeNode const& first,
                      GeometricTreeNode const& second, size_t firstFace,
                      size_t secondFace)
      : first(first),
        second(second),
        firstFacet(firstFace),
        secondFacet(secondFace) {}

  TreeNodePairExample() {}

  friend bool operator==(TreeNodePairExample const& first,
                         TreeNodePairExample const& second) {
    return (first.first.state == second.first.state &&
            first.second.state == second.second.state &&
            first.firstFacet == second.firstFacet &&
            first.secondFacet == second.secondFacet) ||
           (first.first.state == second.second.state &&
            first.second.state == second.first.state &&
            first.firstFacet == second.secondFacet &&
            first.secondFacet == second.firstFacet);
  }
};

class TreeNodePairExampleHash {
 public:
  size_t operator()(TreeNodePairExample const& ex) const {
    size_t someNumber = 1234567;
    // swapping roles of first and second must make no difference
    // => use commutative operation "+"
    return ex.first.state + ex.second.state +
           someNumber * (ex.firstFacet + ex.secondFacet);
  }
};

void CurveInformation::computeInformation() {
  // for computing neighborTable: visit each reachable state and consider its
  // children.
  // for computing opponentTable: visit each occuring pair of states (initialize
  // in the previous
  // loop?) -> pairs might occur from same parent or other neighboring pairs

  std::vector<bool> visitedStates(spec->getNumStates(), false);
  std::vector<std::vector<bool>> visitedStatePairs(spec->getNumStates(),
                                                   visitedStates);

  math::CompletionAlgorithm<GeometricTreeNode, TreeNodeStateHash,
                            TreeNodeStateEquals>
      nodeAlg(getRootNode());
  math::CompletionAlgorithm<TreeNodePairExample, TreeNodePairExampleHash>
      pairAlg;

  // compute the polytope structures beforehand because we need them
  nodeAlg.computeSingleCompletion([&](GeometricTreeNode const& node) {
    stateReachability[node.state] = true;
    polytopeStructures[node.state] =
        geo::ConvexPolytope::convexHull(node.points);
    // std::cout << "Polytope structure: \n" << *polytopeStructures[node.state]
    // << "\n\n";
    for (auto& child : getChildren(node)) {
      nodeAlg.add(child);
    }
  });

  // now compute neighbor table and initialize pairAlg with starting pairs
  for (auto& node : nodeAlg.getResult()) {
    // traverse a node of each possible state
    auto children = getChildren(node);
    for (size_t i = 0; i < children.size(); ++i) {
      for (size_t j = i + 1; j < children.size(); ++j) {
        size_t firstFacet;
        size_t secondFacet;

        // for each pair of children of the current node, look if they share a
        // common facet
        bool success = tryFindAdjacentFacets(children[i], children[j],
                                             firstFacet, secondFacet);
        // std::cout << "firstFacet, secondFacet: " << firstFacet << ", " <<
        // secondFacet << "\n";
        if (success) {
          // if yes, this yields an entry in the neighbor table and a starting
          // pair for pairAlg
          neighborTable.at(i, node.state, firstFacet) = j;
          neighborTable.at(j, node.state, secondFacet) = i;
          pairAlg.add(TreeNodePairExample(children[i], children[j], firstFacet,
                                          secondFacet));
        }
      }
    }
  }

  // start pairAlg to compute opponentTable and parentFacetTable
  pairAlg.computeSingleCompletion([&](TreeNodePairExample const& pair) {
    auto firstChildren = getChildren(pair.first);
    auto secondChildren = getChildren(pair.second);

    // for pairs of children, one per cell in the pair of cells
    for (size_t i = 0; i < firstChildren.size(); ++i) {
      for (size_t j = 0; j < secondChildren.size(); ++j) {
        size_t firstFacet;
        size_t secondFacet;

        // look if the pair shares a common facet
        bool success = tryFindAdjacentFacets(
            firstChildren[i], secondChildren[j], firstFacet, secondFacet);
        if (success) {
          // if yes, compute the entries for the tables and add the newly found
          // pair to pairAlg
          if (opponentTable.containsNotDefault(i, pair.first.state,
                                               pair.second.state, firstFacet) &&
              opponentTable.at(i, pair.first.state, pair.second.state,
                               firstFacet) != j) {
            opponentTableInconsistent = true;
          }
          opponentTable.at(i, pair.first.state, pair.second.state, firstFacet) =
              j;

          if (opponentTable.containsNotDefault(j, pair.second.state,
                                               pair.first.state, secondFacet) &&
              opponentTable.at(j, pair.second.state, pair.first.state,
                               secondFacet) != i) {
            opponentTableInconsistent = true;
          }
          opponentTable.at(j, pair.second.state, pair.first.state,
                           secondFacet) = i;
          parentFacetTable.at(i, pair.first.state, firstFacet) =
              pair.firstFacet;
          parentFacetTable.at(j, pair.second.state, secondFacet) =
              pair.secondFacet;
          pairAlg.add(TreeNodePairExample(firstChildren[i], secondChildren[j],
                                          firstFacet, secondFacet));
        }
      }
    }
  });

  // check palindrome property
  for (size_t j = 0; j < opponentTable.getSize(); ++j) {
    size_t oppositeValue = spec->getNumChildren() - 1 - j;
    auto& t2 = opponentTable[j];
    for (auto& t3 : t2) {
      for (auto& t4 : t3) {
        for (auto value : t4) {
          if (value != UNDEFINED && value != oppositeValue) {
            hasPalindromeProperty = false;
            return;
          }
        }
      }
    }
  }

  // put nothing here because of the return inside the loops
}

CurveInformation::CurveInformation(std::shared_ptr<CurveSpecification> spec)
    : spec(spec),
      polytopeStructures(spec->getNumStates()),
      stateReachability(spec->getNumStates(), false),
      neighborTable(UNDEFINED),
      opponentTable(UNDEFINED),
      parentFacetTable(UNDEFINED),
      opponentTableInconsistent(false),
      hasPalindromeProperty(true) {
  computeInformation();
  if (opponentTableInconsistent) {
    std::cout
        << "CurveInformation::CurveInformation(): ATTENTION: opponentTable "
           "is inconsistent!\n";
  } else {
    if (hasPalindromeProperty) {
      std::cout << "The curve has the modified palindrome property.\n";
    } else {
      std::cout
          << "The curve does not have the modified palindrome property.\n";
    }
  }
}

std::shared_ptr<geo::ConvexPolytope> CurveInformation::getPolytopeForState(
    size_t state) const {
  return polytopeStructures[state];
}

GeometricTreeNode CurveInformation::getRootNode() const {
  return GeometricTreeNode(0, spec->rootPoints);
}

std::vector<GeometricTreeNode> CurveInformation::getChildren(
    const GeometricTreeNode& node) const {
  std::vector<GeometricTreeNode> result(spec->getNumChildren());
  // TODO: use this method in renderer?

  for (size_t i = 0; i < result.size(); ++i) {
    size_t nextState = spec->grammar[node.state][i];

    result[i] = GeometricTreeNode(
        nextState, node.points * spec->transitionMats[node.state][i]);
  }

  return result;
}

void CurveInformation::saveTableDefinitions(std::string filename) {
  // State information
  data::MultidimArray<size_t, 2> pStateTable, cStateTable;

  for (size_t pstate = 0; pstate < spec->grammar.size(); ++pstate) {
    for (size_t i = 0; i < spec->grammar[pstate].size(); ++i) {
      size_t cstate = spec->grammar[pstate][i];
      cStateTable.at(pstate, i) = cstate;
      pStateTable.at(cstate, i) = pstate;
    }
  }

  // TODO: pStateTable does only make sense if grammar has group structure...

  std::string result =
      pStateTable.getCppArrayDeclaration("pStateTable", "size_t") + "\n\n" +
      cStateTable.getCppArrayDeclaration("cStateTable", "size_t") + "\n\n" +
      neighborTable.getCppArrayDeclaration("nTable", "size_t") + "\n\n" +
      opponentTable.getCppArrayDeclaration("oTable", "size_t") + "\n\n" +
      parentFacetTable.getCppArrayDeclaration("pFacetTable", "size_t") + "\n";

  files::writeToFile(filename, result);
}

std::ostream& operator<<(std::ostream& stream,
                         CurveInformation const& curveInfo) {
  stream << "neighborTable: \n"
         << strings::toString(curveInfo.neighborTable, "\n\n", "\n", ", ")
         << "\n\n\n";
  stream << "opponentTable: \n"
         << strings::toString(curveInfo.opponentTable, "\n--------\n", "\n\n",
                              "\n", ", ")
         << "\n\n\n\n";
  stream << "parentFacetTable: \n"
         << strings::toString(curveInfo.parentFacetTable, "\n\n", "\n", ", ");

  return stream;
}

} /* namespace sfc */
} /* namespace sfcpp */
