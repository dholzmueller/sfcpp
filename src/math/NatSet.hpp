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
#include <ostream>
#include <set>

namespace sfcpp {
namespace math {

class NatSet {
 public:
  typedef uint32_t NatType;

 private:
  void computeHash() {
    hash = 0;
    for (auto value : data) {
      hash ^= 1 << (value % NatTypeBits);
    }
  }

  std::set<NatType> data;
  size_t hash;

 public:
  static const uint32_t NatTypeBits = 8 * sizeof(NatType);

  NatSet() : data(), hash(0) {}

  template <typename Collection>
  explicit NatSet(Collection const &c) : data(c), hash(0) {
    computeHash();
  }

  size_t getHash() const { return hash; }

  friend bool operator==(NatSet const &first, NatSet const &second) {
    return first.data == second.data;
  }

  auto begin() const -> decltype(data.cbegin()) { return data.cbegin(); }
  auto end() const -> decltype(data.cend()) { return data.cend(); }

  auto cbegin() const -> decltype(data.cbegin()) { return data.cbegin(); }
  auto cend() const -> decltype(data.cend()) { return data.cend(); }

  void insert(NatType value) {
    auto it = data.find(value);
    if (it == data.end()) {
      hash ^= 1 << (value % NatTypeBits);
      data.insert(value);
    }
  }

  void remove(NatType value) {
    auto it = data.find(value);
    if (it != data.end()) {
      hash ^= 1 << (value % NatTypeBits);
      data.erase(it);
    }
  }

  bool contains(NatType value) { return data.find(value) != data.end(); }

  size_t size() const { return data.size(); }

  NatType anElement() const { return *data.begin(); }

  void clear() { data.clear(); }

  friend std::ostream &operator<<(std::ostream &stream, NatSet const &natSet) {
    std::vector<NatType> vec(natSet.data.begin(), natSet.data.end());
    std::sort(vec.begin(), vec.end());
    stream << "{" << strings::toString(vec, ", ") << "}";
    return stream;
  }

  NatSet reverseAt(NatType bound) {
    NatSet result;

    for (auto value : *this) {
      result.insert(bound - value);
    }

    return result;
  }

  int compare(NatSet const &other) {
    auto firstIt = begin();
    auto secondIt = other.begin();

    for (; firstIt != end() && secondIt != other.end(); ++firstIt, ++secondIt) {
      if (*firstIt < *secondIt) {
        return -1;
      }
      if (*firstIt > *secondIt) {
        return 1;
      }
    }

    if (firstIt != end()) {
      return 1;
    }

    if (secondIt != other.end()) {
      return -1;
    }

    return 0;
  }
};

struct NatSetHash {
  size_t operator()(NatSet const &set) const { return set.getHash(); }
};

} /* namespace math */
} /* namespace sfcpp */
