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



#ifndef PEANO_HPP_
#define PEANO_HPP_

#include <math/math.hpp>
#include <vector>
#include "PeanoOrientation.hpp"

namespace sfcpp {
namespace sfc {

struct NeighborshipInformation {
  bool directionFlip;
  std::array<table_index_type, 2> index;
};

template <index_type d>
struct NeighborshipTable {
  NeighborshipTable(index_type depth) : depth(depth), tableSize(), neighbors() {
    index_type cube_points = math::pow(3, d);
    tableSize = math::pow(cube_points, depth);
    for (index_type dim = 0; dim < d; ++dim) {
      neighbors[dim].resize(tableSize);
    }
  }

  index_type depth;
  index_type tableSize;
  std::array<std::vector<NeighborshipInformation>, d> neighbors;
};

/**
 * Class for converting Peano indices to multi-array-indices and finding
 * neighbors in the peano curve efficiently.
 * Time complexities for each method are given where d means the number of
 * dimensions of the peano curve and l the number of levels (i.e. the depth of
 * the tree).
 */
template <index_type d>
class PeanoAlgorithms {
  const index_type numLevels;
  const index_type numPoints;
  const index_type threeToNumLevelsMinusOne;
  NeighborshipTable<d> neighborshipTable;
  std::vector<PeanoOrientation<d>> orientationTable;
  std::vector<index_type> orientationBinaryTable;
  uint tableSize;
  std::vector<table_index_type> nTable;
  std::vector<bool> flipTable;

  /**
   * Precomputes Neighborship information.
   */
  void fillTable() {
    for (index_type dim = 0; dim < d; ++dim) {
      for (index_type i = 0; i < neighborshipTable.tableSize; ++i) {
        auto &entry = neighborshipTable.neighbors[dim][i];
        entry.index[false] =
            computeCellNeighbor(i, dim, false, neighborshipTable.depth);
        entry.index[true] =
            computeCellNeighbor(i, dim, true, neighborshipTable.depth);

        index_type reducedPIndex = i;

        bool directionFlip = false;

        for (index_type l = 0; l < neighborshipTable.depth; ++l) {
          for (index_type currentDim = 0; currentDim < d; ++currentDim) {
            if (currentDim != dim && (reducedPIndex % 3) == 1) {
              directionFlip = !directionFlip;
            }
            reducedPIndex /= 3;
          }
        }

        entry.directionFlip = directionFlip;
      }
    }

    tableSize = neighborshipTable.tableSize;
    size_t totalSize = tableSize * d * 2;
    nTable.resize(totalSize);
    flipTable.resize(totalSize);
    for (uint rem = 0; rem < tableSize; ++rem) {
      for (size_t dim = 0; dim < d; ++dim) {
        for (size_t direction = 0; direction <= 1; ++direction) {
          size_t idx = (rem * d + dim) * 2 + direction;
          nTable[idx] = neighborshipTable.neighbors[dim][rem].index[direction];
          flipTable[idx] = neighborshipTable.neighbors[dim][rem].directionFlip;
        }
      }
    }
  }

  /**
   * Precomputes orientation information
   */
  void fillOrientationTable(size_t orientationTableDepth) {
    size_t orientationTableSize = math::pow(CUBE_POINTS, orientationTableDepth);

    for (size_t i = 0; i < orientationTableSize; ++i) {
      auto o = computeOrientation(i);
      orientationTable.push_back(o);
      orientationBinaryTable.push_back(o.asBinaryNumber());
    }
  }

 public:
  static const index_type dimension = d;
  static index_type CUBE_POINTS;

  /**
   * Constructor. Complexity: O(d^2 * tableDepth * 3^(tableDepth*d) + l +
   * d*orientationTableDepth*3^(orientationTableDepth*d))
   */
  PeanoAlgorithms(index_type numLevels, index_type tableDepth = 2,
                  index_type orientationTableDepth = 2)
      : numLevels(numLevels),
        numPoints(math::pow(CUBE_POINTS, numLevels)),
        threeToNumLevelsMinusOne(math::pow(3, numLevels - 1)),
        neighborshipTable(tableDepth),
        orientationTable() {
    fillTable();
    fillOrientationTable(orientationTableDepth);
  }

