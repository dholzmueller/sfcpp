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



#include "SFCTypeDefinitions.hpp"

namespace sfcpp {
namespace sfc {

bool equals(MultiIndex const &first, MultiIndex const &second) {
  if (first.size() != second.size()) return false;

  for (size_t i = 0; i < first.size(); ++i) {
    if (first[i] != second[i]) return false;
  }

  return true;
}

std::ostream &operator<<(std::ostream &ostr, MultiIndex const &multiIndex) {
  ostr << "[" << multiIndex[0];
  for (size_t i = 1; i < multiIndex.size(); ++i) {
    ostr << "; " << multiIndex[i];
  }
  ostr << "]";

  return ostr;
}

} /* namespace sfc */
} /* namespace sfcpp */
