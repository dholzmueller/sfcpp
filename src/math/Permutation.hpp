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

#include <strings/strings.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

namespace sfcpp {
namespace math {

class Permutation {
  std::vector<size_t> functionValues;

 public:
  Permutation(size_t length) : functionValues(length) {}

  void set(size_t value, size_t image) {
    while (value >= functionValues.size()) {
      functionValues.push_back(functionValues.size());
    }
    functionValues[value] = image;
  }

  size_t get(size_t value) const {
    if (value >= functionValues.size()) {
      return value;
    }
    return functionValues[value];
  }

  Permutation chain(Permutation const &other) const {
    size_t maxSize =
        std::max(functionValues.size(), other.functionValues.size());
    Permutation result(maxSize);

    for (size_t i = 0; i < maxSize; ++i) {
      result.set(i, get(other.get(i)));
    }

    return result;
  }

  Permutation inverse() const {
    Permutation result(functionValues.size());
    for (size_t i = 0; i < functionValues.size(); ++i) {
      result.set(get(i), i);
    }
    return result;
  }

  static Permutation identity;

  friend bool operator==(Permutation const &first, Permutation const &second) {
    size_t maxSize =
        std::max(first.functionValues.size(), second.functionValues.size());
    for (size_t i = 0; i < maxSize; ++i) {
      if (first.get(i) != second.get(i)) {
        return false;
      }
    }
    return true;
  }

  friend std::ostream &operator<<(std::ostream &stream, Permutation const &p) {
    if (p.functionValues.empty()) {
      stream << "identity";
    } else {
      stream << strings::toString(p.functionValues, ", ");
    }
    return stream;
  }
};

} /* namespace math */
} /* namespace sfcpp */