  void setNumLevels(index_type newNumLevels) {
    numLevels = newNumLevels;
    numPoints = math::pow(CUBE_POINTS, numLevels);
    threeToNumLevelsMinusOne = math::pow(3, numLevels - 1);
  }

  index_type getNumLevels() const { return numLevels; }

  /**
   * Complexity: O(1)
   */
  index_type getNumPoints() const { return numPoints; }

  /**
   * Computes the orientation of the cell given by pIndex (the orientation
   * corresponds to a non-Terminal in a grammar for the peano curve)
   * Complexity: O(d*log(pIndex)), which is in O(ld) if pIndex is valid for this
   * level
   */
  PeanoOrientation<d> computeOrientation(index_type pIndex) const {
    PeanoOrientation<d> orientation;

    if (pIndex == INVALID_INDEX) return orientation;

    while (pIndex != 0) {
      for (index_type dim = 0; dim < d; ++dim) {
        auto result = std::div(pIndex, 3);
        pIndex = result.quot;

        // To go further in dimension dim, all orientations except the
        // orientation in dim have to be flipped
        if (result.rem == 1) {
          orientation.flipExcept(dim);
        }
      }
    }

    return orientation;
  }

  /**
   * Computes the orientation just like computeOrientation(), but more
   * efficiently by using a lookup table.
   * Complexity: O(log(pIndex)), which is in O(l) if pIndex is valid for this
   * level
   */
  PeanoOrientation<d> computeOrientationByLookup(index_type pIndex) const {
    PeanoOrientation<d> orientation;

    if (pIndex == INVALID_INDEX) {
      return orientation;
    }

    long orientationTableSize = orientationTable.size();

    while (pIndex != 0) {
      auto result = std::div(pIndex, orientationTableSize);
      orientation ^= orientationTable[result.rem];
      pIndex = result.quot;
    }

    return orientation;
  }

  /**
   * Computes the orientation just like computeOrientation(), but more
   * efficiently by using a lookup table.
   * Complexity: O(log(pIndex)), which is in O(l) if pIndex is valid for this
   * level
   */
  index_type computeOrientationBinaryByLookup(index_type pIndex) const {
    index_type orientation = 0;

    /*if (pIndex == INVALID_INDEX) {
      return orientation;
    }*/

    long orientationTableSize = orientationTable.size();

    while (pIndex != 0) {
      auto result = std::div(pIndex, orientationTableSize);
      orientation ^= orientationBinaryTable[result.rem];
      pIndex = result.quot;
    }

    return orientation;
  }

  /**
   * Computes the orientation of the cube (with 3^d elements) which contains the
   * cell, not the orientation of the cell itself.
   * Complexity: O(d*log(pIndex)), which is in O(ld) if pIndex is valid for this
   * level
   */
  PeanoOrientation<d> computeAboveOrientation(index_type pIndex) const {
    return computeOrientation(pIndex / CUBE_POINTS);
  }

  /**
   * @return Returns the neighbor index of pIndex in the given dimension nDim.
   * If backward is true, then the neighbor with the lower multi-index is
   * returned, otherwise the higher one.
   * @param orientation The orientation of the peano index pIndex as it can be
   * computed by computeOrientation().
   * Complexity: O(1) (average case), O(l) (worst case)
   */
  index_type computeGlobalNeighbor(PeanoOrientation<d> const &orientation,
                                   index_type pIndex, index_type nDim,
                                   bool backward) const {
    return computeCellNeighborByLookup(
        pIndex,
        2 * nDim + static_cast<index_type>(backward != orientation.at(nDim)));
  }

