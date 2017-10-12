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



#include "performance.hpp"

#include <latex/LatexDocument.hpp>
#include <latex/tikz/TikzAxis.hpp>
#include <latex/tikz/TikzCoordinatePlot.hpp>
#include <latex/tikz/TikzPicture.hpp>
#include <math/math.hpp>
#include <sfc/Hilbert2DAlgorithms.hpp>
#include <sfc/Hilbert3DAlgorithms.hpp>
#include <sfc/Morton2DAlgorithms.hpp>
#include <sfc/Sierpinski2DAlgorithms.hpp>

#include <algorithm>
#include <memory>

namespace sfcpp {
namespace test {

double hilbert2DNeighborPerformance(size_t level, size_t numSamples) {
  sfc::Hilbert2DAlgorithms alg(level);
  auto numPoints = math::pow(4, level);
  size_t numFacets = 4;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);
  std::uniform_int_distribution<uint32_t> faceDist(0, numFacets - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    auto face = faceDist(gen);
    sum += alg.neighbor(idx, 0, (sum + face) % numFacets);
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

double hilbert2DStatePerformance(size_t level, size_t numSamples) {
  sfc::Hilbert2DAlgorithms alg(level);
  auto numPoints = math::pow(4, level);
  size_t numFacets = 4;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    sum += alg.getState((idx + sum) % numFacets);
  }

  double time = stopwatch.elapsedSeconds();

  std::cout << "Time for computation: " << time / numSamples * 1000000000
            << "ns\n";

  gen.seed(gen.default_seed);

  stopwatch.start();

  for (size_t i = 0; i < numSamples; ++i) {
    sum += (idxDist(gen) + sum) % numFacets;
  }

  double defaultTime = stopwatch.elapsedSeconds();

  std::cout << "Time for noOp: " << defaultTime / numSamples * 1000000000
            << "ns\n";

  std::cout << "sum: " << sum << "\n";

  return (time - defaultTime) * 1000000000 / numSamples;
}

double hilbert3DNeighborPerformance(size_t level, size_t numSamples) {
  sfc::Hilbert3DAlgorithms alg(level);
  auto numPoints = math::pow(8, level);
  size_t numFacets = 6;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);
  std::uniform_int_distribution<uint32_t> faceDist(0, numFacets - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    auto face = faceDist(gen);
    sum += alg.neighbor(idx, 0, (sum + face) % numFacets);
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

double morton2DNeighborPerformance(size_t level, size_t numSamples) {
  sfc::Morton2DAlgorithms alg;
  auto numPoints = math::pow(4, level);
  size_t numFacets = 4;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);
  std::uniform_int_distribution<uint32_t> faceDist(0, numFacets - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  stopwatch.start();

  for (size_t i = 0; i < numSamples; ++i) {
    sum = idxDist(gen) + ((faceDist(gen) + sum) / 2) % 2;
  }

  double defaultTime = stopwatch.elapsedSeconds();

  std::cout << "Time for noOp: " << defaultTime / numSamples * 1000000000
            << "ns\n";

  gen.seed(gen.default_seed);

  stopwatch.start();

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    auto face = faceDist(gen);
    sum += alg.neighbor(idx, ((sum + face) / 2) % 2, (sum + face) % 2);
  }

  double time = stopwatch.elapsedSeconds();

  std::cout << "Time for computation: " << time / numSamples * 1000000000
            << "ns\n";

  std::cout << "sum: " << sum << "\n";

  return (time - defaultTime) * 1000000000 / numSamples;
}

double sierpinski2DNeighborPerformance(size_t level, size_t numSamples) {
  sfc::Sierpinski2DAlgorithms alg;
  auto numPoints = math::pow(2, level);
  size_t numFacets = 3;
  std::mt19937 gen;
  std::uniform_int_distribution<uint32_t> idxDist(0, numPoints - 1);
  std::uniform_int_distribution<uint32_t> faceDist(0, numFacets - 1);

  size_t sum = 0;

  time::Stopwatch stopwatch;

  for (size_t i = 0; i < numSamples; ++i) {
    auto idx = idxDist(gen);
    auto face = faceDist(gen);
    sum += alg.neighbor(idx, (sum + face) % numFacets);
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

Eigen::MatrixXd doTimeMeasurements(size_t lmin, size_t lmax,
                                   std::function<double(size_t)> func) {
  Eigen::MatrixXd results(lmax - lmin + 1, 2);

  for (size_t l = lmin; l <= lmax; ++l) {
    results(l - lmin, 0) = l;

    size_t k = 7;
    size_t n = 2 * k + 1;

    std::vector<double> singleResults(n);
    for (size_t i = 0; i < n; ++i) {
      singleResults[i] = func(l);
    }
    std::sort(singleResults.begin(), singleResults.end());
    // results(l - lmin, 1) = (singleResults[1] + singleResults[2] +
    // singleResults[3]) / 3.0;
    results(l - lmin, 1) = singleResults[k];
  }

  return results;
}

void createPeanoDimPerformancePlots(size_t numSamples) {
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  latex::tikz::TikzAxisConfiguration axisConfig;
  axisConfig.xlabel = "Level";
  axisConfig.ylabel = "time [ns]";
  axisConfig.ymin = "0";
  axisConfig.legendPos = "south east";
  axisConfig.width = "14cm";
  axisConfig.height = "10cm";
  axisConfig.additionalOptions = "cycle list name = custom black white";
  auto axis = std::make_shared<latex::tikz::TikzAxis>(axisConfig);
  picture->addElement(axis);

  size_t lmin = 1;
  size_t lmax = 12;

  // try not to ruin the first measurement
  std::cout << "Dummy precomputation: "
            << peanoNeighborPerformance<2>(2, 5 * numSamples, 1) << "\n";

  Eigen::MatrixXd peano2DResults2 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<2>(level, numSamples, 2);
      });
  Eigen::MatrixXd peano3DResults2 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<3>(level, numSamples, 2);
      });
  Eigen::MatrixXd peano4DResults2 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<4>(level, numSamples, 2);
      });

  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults2, "Peano2D(2)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano3DResults2, "Peano3D(2)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano4DResults2, "Peano4D(2)"));

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/plot-peano-dim-time.tex");
}

