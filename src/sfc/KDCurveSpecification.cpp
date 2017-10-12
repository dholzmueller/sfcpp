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



#include "KDCurveSpecification.hpp"

#include <math/math.hpp>
#include <strings/strings.hpp>

#include <deque>
#include <iostream>
#include <stdexcept>

namespace sfcpp {
namespace sfc {

std::shared_ptr<CurveSpecification> KDCurveSpecification::getCurveSpecification() const {
  auto spec = std::make_shared<CurveSpecification>();
  auto numChildren = math::pow(k, d);
  size_t numEdges = math::pow(2, d);

  double kInv = 1.0 / k;

  spec->d = d;
  spec->rootPoints = KDCurveSpecification::generateUnitCube(d);
  spec->grammar = grammar;
  spec->transitionMats.resize(grammar.size());

  for (size_t grammarElement = 0; grammarElement < grammar.size(); ++grammarElement) {
    auto &vec = spec->transitionMats[grammarElement];
    auto &ordering = childOrdering[grammarElement];

    vec.resize(numChildren);

    for (size_t i = 0; i < numChildren; ++i) {
      Eigen::MatrixXd matrix = Eigen::MatrixXd::Ones(numEdges, numEdges);

      size_t reducedIndex = ordering[i];

      for (size_t dim = 0; dim < d; ++dim) {
        size_t remainder = reducedIndex % k;
        reducedIndex /= k;

        for (size_t edge = 0; edge < numEdges; edge++) {
          size_t offset = remainder + ((edge >> dim) & 1);
          // coordinate of the edge in dimension dim
          double coordinate = offset * kInv;

          for (size_t sourceEdge = 0; sourceEdge < numEdges; ++sourceEdge) {
            matrix(sourceEdge, edge) *= ((sourceEdge >> dim) & 1) ? coordinate : (1 - coordinate);
          }
        }
      }

      vec[i] = matrix;
    }
  }

  return spec;
}

std::shared_ptr<CurveSpecification> KDCurveSpecification::getLocalCurveSpecification() const {
  auto spec = std::make_shared<CurveSpecification>();
  auto numChildren = math::pow(k, d);
  size_t numEdges = math::pow(2, d);

  double kInv = 1.0 / k;

  spec->d = d;
  spec->rootPoints = KDCurveSpecification::generateUnitCube(d);
  spec->grammar.push_back(std::vector<size_t>(numChildren, 0));
  spec->transitionMats.resize(grammar.size());

  size_t grammarElement = 0;
  auto &vec = spec->transitionMats[grammarElement];
  auto &ordering = childOrdering[grammarElement];

  vec.resize(numChildren);

  for (size_t i = 0; i < numChildren; ++i) {
    Eigen::MatrixXd matrix = Eigen::MatrixXd::Ones(numEdges, numEdges);
    size_t childState = grammar[0][i];

    size_t reducedIndex = ordering[i];

    for (size_t dim = 0; dim < d; ++dim) {
      size_t remainder = reducedIndex % k;
      reducedIndex /= k;

      for (size_t edge = 0; edge < numEdges; edge++) {
        // compute corresponding edge
        size_t edgeIndex = 0;
        for (size_t dimCounter = 0; dimCounter < d; ++dimCounter) {
          edgeIndex += (k - 1) * math::pow(k, dimCounter) * ((edge >> dimCounter) & 1);
        }
        auto it = std::find(childOrdering[0].begin(), childOrdering[0].end(), edgeIndex);
        size_t foundIndex = it - childOrdering[0].begin();
        size_t childIndex = childOrdering[childState][foundIndex];
        size_t resultingEdge = 0;
        for (size_t dimCounter = 0; dimCounter < d; ++dimCounter) {
          size_t remainder = (childIndex / math::pow(k, dimCounter)) % k;
          if (remainder == k - 1) {
            resultingEdge |= 1 << dimCounter;
          } else if (remainder != 0) {
            throw std::runtime_error(
                "KDCurveSpecification::getLocalCurveSpecification(): invalid remainder, cannot "
                "localize curve");
          }
        }

        size_t offset = remainder + ((resultingEdge >> dim) & 1);
        // coordinate of the edge in dimension dim
        double coordinate = offset * kInv;

        for (size_t sourceEdge = 0; sourceEdge < numEdges; ++sourceEdge) {
          matrix(sourceEdge, edge) *= ((sourceEdge >> dim) & 1) ? coordinate : (1 - coordinate);
        }
      }
    }

    vec[i] = matrix;
  }

  return spec;
}

/*
KDCurveSpecification::KDCurveSpecification(size_t k, size_t d,
                                           std::initializer_list<std::string> stringGrammar)
    : grammar(), childOrdering(), k(k), d(k) {
  size_t numChildren = math::pow(k, d);
  std::vector<std::string> strings(stringGrammar);

  grammar.resize(strings.size());

  for (auto &str : strings) {
    if (str.length() != numChildren) {
      throw std::runtime_error(
          "KDCurveSpecification::KDCurveSpecification(): string lengths are not k^d");
    }
  }
}
*/

Eigen::MatrixXd KDCurveSpecification::generateUnitCube(size_t dim) {
  Eigen::MatrixXd result(dim, 1ul << dim);

  for (size_t pointIdx = 0; pointIdx < (1ul << dim); ++pointIdx) {
    for (size_t d = 0; d < dim; ++d) {
      result(d, pointIdx) = static_cast<double>((pointIdx >> d) & 1);
    }
  }

  return result;
}

KDCurveSpecification::KDCurveSpecification(size_t k, size_t d, size_t numGrammarElements)
    : k(k),
      d(d),
      grammar(numGrammarElements, std::vector<size_t>(math::pow(k, d))),
      childOrdering(numGrammarElements, std::vector<size_t>(math::pow(k, d))) {}

/**
 * An instance of Orientation specifies for a given point and each dimension whether the orientation
 * of the peano curve is flipped in this dimension.
 */
class Orientation {
  std::deque<bool> data;

