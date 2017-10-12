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

#include <math/VectorSubspace.hpp>

namespace sfcpp {
namespace math {

class AffineSubspace {
  VectorSubspace vs;
  Eigen::VectorXd anchor;

 public:
  AffineSubspace(Eigen::VectorXd const &v) : vs(v.rows()), anchor(v) {}

  void addVector(Eigen::VectorXd const &vec) { vs.addVector(vec - anchor); }

  void addIndependentVector(Eigen::VectorXd const &vec) {
    vs.addIndependentVector(vec - anchor);
  }

  bool containsVector(Eigen::VectorXd const &vec) const {
    return vs.containsVector(vec - anchor);
  }

  Eigen::VectorXd orthogonalVector() { return vs.orthogonalVector(); }

  Eigen::VectorXd getAnchor() const { return anchor; }
};

} /* namespace math */
} /* namespace sfcpp */
