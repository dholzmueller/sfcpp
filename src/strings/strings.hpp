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



#ifndef UTILS_STRINGS_STRINGS_HPP_
#define UTILS_STRINGS_STRINGS_HPP_

#include <sstream>
#include <string>
#include <vector>

namespace sfcpp {
namespace strings {

std::string repeat(std::string const &str, size_t n);

std::string replace(std::string str, char toReplace, std::string replacement);

std::string trim(std::string const &str, std::string const &trimChars = " \t\r\n");

bool startsWith(std::string const &str, std::string const &prefix);

char charAtOrDefault(std::string const &str, size_t index, char defaultChar);

template <typename T>
std::string toString(T const &value) {
  std::ostringstream stream;
  stream << value;
  return stream.str();
}

template <typename T, typename... Args>
std::string toString(T const &container, std::string firstSeparator, Args... separators) {
  bool afterFirst = false;
  std::string result;
  for (auto &elem : container) {
    if (afterFirst) {
      result += firstSeparator;
    }

    result += toString(elem, separators...);

    afterFirst = true;
  }

  return result;
}

template <typename T>
std::string toStringFixedPrecision(T const &value, size_t precision) {
  std::ostringstream stream;
  stream.precision(precision);
  stream << std::fixed << value;
  return stream.str();
}

template <typename T>
T fromString(std::string const &str) {
  std::istringstream stream(str);
  T value;
  stream >> value;
  return value;
}

/**
 * Does not work for the lowest negative value if the type is signed...
 */
template <typename T>
std::string toStringFromBase(T value, size_t base = 10, size_t minNumDigits = 1) {
  if (base < 2 || base > 36) {
    throw std::runtime_error(
        "strings::toStringFromBase(): base must be between (including) 2 and 36");
  }

  const std::string digits = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::string prefix = "";
  std::string result = "";

  if (value < 0) {
    value = -value;
    prefix = "-";
  }

  for (size_t i = 0; i < minNumDigits || value != 0; ++i) {
    result = digits[value % base] + result;
    value /= base;
  }

  return result;
}

/**
 * Returns the digit or -1 if letter is not a digit
 */
int getDigit(char letter);
}
}

#endif /* UTILS_STRINGS_STRINGS_HPP_ */
