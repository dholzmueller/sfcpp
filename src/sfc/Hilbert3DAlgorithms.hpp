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

#include <sfc/SFCTypeDefinitions.hpp>

#include <vector>

namespace sfcpp {
namespace sfc {

/**
 * Algorithms for the 3D Hilbert curve
 */
class Hilbert3DAlgorithms {
  static const size_t d = 3;
  static const size_t b = 1 << d;
  static const size_t numStates = 12;
  static const size_t numFacets = 6;
  size_t level;
  size_t tableSize;
  std::vector<table_index_type *> levelTables;

  static table_index_type pStateTable[numStates][b];
  static table_index_type cStateTable[numStates][b];
  static table_index_type nTable[b][numStates][numFacets];
  static table_index_type oTable[b][numStates][numStates][numFacets];
  static table_index_type pFacetTable[b][numStates]
                                     [numFacets];  // not necessary

 public:
  Hilbert3DAlgorithms(size_t level)
      : level(level), tableSize(b), levelTables(level) {}

  index_type neighbor(index_type index, index_type state, index_type facet) {
    uint rem = index % tableSize;
    index_type pState = pStateTable[state][rem];

    // auto idx = b * rem + facet;
    auto neighborIndex = nTable[rem][pState][facet];
    if (neighborIndex != TABLE_INVALID_INDEX) {
      index_type resultingIndex = index - rem + neighborIndex;
      return resultingIndex;
    }

    size_t quot = index / tableSize;  // TODO: replace by shift

    // for (index_type i = 0; i < maxIterations; ++i) {
    for (size_t i = 1; i < level; ++i) {
      state = pState;
      levelTables[i] = &oTable[rem][state][0][0];

      rem = quot % tableSize;
      quot = quot / tableSize;

      pState = pStateTable[state][rem];

      neighborIndex = nTable[rem][pState][facet];
      if (neighborIndex != TABLE_INVALID_INDEX) {
        state = cStateTable[pState][neighborIndex];
        quot = quot * tableSize + neighborIndex;
        for (; i > 0; --i) {
          auto childIndex = levelTables[i][numFacets * state + facet];
          quot = quot * tableSize + childIndex;
          state = cStateTable[state][childIndex];
        }
        return quot;
      }
    }

    return INVALID_INDEX;
  }
};

} /* namespace sfc */
} /* namespace sfcpp */
