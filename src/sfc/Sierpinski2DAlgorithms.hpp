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

namespace sfcpp {
namespace sfc {

/**
 * Algorithms for the 2D Sierpinski curve.
 */
class Sierpinski2DAlgorithms {
 public:
  /**
   * Neighbor-finding algorithm for a local model of the 2D Sierpinski curve.
   */
  index_type neighbor(index_type position, uint facet) {
    index_type currentBitMask = 1;
    index_type cumulativeBitMask = 1;
    index_type reducedPosition = position;

    while (true) {
      uint index = reducedPosition % 2;

      if (index + facet == 1) {
        return position ^ cumulativeBitMask;
      }

      if (reducedPosition == 0) {
        return TABLE_INVALID_INDEX;
      }

      facet = 2 - facet + index;

      reducedPosition /= 2;
      currentBitMask *= 2;
      cumulativeBitMask |= currentBitMask;
    }
  }
};

} /* namespace sfc */
} /* namespace sfcpp */