  /**
   * Attention: only used for computing lookup tables, the lookup algorithm is
   * faster!
   *
   * Computes a the index of a neighbor cell of the cell given by pIndex. The
   * neighbor is a neighbor in the given dimension nDim.
   * Depending on the orientation of the peano curve at pIndex and the boolean
   * parameter shouldGoBackward,
   * either the neighbor with a higher or a lower multi-index is returned.
   * This makes the computation more efficient.
   * In any case, varying shouldGoBackward will give the other neighbor.
   * If you want to know precisely which neighbor is which one, consider
   * computing the orientation of pIndex once and then using
   * computeGlobalNeighbor().
   * This can be used to precompute a table for computeCellNeighborByLookup().
   * This method considers the number of levels given by the parameter levels
   * instead of numLevels.
   * Complexity: O(d) (average case), O(levels * d) (worst case)
   */
  index_type computeCellNeighbor(index_type pIndex, index_type nDim,
                                 bool shouldGoBackward,
                                 index_type levels) const {
    // The goal is to find a peano index at which pIndex is "mirrored" to get
    // the neighbor index, i.e. neighborIndex = 2*mirrorIndex - 1 - pIndex.
    // For example, consider a simple 2-dimensional peano curve:
    // 6 7 8
    // 5 4 3
    // 0 1 2
    // If pIndex is either 0, 1 or 2 and you want to find the upper neighbor,
    // you can mirror pIndex at the "point" 2.5. Since we only use integers,
    // we can take mirrorIndex = 3 and see that
    // 2*3 - 1 - 0 == 5
    // 2*3 - 1 - 1 == 4
    // 2*3 - 1 - 2 == 3.
    // The mirror index is a multiple of the stepwidth (which would be 3 in this
    // case).
    index_type stepwidth = math::pow(3, nDim);
    index_type cubeSize = CUBE_POINTS;

    // direction is true iff we want to find the lower neighbor.
    // direction may change when a higher level is considered because the
    // neighbor is not contained in the lower-level cube.
    // For example, if we want to find the lower neighbor in the local
    // "coordinate system" of the lower level,
    // but we see that the local coordinate system of the lower level is flipped
    // in dimension nDim
    // from the viewpoint of the local coordinate system of the higher level, we
    // have to change the orientation.
    // The initial direction (e. g. shouldGoBackward) is viewed from the
    // orientation of a single cell,
    // i. e. going up in the above example would mean to go down if pIndex == 1,
    // because the orientation of 1 is "down right".
    bool direction = shouldGoBackward;

    // The reduced peano index is divided by CUBE_POINTS when we go up one
    // level.
    index_type reducedPeanoIndex = pIndex;

    for (index_type l = levels; l > 0; --l) {
      index_type dimensionStepwidth = CUBE_POINTS / 3;

      bool upperDimensionFlip = false;

      // compute contribution of higher dimensions to direction
      for (index_type dim = d - 1; dim > nDim; --dim) {
        index_type rem = (reducedPeanoIndex / dimensionStepwidth) % 3;

        if (rem == 1) {
          upperDimensionFlip = !upperDimensionFlip;
        }

        dimensionStepwidth /= 3;
      }

      direction = direction != upperDimensionFlip;

      dimensionStepwidth /= 3;  // reduction for the current dimension

      for (int dim = int(nDim) - 1; dim >= 0; --dim) {
        index_type rem = (reducedPeanoIndex / dimensionStepwidth) % 3;

        if (rem == 1) {
          direction = !direction;
        }

        dimensionStepwidth /= 3;
      }

      // ----- MAIN PART -----

      auto result = std::div(pIndex, long(stepwidth));
      index_type localIndex = result.quot % 3;

      // check if the neighbor cell is contained in the considered cube at this
      // level
      if (direction != upperDimensionFlip) {
        if (localIndex !=
            0) {  // 0 has no lower neighbor in the current considered cube
          // a neighbor at the current level exists
          index_type mirrorIndex = result.quot * stepwidth;
          return 2 * mirrorIndex - 1 - pIndex;
        }
      } else {
        if (localIndex !=
            2) {  // 2 has no higher neighbor in the current considered cube
          // a neighbor at the current level exists
          index_type mirrorIndex = (result.quot + 1) * stepwidth;
          return 2 * mirrorIndex - 1 - pIndex;
        }
      }

      stepwidth *= CUBE_POINTS;
      cubeSize *= CUBE_POINTS;
      reducedPeanoIndex /= CUBE_POINTS;  // go to next level
    }

    // If pIndex is at the border of the domain, there is no neighbor.
    return INVALID_INDEX;
  }

