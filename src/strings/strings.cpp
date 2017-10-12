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



#include "strings.hpp"

#include <stdexcept>

namespace sfcpp {
namespace strings {

std::string repeat(const std::string &str, size_t n) {
  size_t partLength = str.length();
  size_t numChars = n * partLength;
  std::string result(numChars, ' ');

  for (size_t i = 0; i < numChars; ++i) {
    result[i] = str[i % partLength];
  }

  return result;
}

std::string trim(std::string const &str, std::string const &trimChars) {
  if (str.length() == 0) {
    return str;
  }

  size_t start = 0;
  for (; start < str.length(); ++start) {
    if (trimChars.find(str[start]) == std::string::npos) {
      break;
    }
  }

  if (start == str.length()) {
    return "";
  }

  size_t end = str.length();
  for (; end > start; --end) {
    if (trimChars.find(str[end - 1]) == std::string::npos) {
      break;
    }
  }

  return str.substr(start, end - start);
}

std::string replace(std::string str, char toReplace, std::string replacement) {
  std::string result = "";
  for (size_t i = 0; i < str.length(); ++i) {
    if (str[i] == toReplace) {
      result += replacement;
    } else {
      result += str[i];
    }
  }
  return result;
}

char charAtOrDefault(const std::string &str, size_t index, char defaultChar) {
  if (index < str.length()) {
    return str[index];
  }

  return defaultChar;
}

bool startsWith(std::string const &str, std::string const &prefix) { return str.find(prefix) == 0; }

int getDigit(char letter) {
  int digit = int(letter) - int('0');
  if (digit < 0 || digit > 9) {
    return -1;
  }

  return digit;
}
}
}
