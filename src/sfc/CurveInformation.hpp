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

#include <data/MultidimArray.hpp>
#include <sfc/CurveSpecification.hpp>

#include <memory>
#include <vector>

namespace sfcpp {
namespace sfc {

struct GeometricTreeNode {
  size_t state;
  Eigen::MatrixXd points;

  GeometricTreeNode(size_t state, Eigen::MatrixXd const &points)
      : state(state), points(points) {}
  GeometricTreeNode() : state(0), points() {}
};

/**
 * This class takes a CurveSpecification and computes information about polytope
 * structures for several states and neighbor-lookup-tables.
 */
class CurveInformation {
  std::shared_ptr<CurveSpecification> spec;

  /**
   * A vector containing a polytope (combinatorial, i.e. without geometric
   * information, just the indices of points contained in each face) for each
   * state
   */
  std::vector<std::shared_ptr<geo::ConvexPolytope>> polytopeStructures;

  /**
   * Stores for each state if it is reachable
   */
  std::vector<bool> stateReachability;

  /**
   * index of child, state of parent, facet index
   */
  data::MultidimArray<size_t, 3> neighborTable;

  /**
   * index of child, state of parent, state of opponents parent, facet index
   */
  data::MultidimArray<size_t, 4> opponentTable;

  /**
   * index of child, state of parent, facet (of child)
   */
  data::MultidimArray<size_t, 3> parentFacetTable;

  bool opponentTableInconsistent;
  bool hasPalindromeProperty;

  /**
   * @return true, if successful.
   * @param first First tree node
   * @param second Second tree node
   * @param firstFacet output parameter for the first facet, if successful.
   * @param secondFacet output parameter for the second facet, if successful.
   */
  bool tryFindAdjacentFacets(GeometricTreeNode &first,
                             GeometricTreeNode &second, size_t &firstFacet,
                             size_t &secondFacet);

  /**
   * Internally used by the constructor to compute the desired information.
   */
  void computeInformation();

 public:
  /**
   * Initializes the CurveInformation object by computing properties of the
   * specification
   */
  CurveInformation(std::shared_ptr<CurveSpecification> spec);

  /**
   * Returns the polytope structure of a node of the given state, assuming that
   * this structure is the same for all nodes with this state
   */
  std::shared_ptr<geo::ConvexPolytope> getPolytopeForState(size_t state) const;

  /**
   * Returns the root node of a corresponding geometric tree
   */
  GeometricTreeNode getRootNode() const;

  /**
   * Returns all children of the given node in a corresponding geometric tree
   */
  std::vector<GeometricTreeNode> getChildren(
      GeometricTreeNode const &node) const;

  /**
   * This saves definitions of all computed lookup tables as C++ arrays into the
   * specified file.
   */
  void saveTableDefinitions(std::string filename);

  friend std::ostream &operator<<(std::ostream &stream,
                                  CurveInformation const &curveInfo);
};

} /* namespace sfc */
} /* namespace sfcpp */
