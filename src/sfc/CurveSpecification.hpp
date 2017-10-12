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

#include <geo/ConvexPolytope.hpp>

#include <Eigen/Dense>
#include <memory>
#include <ostream>
#include <vector>

namespace sfcpp {
namespace sfc {
/**
 * This class can be used to specify a curve geometrically.
 */
struct CurveSpecification {
  // Dimension of embedding space
  size_t d;
  // Matrix containing the points of the root polytope as columns
  Eigen::MatrixXd rootPoints;
  // A lookup table representing the child state function S^c
  std::vector<std::vector<size_t>> grammar;
  // Transition matrices, corresponding to the matrices M^{s,j} from the thesis.
  std::vector<std::vector<Eigen::MatrixXd>> transitionMats;

  /**
   * This equals b in the paper (the branching factor).
   */
  size_t getNumChildren() const { return grammar[0].size(); }

  size_t getNumStates() const { return grammar.size(); }

  /**
   * For d = 2, this function returns a local model of the Sierpinski curve.
   * For d > 2, it produces a local model of a Sierpinski-like d-dimensional curve which apparently
   * converges to a continuous curve but whose simplices appear to degenerate in shape with
   * increasing level.
   */
  static std::shared_ptr<CurveSpecification> getSierpinskiCurveSpecification(size_t d = 2);
  /**
   * A custom curve whose limit curve is not continuous.
   */
  static std::shared_ptr<CurveSpecification> getCustomCurveSpecification1();

  /**
   * Semi-local model of the Gosper curve.
   */
  static std::shared_ptr<CurveSpecification> getGosperCurveSpecification();

  /**
   * Semi-local model of the beta Omega curve.
   */
  static std::shared_ptr<CurveSpecification> getBetaOmegaCurveSpecification();

  friend std::ostream &operator<<(std::ostream &stream, CurveSpecification const &spec);
};

} /* namespace sfc */
} /* namespace sfcpp */
