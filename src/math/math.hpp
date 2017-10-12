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



#ifndef UTILS_MATH_MATH_HPP_
#define UTILS_MATH_MATH_HPP_

#include <cmath>
#include <cstddef>
#include <functional>
#include <vector>

namespace sfcpp {
namespace math {

static const double RAD_TO_DEG = 180.0 / M_PI;
static const double DEG_TO_RAD = 1.0 / RAD_TO_DEG;

template <typename T>
T pow(T base, size_t exponent) {
  T result = 1;
  while (exponent) {
    if (exponent & 1) result *= base;
    exponent >>= 1;
    base *= base;
  }

  return result;
}

template <class T>
T sgn(T t) {
  if (t < T(0)) return T(-1);
  return t > T(0) ? T(1) : T(0);
}

double mod(double val, double div);
float mod(float val, float div);

template <typename T>
T mod(T val, T div) {
  T result = val % div;
  if (result < 0) {
    result += div;
  }
  return result;
}

template <typename T>
T interpolateLinear(T first, T second, double weight) {
  return (1 - weight) * first + weight * second;
}

inline double radToDeg(double value) { return RAD_TO_DEG * value; }

inline double degToRad(double value) { return DEG_TO_RAD * value; }

double bound(double value, double lower, double upper);

double uniformPointWithBorder(size_t n, size_t i, double start = 0.0, double end = 1.0);
double uniformPointWithoutBorder(size_t n, size_t i, double start = 0.0, double end = 1.0);

template <typename T>
bool even(T value) {
  return (value % 2) == 0;
}

template <typename T>
bool odd(T value) {
  return (value % 2) == 1;
}

template <typename Func, typename In, typename Out = In, typename Comp = std::less<Out>>
std::pair<Out, size_t> minArgminIdx(Func const &func, std::vector<In> const &candidates) {
  if (candidates.size() == 0) {
    throw std::runtime_error("utils::math::minArgmin(): candidates.size() == 0");
  }

  size_t argminIdx = 0;
  Out minVal = func(candidates[0]);

  for (size_t i = 1; i < candidates.size(); ++i) {
    Out newVal = func(candidates[i]);
    if (Comp()(newVal, minVal)) {
      minVal = newVal;
      argminIdx = i;
    }
  }

  return std::make_pair(minVal, argminIdx);
}

template <typename Func, typename In, typename Out = In, typename Comp = std::less<Out>>
Out min(Func const &func, std::vector<In> const &candidates) {
  return minArgminIdx<Func, In, Out, Comp>(func, candidates).first;
}

template <typename Func, typename In, typename Out = In, typename Comp = std::less<Out>>
size_t argminIdx(Func const &func, std::vector<In> const &candidates) {
  return minArgminIdx<Func, In, Out, Comp>(func, candidates).second;
}

template <typename Func, typename In, typename Out = In, typename Comp = std::less<Out>>
In argmin(Func const &func, std::vector<In> const &candidates) {
  return candidates[argminIdx<Func, In, Out, Comp>(func, candidates)];
}
}
}

#endif /* UTILS_MATH_MATH_HPP_ */
