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

#include <sfc/CurveSpecification.hpp>

#include <Eigen/Dense>

#include <cstddef>
#include <memory>
#include <vector>

namespace sfcpp {
namespace sfc {

/**
 * This class provides a simpler means to specify curves that are based on
 * k^d-trees. An object of this class can be converted to a CurveSpecification
 * object.
 */
struct KDCurveSpecification {
  // Number of cubes per dimension in the subdivision step
  size_t k;
  // Number of dimensions
  size_t d;
  // A lookup table representing the child state function S^c
  std::vector<std::vector<size_t>> grammar;

  // A vector providing the order of subcubes for each state (subcubes are
  // enumerated
  // row-major), for example, in the 3D Hilbert curve, childOrdering[0] might be
  // {0, 1, 3, 2, 6, 7, 5, 4}.
  std::vector<std::vector<size_t>> childOrdering;

  KDCurveSpecification(size_t k, size_t d, size_t numGrammarElements);

  // KDCurveSpecification(size_t k, size_t d, std::initializer_list<std::string>
  // stringGrammar);

  /**
   * Returns a global model of the specified curve.
   */
  std::shared_ptr<CurveSpecification> getCurveSpecification() const;

  /**
   * Returns a local model of the specified curve. Only works for curves where
   * this is possible!
   */
  std::shared_ptr<CurveSpecification> getLocalCurveSpecification() const;

  static Eigen::MatrixXd generateUnitCube(size_t dim);

  /**
   * @param k number of cubes per "row", must be odd and >= 3.
   */
  static std::shared_ptr<KDCurveSpecification> getPeanoCurveSpecification(
      size_t d, size_t k = 3);

  static std::shared_ptr<KDCurveSpecification> getHilbertCurveSpecification(
      size_t d);

  /**
   * @param k number of cubes per "row", can be anything >= 2.
   * A big value of k can be used to emulate row-major layout.
   */
  static std::shared_ptr<KDCurveSpecification> getMortonCurveSpecification(
      size_t d, size_t k = 2);
};

} /* namespace sfc */
} /* namespace sfcpp */
