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

#include <math/NatSet.hpp>

#include <Eigen/Dense>

#include <memory>
#include <ostream>
#include <vector>

namespace sfcpp {
namespace geo {

/**
 * Stores the combinatorial information of a polytope, i. e. the indices of
 * vertices contained in each face.
 */
class ConvexPolytope {
 public:
  struct Face {
    math::NatSet vertices;
    std::vector<size_t> parentIndexes;
    std::vector<size_t> childIndexes;
    size_t dim;

    friend std::ostream &operator<<(std::ostream &stream,
                                    ConvexPolytope::Face const &face);
  };

  std::vector<std::vector<Face>> faces;

  ConvexPolytope(size_t d);

  bool tryFindFace(size_t dim, math::NatSet const &vertexSet,
                   size_t &faceIndex) const;

  static std::shared_ptr<ConvexPolytope> convexHull(Eigen::MatrixXd points);

  friend std::ostream &operator<<(std::ostream &stream,
                                  ConvexPolytope const &polytope);

  void sort();

  size_t getDimension();

  // TODO: method for comparing two ConvexPolytopes?
};

} /* namespace geo */
} /* namespace sfcpp */