void createPeanoDepthPerformancePlots(size_t numSamples) {
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  latex::tikz::TikzAxisConfiguration axisConfig;
  axisConfig.xlabel = "Level";
  axisConfig.ylabel = "time [ns]";
  axisConfig.ymin = "0";
  axisConfig.legendPos = "south east";
  axisConfig.width = "14cm";
  axisConfig.height = "10cm";
  axisConfig.additionalOptions = "cycle list name = custom black white";
  auto axis = std::make_shared<latex::tikz::TikzAxis>(axisConfig);
  picture->addElement(axis);

  size_t lmin = 1;
  size_t lmax = 12;

  // try not to ruin the first measurement
  std::cout << "Dummy precomputation: "
            << peanoNeighborPerformance<2>(2, 5 * numSamples, 1) << "\n";

  Eigen::MatrixXd peano2DResults1 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<2>(level, numSamples, 1);
      });
  Eigen::MatrixXd peano2DResults2 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<2>(level, numSamples, 2);
      });
  Eigen::MatrixXd peano2DResults3 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<2>(level, numSamples, 3);
      });
  Eigen::MatrixXd peano2DResults4 =
      doTimeMeasurements(lmin, lmax, [&](size_t level) {
        return peanoNeighborPerformance<2>(level, numSamples, 4);
      });

  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults1, "Peano2D(1)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults2, "Peano2D(2)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults3, "Peano2D(3)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults4, "Peano2D(4)"));

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/plot-peano-depth-time.tex");
}

void create2DPerformancePlots(size_t numSamples) {
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  latex::tikz::TikzAxisConfiguration axisConfig;
  axisConfig.xlabel = "Level";
  axisConfig.ylabel = "time [ns]";
  axisConfig.ymin = "0";
  axisConfig.legendPos = "south east";
  axisConfig.width = "14cm";
  axisConfig.height = "10cm";
  axisConfig.additionalOptions = "cycle list name = custom black white";
  auto axis = std::make_shared<latex::tikz::TikzAxis>(axisConfig);
  picture->addElement(axis);

  size_t lmin = 1;
  size_t lmax = 12;

  // try not to ruin the first measurement
  std::cout << "Dummy precomputation: "
            << peanoNeighborPerformance<2>(2, 5 * numSamples, 1) << "\n";

  auto peano2DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return peanoNeighborPerformance<2>(level, numSamples, 1);
  });
  auto hilbert2DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return hilbert2DNeighborPerformance(level, numSamples);
  });
  auto hilbert3DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return hilbert3DNeighborPerformance(level, numSamples);
  });
  auto morton2DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return morton2DNeighborPerformance(level, numSamples);
  });
  auto sierpinski2DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return sierpinski2DNeighborPerformance(level, numSamples);
  });

  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults1, "Peano2D(1)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      hilbert2DResults1, "Hilbert2D(1)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      hilbert3DResults1, "Hilbert3D(1)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      morton2DResults1, "Morton2D"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      sierpinski2DResults1, "Sierpinski2D"));

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/plot-2D-time.tex");
}

void createStatePerformancePlots(size_t numSamples) {
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  latex::tikz::TikzAxisConfiguration axisConfig;
  axisConfig.xlabel = "Level";
  axisConfig.ylabel = "time [ns]";
  axisConfig.ymin = "0";
  axisConfig.legendPos = "north west";
  axisConfig.width = "14cm";
  axisConfig.height = "10cm";
  axisConfig.additionalOptions = "cycle list name = custom black white";
  auto axis = std::make_shared<latex::tikz::TikzAxis>(axisConfig);
  picture->addElement(axis);

  size_t lmin = 1;
  size_t lmax = 12;

  // try not to ruin the first measurement
  std::cout << "Dummy precomputation: "
            << peanoNeighborPerformance<2>(2, 5 * numSamples, 1) << "\n";

  auto peano2DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return peanoStatePerformance<2>(level, numSamples, 1);
  });
  auto peano2DResults2 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return peanoStatePerformance<2>(level, numSamples, 2);
  });
  auto peano2DResults3 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return peanoStatePerformance<2>(level, numSamples, 3);
  });
  auto hilbert2DResults1 = doTimeMeasurements(lmin, lmax, [&](size_t level) {
    return hilbert2DStatePerformance(level, numSamples);
  });

  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults1, "Peano2D(1)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults2, "Peano2D(2)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      peano2DResults3, "Peano2D(3)"));
  axis->addElement(std::make_shared<latex::tikz::TikzCoordinatePlot>(
      hilbert2DResults1, "Hilbert2D"));

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/plot-state-time.tex");
}
}  // namespace test
}  // namespace sfcpp
