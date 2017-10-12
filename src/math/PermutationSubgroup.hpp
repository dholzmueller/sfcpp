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

#include <math/Permutation.hpp>

#include <queue>

namespace sfcpp {
namespace math {

class PermutationSubgroup {
  std::vector<Permutation> permutations;

 public:
  PermutationSubgroup() : permutations(1, Permutation::identity) {}

  void add(Permutation permutation) {
    std::queue<Permutation> remaining;
    remaining.push(permutation);

    while (!remaining.empty()) {
      auto p = remaining.front();
      remaining.pop();

      if (!contains(p)) {
        permutations.push_back(p);
        remaining.push(p.inverse());
        for (auto q : permutations) {
          remaining.push(q.chain(p));
          remaining.push(p.chain(q));
        }
      }
    }
  }

  bool contains(Permutation const &permutation) {
    for (auto &p : permutations) {
      if (p == permutation) {
        return true;
      }
    }

    return false;
  }

  size_t getSize() { return permutations.size(); }

  friend std::ostream &operator<<(std::ostream &stream, PermutationSubgroup const &g) {
    stream << strings::toString(g.permutations, "\n");
    return stream;
  }
};

} /* namespace math */
} /* namespace sfcpp */