 public:
  /**
   * Constructs an Orientation<d>-object which is false in all dimensions.
   */
  Orientation(size_t d) : data(d, false) {}

  Orientation(size_t d, size_t binary) : data(d, false) {
    for (size_t dim = 0; dim < d; ++dim) {
      if ((binary >> dim) & 1) {
        data[dim] = true;
      }
    }
  }

  /**
   * Produces a Orientation object with d dimensions which is true at dim and false in all other
   * dimensions.
   */
  static Orientation characteristic(size_t d, size_t dim) {
    Orientation result(d);
    result.at(dim) = true;
    return result;
  }

  /**
   * @return Returns true iff the peano curve is flipped in the given dimension.
   * The result is a reference which can be assigned to.
   */
  bool &at(size_t dim) { return data[dim]; }

  /**
   * @return Returns true iff the peano curve is flipped in the given dimension.
   */
  bool at(size_t dim) const { return data[dim]; }

  /**
   * Inverts the orientation in all dimensions except the given dimension dim.
   * This can be used to compute the orientation at a peano index.
   * A useful property of the peano curve is that if you know the orientation o of a peano index
   * pIndex,
   * the orientation of one of the neighbour indices in direction dim can be obtained by calling
   * flipExcept(dim)
   */
  void flipExcept(size_t dim) {
    for (size_t otherdim = 0; otherdim < data.size(); ++otherdim) {
      if (otherdim != dim) {
        data[otherdim] = !data[otherdim];
      }
    }
  }

  size_t getBinary() const {
    size_t binary = 0;
    for (size_t i = 0; i < data.size(); ++i) {
      if (data[i]) {
        binary |= 1 << i;
      }
    }
    return binary;
  }

  /**
   * Returns an Orientation<d>-object where the orientation is inverted in all dimensions.
   */
  Orientation inv() const {
    Orientation result(data.size());
    for (size_t dim = 0; dim < data.size(); ++dim) {
      result.at(dim) = !data[dim];
    }
    return result;
  }

  Orientation &operator^=(Orientation const &other) {
    for (size_t dim = 0; dim < data.size(); ++dim) {
      data[dim] = data[dim] != other.data[dim];
    }

    return *this;
  }
};

std::shared_ptr<KDCurveSpecification> KDCurveSpecification::getPeanoCurveSpecification(size_t d,
                                                                                       size_t k) {
  if (k % 2 != 1 || k < 3) {
    throw std::runtime_error(
        "KDCurveSpecification::getPeanoCurveSpecification(): k % 2 != 1 || k < 3");
  }

  size_t numGrammarElements = std::pow(2, d);
  size_t numChildren = std::pow(k, d);
  auto spec = std::make_shared<KDCurveSpecification>(k, d, numGrammarElements);

  for (size_t grammarElement = 0; grammarElement < numGrammarElements; ++grammarElement) {
    for (size_t child = 0; child < numChildren; ++child) {
      Orientation orientation(d, grammarElement);

      // compute the childOrdering
      size_t orderedChild = 0;
      size_t power = math::pow(k, d - 1);

      // specifies whether higher dimensions require the lower dimension to flip order
      bool higherDimensionsFlip = false;
      for (size_t invDim = 0; invDim < d; ++invDim) {
        size_t dim = d - 1 - invDim;
        size_t kAryDigit = (child / power) % k;

        if (orientation.at(dim) != higherDimensionsFlip) {
          kAryDigit = k - 1 - kAryDigit;  // flip digit for reverse order
        }

        orderedChild += power * kAryDigit;

        if (kAryDigit % 2 == 1) {
          higherDimensionsFlip = !higherDimensionsFlip;
        }

        power /= k;
      }

      // compute the grammar
      size_t reducedIndex = child;
      for (size_t dim = 0; dim < d; ++dim) {
        auto remainder = reducedIndex % k;
        reducedIndex /= k;

        if (remainder % 2 == 1) {
          orientation.flipExcept(dim);
        }
      }
      spec->grammar[grammarElement][child] = orientation.getBinary();
      spec->childOrdering[grammarElement][child] = orderedChild;
    }
  }

  return spec;
}

struct HilbertDirection {
  size_t vertex;
  size_t dim;

