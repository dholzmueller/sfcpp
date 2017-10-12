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

#include <sfc/PeanoAlgorithms.hpp>
#include <time/Stopwatch.hpp>

#include <Eigen/Dense>

#include <functional>
#include <random>

namespace sfcpp {
namespace test {
double hilbert2DNeighborPerformance(size_t level, size_t numSamples);
double hilbert3DNeighborPerformance(size_t level, size_t numSamples);
double morton2DNeighborPerformance(size_t level, size_t numSamples);
double sierpinski2DNeighborPerformance(size_t level, size_t numSamples);

Eigen::MatrixXd doTimeMeasurements(size_t lmin, size_t lmax,
                                   std::function<double(size_t)> func);

void createPeanoDimPerformancePlots(size_t numSamples);
void createPeanoDepthPerformancePlots(size_t numSamples);
void create2DPerformancePlots(size_t numSamples);
void createStatePerformancePlots(size_t numSamples);

template <size_t d>
double peanoNeighborPerformance(size_t level, size_t numSamples,
                                size_t tableDepth) {
  sfc::PeanoAlgorithms<d> peano(level, tableDepth, tableDepth);
  auto numPoints = peano.getNumPoints();
  size_t numFacets = 2 * d;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);
  std::uniform_int_distribution<uint32_t> faceDist(0, numFacets - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    auto face = faceDist(gen);
    sum += peano.computeCellNeighborByLookup(idx, (sum + face) % numFacets);
  }

  double time = stopwatch.elapsedSeconds();

  std::cout << "Time for computation: " << time / numSamples * 1000000000
            << "ns\n";

  gen.seed(gen.default_seed);

  stopwatch.start();

  for (size_t i = 0; i < numSamples; ++i) {
    sum += (idxDist(gen) + faceDist(gen) + sum) % numFacets;
  }

  double defaultTime = stopwatch.elapsedSeconds();

  std::cout << "Time for noOp: " << defaultTime / numSamples * 1000000000
            << "ns\n";

  std::cout << "sum: " << sum << "\n";

  return (time - defaultTime) * 1000000000 / numSamples;
}

template <size_t d>
double peanoStatePerformance(size_t level, size_t numSamples,
                             size_t tableDepth) {
  sfc::PeanoAlgorithms<d> peano(level, tableDepth, tableDepth);
  auto numPoints = peano.getNumPoints();
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    sum += peano.computeOrientationBinaryByLookup((sum + idx) % numPoints);
  }

  double time = stopwatch.elapsedSeconds();

  std::cout << "Time for computation: " << time / numSamples * 1000000000
            << "ns\n";

  gen.seed(gen.default_seed);

  stopwatch.start();

  for (size_t i = 0; i < numSamples; ++i) {
    sum += (idxDist(gen) + sum) % numPoints;
  }

  double defaultTime = stopwatch.elapsedSeconds();

  std::cout << "Time for noOp: " << defaultTime / numSamples * 1000000000
            << "ns\n";

  std::cout << "sum: " << sum << "\n";

  return (time - defaultTime) * 1000000000 / numSamples;
}
}  // namespace test
}  // namespace sfcpp
