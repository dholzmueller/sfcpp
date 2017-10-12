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

#include <functional>
#include <queue>
#include <unordered_set>
#include <vector>

namespace sfcpp {
namespace math {

template <typename T, typename Hash = std::hash<T>, typename Equal = std::equal_to<T>>
class CompletionAlgorithm {
  std::unordered_set<T, Hash, Equal> visitedElements;
  std::queue<T> unprocessedElements;

 public:
  CompletionAlgorithm(std::vector<T> initialElements)
      : visitedElements(initialElements.begin(), initialElements.end()),
        unprocessedElements(initialElements.begin(), initialElements.end()) {}

  CompletionAlgorithm(T const &initialElement) : visitedElements(), unprocessedElements() {
    add(initialElement);
  }

  CompletionAlgorithm() : visitedElements(), unprocessedElements() {}

  typedef std::function<void(T const &)> CallbackFunction;

  void add(T const &newElement) {
    if (visitedElements.find(newElement) == visitedElements.end()) {
      visitedElements.insert(newElement);
      unprocessedElements.push(newElement);
    }
  }

  template <typename Iter>
  void add(Iter beginIter, Iter endIter) {
    for (; beginIter != endIter; ++beginIter) {
      add(*beginIter);
    }
  }

  void computeSingleCompletion(CallbackFunction func) {
    while (!unprocessedElements.empty()) {
      T value = unprocessedElements.front();
      unprocessedElements.pop();

      func(value);
    }
  }

  std::vector<T> getResult() const {
    return std::vector<T>(visitedElements.begin(), visitedElements.end());
  }
};

} /* namespace math */
} /* namespace sfcpp */