  /**
   * Computes a the index of a neighbor cell of the cell given by pIndex. The
   * neighbor is a neighbor in the dimension face / 2, face % 2 distinguishes
   * these two neighbors.
   * Depending on face % 2 the orientation of the peano curve at pIndex,
   * either the neighbor with a higher or a lower multi-index is returned.
   * This makes the computation more efficient.
   * In any case, varying face % 2 will give the other neighbor.
   * If you want to know precisely which neighbor is which one, consider
   * computing the orientation of pIndex once and then using
   * computeGlobalNeighbor().
   *
   * If no neighbor exists, the algorithm returns a value >= numPoints (not
   * necessarily INVALID_INDEX)
   * Complexity: O(1) (average case), O(l) (worst case)
   */
  index_type computeCellNeighborByLookup(index_type pIndex, uint face) const {
    uint rem = pIndex % tableSize;

    auto idx = (rem * d * 2) + face;
    index_type neighborIndex = nTable[idx];
    if (neighborIndex != TABLE_INVALID_INDEX) {
      // a neighbor is found
      // assemble the three parts to a neighbor index
      index_type resultingIndex = pIndex - rem + neighborIndex;
      return resultingIndex;
      // return resultingIndex < numPoints ? resultingIndex : INVALID_INDEX;
      // it might be that the table depth is not a divisor of numLevels
    }

    // index_type maxIterations = (numLevels - 1) / neighborshipTable.depth + 1;
    index_type restIndex = 0;
    index_type stepsize = 1;
    size_t quot = pIndex / tableSize;

    // for (index_type i = 0; i < maxIterations; ++i) {
    while (quot != 0 || rem != 0) {
      face ^= flipTable[idx];

      restIndex +=
          stepsize * (tableSize - 1 - rem);  // rest of index is mirrored

      rem = quot % tableSize;
      quot = quot / tableSize;
      stepsize *= tableSize;

      idx = rem * d * 2 + face;
      index_type neighborIndex = nTable[idx];
      if (neighborIndex != TABLE_INVALID_INDEX) {
        // a neighbor is found
        // assemble the three parts to a neighbor index
        index_type resultingIndex =
            (tableSize * quot + neighborIndex) * stepsize + restIndex;
        return resultingIndex;
        // return resultingIndex < numPoints ? resultingIndex : INVALID_INDEX;
        // it might be that the table depth is not a divisor of numLevels
      }
    }

    return INVALID_INDEX;
  }

  /**
   * Converts a peano index to a Multi-Index, that is, the corresponding
   * coordinates in all dimensions
   * Complexity: O(ld)
   */
  MultiIndex peanoToMultiIndex(index_type pIndex) const {
    MultiIndex multiIndex(d, 0);
    PeanoOrientation<d> orientation;

    if (pIndex == INVALID_INDEX) return multiIndex;

    long int divisor = numPoints / 3;

    for (index_type l = numLevels; l > 0; --l) {
      for (int dim = int(d) - 1; dim >= 0; --dim) {
        auto result = std::div(pIndex, divisor);

        multiIndex[dim] *= 3;
        multiIndex[dim] += orientation.at(dim) ? 2 - result.quot : result.quot;

        if (result.quot == 1) {
          orientation.flipExcept(dim);
        }
        pIndex = result.rem;
        divisor /= 3;
      }
    }

    return multiIndex;
  }

  /**
   * Converts a multi-index (i.e. coordinates of a grid cell) into the
   * corresponding peano index.
   * Complexity: O(ld)
   */
  index_type multiToPeanoIndex(MultiIndex multiIndex) const {
    long int divisor = threeToNumLevelsMinusOne;
    index_type pIndex = 0;
    PeanoOrientation<d> orientation;

    for (index_type dim = 0; dim < d; ++dim) {
      if (multiIndex[dim] >= 3 * threeToNumLevelsMinusOne) {
        return INVALID_INDEX;
      }
    }

    for (index_type l = numLevels; l > 0; --l) {
      for (int dim = int(d) - 1; dim >= 0; --dim) {
        pIndex *= 3;
        auto result = std::div(multiIndex[dim], divisor);
        multiIndex[dim] = result.rem;

        pIndex += orientation.at(dim) ? 2 - result.quot : result.quot;

        if (result.quot == 1) {
          orientation.flipExcept(dim);
        }
      }

      divisor /= 3;
    }

    return pIndex;
  }
};

template <index_type d>
index_type PeanoAlgorithms<d>::CUBE_POINTS = math::pow(3, d);
}
} /* namespace peano */

#endif /* PEANO_HPP_ */
