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



#include <sfc/Hilbert2DAlgorithms.hpp>

namespace sfcpp {
namespace sfc {

static const table_index_type T = TABLE_INVALID_INDEX;

table_index_type Hilbert2DAlgorithms::pStateTable[4][4] = {
    {1, 0, 0, 2}, {0, 1, 1, 3}, {3, 2, 2, 0}, {2, 3, 3, 1}};

table_index_type Hilbert2DAlgorithms::cStateTable[4][4] = {
    {1, 0, 0, 2}, {0, 1, 1, 3}, {3, 2, 2, 0}, {2, 3, 3, 1}};

table_index_type Hilbert2DAlgorithms::nTable[4][4][4] = {
    {{T, 1, T, 3}, {T, 3, T, 1}, {3, T, 1, T}, {1, T, 3, T}},
    {{0, T, T, 2}, {T, 2, 0, T}, {2, T, T, 0}, {T, 0, 2, T}},
    {{3, T, 1, T}, {1, T, 3, T}, {T, 1, T, 3}, {T, 3, T, 1}},
    {{T, 2, 0, T}, {0, T, T, 2}, {T, 0, 2, T}, {2, T, T, 0}}};

table_index_type Hilbert2DAlgorithms::oTable[4][4][4][4] = {
    {{{T, T, 3, T}, {3, T, T, T}, {1, T, 3, T}, {3, T, 1, T}},
     {{T, T, 3, T}, {3, T, T, T}, {1, T, 3, T}, {3, T, 1, T}},
     {{T, 3, T, 1}, {T, 1, T, 3}, {T, 3, T, T}, {T, T, T, 3}},
     {{T, 3, T, 1}, {T, 1, T, 3}, {T, 3, T, T}, {T, T, T, 3}}},
    {{{T, T, 2, T}, {T, T, T, T}, {T, T, 0, T}, {T, 2, 0, T}},
     {{T, T, T, T}, {2, T, T, T}, {0, T, T, 2}, {0, T, T, T}},
     {{T, 0, T, T}, {T, 0, 2, T}, {T, 2, T, T}, {T, T, T, T}},
     {{2, T, T, 0}, {T, T, T, 0}, {T, T, T, T}, {T, T, T, 2}}},
    {{{T, T, T, 1}, {T, T, T, 3}, {T, T, T, T}, {T, 1, T, 3}},
     {{T, 3, T, T}, {T, 1, T, T}, {T, 3, T, 1}, {T, T, T, T}},
     {{T, T, T, T}, {3, T, 1, T}, {1, T, T, T}, {3, T, T, T}},
     {{1, T, 3, T}, {T, T, T, T}, {T, T, 3, T}, {T, T, 1, T}}},
    {{{T, T, T, 0}, {2, T, T, 0}, {0, T, T, T}, {0, T, T, 2}},
     {{T, 0, 2, T}, {T, 0, T, T}, {T, 2, 0, T}, {T, T, 0, T}},
     {{T, T, T, 0}, {2, T, T, 0}, {0, T, T, T}, {0, T, T, 2}},
     {{T, 0, 2, T}, {T, 0, T, T}, {T, 2, 0, T}, {T, T, 0, T}}}};

table_index_type Hilbert2DAlgorithms::pFacetTable[4][4][4] = {
    {{0, T, 2, T}, {0, T, 2, T}, {T, 1, T, 3}, {T, 1, T, 3}},
    {{T, 1, 2, T}, {0, T, T, 3}, {T, 1, 2, T}, {0, T, T, 3}},
    {{T, 1, T, 3}, {T, 1, T, 3}, {0, T, 2, T}, {0, T, 2, T}},
    {{0, T, T, 3}, {T, 1, 2, T}, {0, T, T, 3}, {T, 1, 2, T}}};
}
}
