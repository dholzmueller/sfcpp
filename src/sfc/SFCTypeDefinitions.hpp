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

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace sfcpp {
namespace sfc {

typedef size_t index_type;
typedef unsigned int uint;
static const index_type INVALID_INDEX = -1;
typedef uint32_t table_index_type;
static const table_index_type TABLE_INVALID_INDEX = -1;

typedef std::vector<index_type> MultiIndex;
bool equals(MultiIndex const &first, MultiIndex const &second);
std::ostream &operator<<(std::ostream &ostr, MultiIndex const &multiIndex);

} /* namespace sfc */
} /* namespace sfcpp */
