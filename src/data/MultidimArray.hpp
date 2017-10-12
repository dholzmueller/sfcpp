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

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

#include <strings/strings.hpp>

namespace sfcpp {
namespace data {

template <typename T, size_t d>
class MultidimArray;

template <typename T>
class MultidimArray<T, 0> {
  typedef T ArrayType;

  T value;
  std::array<size_t, 0> sizes;

 public:
  T &at() { return value; }

  bool contains() const { return true; }

  size_t getSize(size_t) { throw std::runtime_error("Called MultidimArray<T, 0>::getSize()"); }
};

template <typename T>
class MultidimArray<T, 1> {
  std::vector<T> data;
  std::array<size_t, 1> sizes;
  T defaultValue;

  void ensureIndex(size_t firstIndex) {
    if (data.size() <= firstIndex) {
      sizes[0] = firstIndex + 1;
      while (data.size() <= firstIndex) {
        data.push_back(defaultValue);
      }
    }
  }

 public:
  MultidimArray(T defaultValue = T()) : data(), sizes{0}, defaultValue(defaultValue) {}

  T &at(size_t firstIndex) {
    ensureIndex(firstIndex);
    return data[firstIndex];
  }

  T &operator[](size_t index) {
    ensureIndex(index);
    return data[index];
  }

  bool contains(size_t firstIndex) const { return firstIndex < data.size(); }

  bool containsNotDefault(size_t firstIndex) const {
    return firstIndex < data.size() && data[firstIndex] != defaultValue;
  }

  template <typename Collection>
  std::string getCppInitializer(Collection const &sizes, size_t startIndex) {
    std::string result = "{";
    bool first = true;
    for (auto &item : data) {
      if (first) {
        first = false;
      } else {
        result += ", ";
      }
      result += strings::toString(item);
    }
    for (size_t i = data.size(); i < sizes[startIndex]; ++i) {
      if (first) {
        first = false;
      } else {
        result += ",\n";
      }
      result += strings::toString(defaultValue);
    }
    result += "}";
    return result;
  }

  size_t getSize(size_t dim = 0) { return sizes[dim]; }

  auto begin() -> decltype(data.begin()) { return data.begin(); }

  auto end() -> decltype(data.end()) { return data.end(); }

  auto begin() const -> decltype(data.cbegin()) { return data.cbegin(); }

  auto end() const -> decltype(data.cend()) { return data.cend(); }
};

template <typename T, size_t d>
class MultidimArray {
 protected:
  std::vector<MultidimArray<T, d - 1>> data;
  std::array<size_t, d> sizes;
  T defaultValue;

  void ensureIndex(size_t firstIndex) {
    if (data.size() <= firstIndex) {
      sizes[0] = firstIndex + 1;
      while (data.size() <= firstIndex) {
        data.push_back(MultidimArray<T, d - 1>(defaultValue));
      }
    }
  }

 public:
  MultidimArray(T defaultValue = T()) : data(), sizes{0}, defaultValue(defaultValue) {}

  template <typename... SizeType>
  T &at(size_t firstIndex, SizeType... indexes) {
    ensureIndex(firstIndex);

    T &ref = data[firstIndex].at(indexes...);

    for (size_t dim = 0; dim < d - 1; ++dim) {
      sizes[dim + 1] = std::max(sizes[dim + 1], data[firstIndex].getSize(dim));
    }

    return ref;
  }

  MultidimArray<T, d - 1> &operator[](size_t index) {
    ensureIndex(index);
    return data[index];
  }

  template <typename... SizeType>
  bool contains(size_t firstIndex, SizeType... indexes) const {
    return firstIndex < data.size() && data[firstIndex].contains(indexes...);
  }

  template <typename... SizeType>
  bool containsNotDefault(size_t firstIndex, SizeType... indexes) const {
    return firstIndex < data.size() && data[firstIndex].containsNotDefault(indexes...);
  }

  template <typename Collection>
  std::string getCppInitializer(Collection const &sizes, size_t startIndex) {
    std::string result = "{";
    bool first = true;
    for (auto &item : data) {
      if (first) {
        first = false;
      } else {
        result += ",\n";
      }
      result += item.getCppInitializer(sizes, startIndex + 1);
    }
    for (size_t i = data.size(); i < sizes[startIndex]; ++i) {
      if (first) {
        first = false;
      } else {
        result += ",\n";
      }
      result += MultidimArray<T, d - 1>(defaultValue).getCppInitializer(sizes, startIndex + 1);
    }
    result += "}";
    return result;
  }

  std::string getCppArrayDeclaration(std::string arrayName, std::string typeName) {
    return typeName + " " + arrayName + "[" + strings::toString(sizes, "][") + "] = " +
           getCppInitializer(sizes, 0) + ";";
  }

  size_t getSize(size_t dim = 0) { return sizes[dim]; }

  auto begin() -> decltype(data.begin()) { return data.begin(); }

  auto end() -> decltype(data.end()) { return data.end(); }

  auto begin() const -> decltype(data.cbegin()) { return data.cbegin(); }

  auto end() const -> decltype(data.cend()) { return data.cend(); }
};

} /* namespace data */
} /* namespace sfcpp */
