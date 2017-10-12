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



#include "math.hpp"

#include <stdexcept>

namespace sfcpp {
namespace math {

double bound(double value, double lower, double upper) {
  return value < lower ? lower : (value > upper ? upper : value);
}
}
}

double sfcpp::math::mod(double val, double div) {
  double result = fmod(val, div);
  if (result < 0) {
    result += div;
  }
  return result;
}

float sfcpp::math::mod(float val, float div) {
  float result = fmod(val, div);
  if (result < 0) {
    result += div;
  }
  return result;
}

double sfcpp::math::uniformPointWithBorder(size_t n, size_t i, double start, double end) {
  if (n <= 1) {
    if (n <= 0) {
      throw std::runtime_error("utils::math::uniformPointWithBorder(): n <= 0");
    }

    return (start + end) / 2.0;
  }

  return start + i * (end - start) / (n - 1);
}

double sfcpp::math::uniformPointWithoutBorder(size_t n, size_t i, double start, double end) {
  if (n <= 0) {
    throw std::runtime_error("utils::math::uniformPointWithoutBorder(): n <= 0");
  }

  return start + i * (end - start) / (n + 1);
}