  HilbertDirection(size_t vertex, size_t dim) : vertex(vertex), dim(dim) {}
};

std::shared_ptr<KDCurveSpecification> KDCurveSpecification::getHilbertCurveSpecification(size_t d) {
  size_t numGrammarElements = 0;  // don't know in advance
  size_t numChildren = std::pow(2, d);
  size_t k = 2;
  auto spec = std::make_shared<KDCurveSpecification>(k, d, numGrammarElements);

  const size_t UNDEFINED = -1;

  // grammarNumber(edge, dim)
  std::vector<std::vector<size_t>> directionToGrammar(numChildren,
                                                      std::vector<size_t>(d, UNDEFINED));
  std::vector<HilbertDirection> grammarToDirection(1, HilbertDirection(0, 0));
  directionToGrammar[0][0] = 0;

  size_t currentGrammarElement = 0;

  while (currentGrammarElement < grammarToDirection.size()) {
    // compute grammar rules and childOrdering for next grammar element
    auto currentDirection = grammarToDirection[currentGrammarElement];
    std::vector<size_t> nextRule(numChildren, 0);
    std::vector<size_t> nextOrdering(1, currentDirection.vertex);

    size_t nextDim = (currentDirection.dim + 1) % d;
    nextOrdering.push_back(currentDirection.vertex ^ (1 << nextDim));

    HilbertDirection firstDirection(currentDirection.vertex, nextDim);
    std::vector<HilbertDirection> nextDirections(2, firstDirection);

    while (nextDim != currentDirection.dim) {
      nextDim = (nextDim + 1) % d;

      // mirror
      for (int index = nextDirections.size() - 1; index >= 0; --index) {
        // mirror nextDirections
        HilbertDirection dir = nextDirections[index];
        size_t bitmask = (1 << dir.dim) | (1 << nextDim);
        dir.vertex ^= bitmask;
        nextDirections.push_back(dir);

        // mirror nextOrdering
        size_t orderingIndex = nextOrdering[index];
        orderingIndex ^= (1 << nextDim);
        nextOrdering.push_back(orderingIndex);
      }

      // connect middle
      size_t middle = nextDirections.size() / 2;
      auto &dir = nextDirections[middle];
      dir.vertex ^= (1 << dir.dim) | (1 << nextDim);
      dir.dim = nextDim;
      nextDirections[middle - 1].dim = nextDim;
    }

    // compute nextRule from nextDirections
    for (size_t i = 0; i < numChildren; ++i) {
      auto &dir = nextDirections[i];
      size_t grammarIndex = directionToGrammar[dir.vertex][dir.dim];
      if (grammarIndex == UNDEFINED) {
        grammarIndex = grammarToDirection.size();
        directionToGrammar[dir.vertex][dir.dim] = grammarIndex;
        grammarToDirection.push_back(dir);
      }
      nextRule[i] = grammarIndex;
    }

    spec->grammar.push_back(nextRule);
    spec->childOrdering.push_back(nextOrdering);

    ++currentGrammarElement;
  }

  return spec;
}

std::shared_ptr<KDCurveSpecification> KDCurveSpecification::getMortonCurveSpecification(size_t d,
                                                                                        size_t k) {
  size_t numGrammarElements = 1;
  size_t numChildren = std::pow(k, d);
  auto spec = std::make_shared<KDCurveSpecification>(k, d, numGrammarElements);

  spec->grammar[0] = std::vector<size_t>(numChildren, 0);

  for (size_t i = 0; i < numChildren; ++i) {
    spec->childOrdering[0][i] = i;
  }

  return spec;
}

} /* namespace sfc */
} /* namespace sfcpp */
