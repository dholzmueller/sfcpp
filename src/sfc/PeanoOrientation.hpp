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



#ifndef ORIENTATION_HPP_
#define ORIENTATION_HPP_

#include <sfc/SFCTypeDefinitions.hpp>

#include <array>

namespace sfcpp {
namespace sfc {

/**
 * An instance of Orientation specifies for a given point and each dimension whether the orientation
 * of the peano curve is flipped in this dimension.
 */
template <index_type d>
class PeanoOrientation {
  std::array<bool, d> data;

 public:
  /**
   * Constructs an Orientation<d>-object which is false in all dimensions.
   */
  PeanoOrientation() {
    for (index_type dim = 0; dim < d; ++dim) {
      data[dim] = false;
    }
  }

  /**
   * Produces a Orientation<d>-object which is true at dim and false in all other dimensions.
   */
  static PeanoOrientation<d> characteristic(index_type dim) {
    PeanoOrientation<d> result;
    result.at(dim) = true;
    return result;
  }

  /**
   * @return Returns true iff the peano curve is flipped in the given dimension.
   * The result is a reference which can be assigned to.
   */
  bool &at(index_type dim) { return data[dim]; }

  /**
   * @return Returns true iff the peano curve is flipped in the given dimension.
   */
  bool at(index_type dim) const { return data[dim]; }

  /**
   * Inverts the orientation in all dimensions except the given dimension dim.
   * This can be used to compute the orientation at a peano index.
   * A useful property of the peano curve is that if you know the orientation o of a peano index
   * pIndex,
   * the orientation of one of the neighbour indices in direction dim can be obtained by calling
   * flipExcept(dim)
   */
  void flipExcept(index_type dim) {
    for (index_type otherdim = 0; otherdim < d; ++otherdim) {
      if (otherdim != dim) {
        data[otherdim] = !data[otherdim];
      }
    }
  }

  /**
   * Returns an Orientation<d>-object where the orientation is inverted in all dimensions.
   */
  PeanoOrientation<d> inv() const {
    PeanoOrientation<d> result;
    for (index_type dim = 0; dim < d; ++dim) {
      result.at(dim) = !data[dim];
    }
    return result;
  }

  PeanoOrientation<d> &operator^=(PeanoOrientation<d> const &other) {
    for (size_t dim = 0; dim < d; ++dim) {
      data[dim] = data[dim] != other.data[dim];
    }

    return *this;
  }

  index_type asBinaryNumber() const {
    index_type number = 0;
    for (size_t dim = 0; dim < d; ++dim) {
      if (data[dim]) {
        number |= (1 << dim);
      }
    }
    return number;
  }
};

/**
 * Compares two orientations.
 */
template <index_type d>
bool operator==(PeanoOrientation<d> first, PeanoOrientation<d> second) {
  for (index_type dim = 0; dim < d; ++dim) {
    if (first.at(dim) != second.at(dim)) return false;
  }

  return true;
}

/**
 * Returns the componentwise xor of two orientations.
 */
template <index_type d>
PeanoOrientation<d> operator^(PeanoOrientation<d> first, PeanoOrientation<d> second) {
  PeanoOrientation<d> result;

  for (index_type dim = 0; dim < d; ++dim) {
    result.at(dim) = (first.at(dim) != second.at(dim));
  }

  return result;
}
}
} /* namespace sfcpp */

#endif /* ORIENTATION_HPP_ */
