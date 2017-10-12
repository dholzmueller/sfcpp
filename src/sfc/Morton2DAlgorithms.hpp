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
 * Algorithms for the 2D Morton order.
 */
class Morton2DAlgorithms {
 public:
  /**
   * Neighbor-finding algorithm by Schrack (1992).
   */
  index_type neighbor(index_type index, uint nDim, bool shouldGoBackward) {
    static const size_t masks[] = {0x5555555555555555ul, 0xAAAAAAAAAAAAAAAAul,
                                   0x5555555555555555ul};

    size_t dimMask = masks[nDim];
    size_t otherDimMask = masks[nDim + 1];
    size_t secondPart = index & otherDimMask;

    if (shouldGoBackward) {
      size_t firstPart = index & dimMask;
      --firstPart;
      return (firstPart & dimMask) | secondPart;
    } else {
      size_t firstPart = index | otherDimMask;
      ++firstPart;
      return (firstPart & dimMask) | secondPart;
    }
  }
};

} /* namespace sfc */
} /* namespace sfcpp */
