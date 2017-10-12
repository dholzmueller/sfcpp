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

#include <math/math.hpp>
#include <sfc/SFCTypeDefinitions.hpp>

#include <vector>

namespace sfcpp {
namespace sfc {

/**
 * Algorithms for the 2D Hilbert curve.
 */
class Hilbert2DAlgorithms {
  static const size_t d = 2;
  static const size_t b = 1 << d;
  static const size_t numStates = 4;
  static const size_t numFacets = 4;
  static const index_type lowerMask = 0x5555555555555555ul;
  static const size_t numBits = 8 * sizeof(index_type);
  size_t level;
  std::vector<table_index_type *> levelTables;
  index_type flipMask;

  static table_index_type pStateTable[numStates][b];
  static table_index_type cStateTable[numStates][b];
  static table_index_type nTable[b][numStates][numFacets];
  static table_index_type oTable[b][numStates][numStates][numFacets];
  static table_index_type pFacetTable[b][numStates]
                                     [numFacets];  // not necessary

 public:
  Hilbert2DAlgorithms(size_t level)
      : level(level),
        levelTables(level),
        flipMask(lowerMask >> (numBits - 2 * level)) {}

  /**
   * Neighbor-finding algorithm with worst-case complexity O(level) and
   * average-case complexity O(1)
   */
  index_type neighbor(index_type position, index_type state, index_type facet) {
    static const index_type stateMaskTable[] = {1, 0, 0, 2};
    uint rem = position % b;
    index_type pState = state ^ stateMaskTable[rem];

    auto neighborIndex = nTable[rem][pState][facet];
    if (neighborIndex != TABLE_INVALID_INDEX) {
      index_type resultingIndex = position - rem + neighborIndex;
      return resultingIndex;
    }

    size_t quot = position / b;  // TODO: replace by shift

    for (size_t i = 1; i < level; ++i) {
      state = pState;
      levelTables[i] = &oTable[rem][state][0][0];

      rem = quot % b;
      quot = quot / b;

      pState = state ^ stateMaskTable[rem];

      neighborIndex = nTable[rem][pState][facet];
      if (neighborIndex != TABLE_INVALID_INDEX) {
        state = pState ^ stateMaskTable[neighborIndex];
        quot = quot * b + neighborIndex;
        for (; i > 0; --i) {
          auto childIndex = levelTables[i][numFacets * state + facet];
          quot = quot * b + childIndex;
          state = state ^ stateMaskTable[childIndex];
        }
        return quot;
      }
    }

    return INVALID_INDEX;
  }

  /**
   * Old version of the neighbor-finding algorithm using a state lookup table
   */
  index_type neighborOld(index_type position, index_type state, index_type facet) {
    // TODO: state transitions can be implemented more efficiently using XOR and
    // exploiting pStateTable = cStateTable...
    uint rem = position % b;
    index_type pState = pStateTable[state][rem];

    auto neighborIndex = nTable[rem][pState][facet];
    if (neighborIndex != TABLE_INVALID_INDEX) {
      index_type resultingIndex = position - rem + neighborIndex;
      return resultingIndex;
    }

    size_t quot = position / b;  // TODO: replace by shift

    for (size_t i = 1; i < level; ++i) {
      state = pState;
      levelTables[i] = &oTable[rem][state][0][0];

      rem = quot % b;
      quot = quot / b;

      pState = pStateTable[state][rem];

      neighborIndex = nTable[rem][pState][facet];
      if (neighborIndex != TABLE_INVALID_INDEX) {
        state = cStateTable[pState][neighborIndex];
        quot = quot * b + neighborIndex;
        for (; i > 0; --i) {
          auto childIndex = levelTables[i][numFacets * state + facet];
          quot = quot * b + childIndex;
          state = cStateTable[state][childIndex];
        }
        return quot;
      }
    }

    return INVALID_INDEX;
  }

  /**
   * O(1) state computation algorithm
   */
  index_type getState(index_type position) {
    index_type a = position & lowerMask;
    index_type b = (position >> 1) & lowerMask;
    index_type aband = a & b;
    index_type abnor = flipMask ^ (a | b);
    return 2 * (__builtin_popcount(aband) % 2) +
           (__builtin_popcount(abnor) % 2);
  }
};
}
}
