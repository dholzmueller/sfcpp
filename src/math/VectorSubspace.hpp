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

#include <iostream>

namespace sfcpp {
namespace math {

class VectorSubspace {
  Eigen::MatrixXd baseMat, kernelMat;
  size_t numCoordinates;
  static constexpr double myeps = 1e-8;
  static constexpr double mysqeps = myeps * myeps;

 public:
  VectorSubspace(size_t numCoordinates)
      : baseMat(Eigen::MatrixXd::Zero(numCoordinates, 1)),
        kernelMat(Eigen::MatrixXd::Identity(numCoordinates, numCoordinates)),
        numCoordinates(numCoordinates) {}

  void addVector(Eigen::VectorXd const &vec) {
    if (!containsVector(vec)) {
      addIndependentVector(vec);
    }
  }

  void addIndependentVector(Eigen::VectorXd const &vec) {
    baseMat.conservativeResize(baseMat.rows(), baseMat.cols() + 1);
    baseMat.col(baseMat.cols() - 1) = vec;

    kernelMat = baseMat.transpose().fullPivLu().kernel();
  }

  bool containsVector(Eigen::VectorXd const &vec) const {
    return (vec.transpose() * kernelMat).squaredNorm() <= mysqeps;
  }

  bool isOrthogonalTo(Eigen::VectorXd const &vec) const {
    return (vec.transpose() * baseMat).squaredNorm() <= mysqeps;
  }

  /**
   * @return a vector orthogonal to the subspace, which is nonzero iff the
   * subspace is not the whole space.
   */
  Eigen::VectorXd orthogonalVector() { return kernelMat.col(0); }
};

} /* namespace math */
} /* namespace sfcpp */
