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

#include <geo/ConvexPolytope.hpp>
#include <geo/geo.hpp>
#include <img/Color.hpp>
#include <latex/LatexDocument.hpp>
#include <latex/tikz/TikzElementConfiguration.hpp>
#include <latex/tikz/TikzLine.hpp>
#include <latex/tikz/TikzPicture.hpp>
#include <math/AffineSubspace.hpp>
#include <math/NatSet.hpp>
#include <math/PermutationSubgroup.hpp>
#include <sfc/CurveInformation.hpp>
#include <sfc/CurveRenderer.hpp>
#include <sfc/Hilbert2DAlgorithms.hpp>
#include <sfc/KDCurveSpecification.hpp>
#include <sfc/Morton2DAlgorithms.hpp>
#include <time/Stopwatch.hpp>

#include "performance.hpp"
#include "rendering.hpp"

#include <iostream>
#include <limits>
#include <random>

using namespace sfcpp;

int eulerCharacteristic(geo::ConvexPolytope const &polytope) {
  int factor = 1;
  int sum = 0;
  for (size_t d = 0; d < polytope.faces.size(); ++d) {
    sum += factor * polytope.faces[d].size();
    factor *= (-1);
  }
  return sum;
}

math::PermutationSubgroup grammarGroup(std::vector<std::vector<size_t>> grammar) {
  math::PermutationSubgroup result;

  for (size_t i = 0; i < grammar[0].size(); ++i) {
    math::Permutation p(0);
    for (size_t j = 0; j < grammar.size(); ++j) {
      p.set(j, grammar[j][i]);
    }
    result.add(p);
  }

  return result;
}

struct HilbertData {
  double value;
  sfc::index_type neighbors[4] = {0};
  HilbertData(double v) : value(v){};
};

void testPoissonSolvers() {
  size_t level = 13;
  size_t numPoints = 1 << (2 * level);
  size_t numIterations = 10;
  size_t numFacets = 4;
  sfc::Hilbert2DAlgorithms h2D(level);
  sfc::Morton2DAlgorithms m2D;
  std::vector<double> hilbertVec(numPoints, 1.0);
  std::vector<double> mortonVec(numPoints, 1.0);
  std::vector<HilbertData> hilbertExtendedVec(numPoints, HilbertData(1.0));

  time::Stopwatch stopwatch;

  // init hilbert extended
  for (size_t i = 0; i < numPoints; ++i) {
    for (size_t f = 0; f < numFacets; ++f) {
      hilbertExtendedVec[i].neighbors[f] = h2D.neighbor(i, 0, f);
    }
  }

  stopwatch.log();

  stopwatch.start();

  for (size_t numIt = 0; numIt < numIterations; ++numIt) {
    for (size_t i = 0; i < numPoints; ++i) {
      double sum = 0.0;
      auto &entry = hilbertExtendedVec[i];
      for (size_t f = 0; f < numFacets; ++f) {
        auto idx = entry.neighbors[f];
        if (idx < numPoints) {
          sum += hilbertExtendedVec[idx].value;
        }
      }
      entry.value = sum / 4.0;
    }
  }

  stopwatch.log();

  stopwatch.start();

  for (size_t numIt = 0; numIt < numIterations; ++numIt) {
    for (size_t i = 0; i < numPoints; ++i) {
      double sum = 0.0;
      for (size_t f = 0; f < numFacets; ++f) {
        auto idx = h2D.neighbor(i, 0, f);
        if (idx < numPoints) {
          sum += hilbertVec[idx];
        }
      }
      hilbertVec[i] = sum / 4.0;
    }
  }

  stopwatch.log();

  stopwatch.start();

  for (size_t numIt = 0; numIt < numIterations; ++numIt) {
    for (size_t i = 0; i < numPoints; ++i) {
      double sum = 0.0;
      for (size_t f = 0; f < numFacets; ++f) {
        auto idx = m2D.neighbor(i, 0, f);
        if (idx < numPoints) {
          sum += mortonVec[idx];
        }
      }
      mortonVec[i] = sum / 4.0;
    }
  }

  stopwatch.log();
}

void testPoissonSolvers2() {
  size_t level = 15;
  size_t numPoints = 1 << (2 * level);
  size_t numIterations = 10;
  size_t numFacets = 4;
  sfc::Hilbert2DAlgorithms h2D(level);
  sfc::Morton2DAlgorithms m2D;
  {
    std::vector<double> hilbertVec(numPoints, 1.0);

    time::Stopwatch stopwatch;

    for (size_t numIt = 0; numIt < numIterations; ++numIt) {
      for (size_t i = 0; i < numPoints; ++i) {
        double sum = 0.0;
        for (size_t f = 0; f < numFacets; ++f) {
          auto idx = h2D.neighbor(i, 0, f);
          if (idx < numPoints) {
            sum += hilbertVec[idx];
          }
        }
        hilbertVec[i] = sum / 4.0;
      }
    }

    stopwatch.log();
  }

  {
    std::vector<double> mortonVec(numPoints, 1.0);

    time::Stopwatch stopwatch;

    for (size_t numIt = 0; numIt < numIterations; ++numIt) {
      for (size_t i = 0; i < numPoints; ++i) {
        double sum = 0.0;
        for (size_t f = 0; f < numFacets; ++f) {
          auto idx = m2D.neighbor(i, 0, f);
          if (idx < numPoints) {
            sum += mortonVec[idx];
          }
        }
        mortonVec[i] = sum / 4.0;
      }
    }

    stopwatch.log();
  }
}

template <size_t size>
struct MyVec {
  std::array<double, size> data;

  MyVec(double value) {
    for (size_t i = 0; i < size; ++i) {
      data[i] = value;
    }
  }

  MyVec<size> &operator+=(MyVec<size> const &other) {
    for (size_t i = 0; i < size; ++i) {
      data[i] += other.data[i];
    }
    return *this;
  }

  friend MyVec operator*(double factor, MyVec<size> const &v) {
    MyVec<size> result(0.0);
    for (size_t i = 0; i < size; ++i) {
      result.data[i] = factor * v.data[i];
    }
    return result;
  }
};

void testPoissonSolvers3() {
  size_t level = 7;
  size_t numPoints = 1 << (2 * level);
  size_t numIterations = 10;
  size_t numFacets = 4;
  sfc::Hilbert2DAlgorithms h2D(level);
  sfc::Morton2DAlgorithms m2D;

  typedef MyVec<10000> DataType;
  const DataType defaultValue(1);
  const DataType zeroValue(0);

  {
    std::vector<DataType> hilbertVec(numPoints, defaultValue);

    time::Stopwatch stopwatch;

    for (size_t numIt = 0; numIt < numIterations; ++numIt) {
      for (size_t i = 0; i < numPoints; ++i) {
        DataType sum = zeroValue;
        for (size_t f = 0; f < numFacets; ++f) {
          auto idx = h2D.neighbor(i, 0, f);
          if (idx < numPoints) {
            sum += hilbertVec[idx];
          }
        }
        hilbertVec[i] = 0.25 * sum;
      }
    }

    stopwatch.log();
  }

  {
    std::vector<DataType> mortonVec(numPoints, defaultValue);

    time::Stopwatch stopwatch;

    for (size_t numIt = 0; numIt < numIterations; ++numIt) {
      for (size_t i = 0; i < numPoints; ++i) {
        DataType sum = zeroValue;
        for (size_t f = 0; f < numFacets; ++f) {
          auto idx = m2D.neighbor(i, 0, f);
          if (idx < numPoints) {
            sum += mortonVec[idx];
          }
        }
        mortonVec[i] = 0.25 * sum;
      }
    }

    stopwatch.log();
  }
}

void testCubes() {
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);
  std::cout << "RAND_MAX: " << RAND_MAX << "\n";
  for (size_t d = 2; d <= 8; ++d) {
    std::cout << "Dimension of cube: " << d << "\n";
    auto cubeMatrix = sfc::KDCurveSpecification::generateUnitCube(d);
    // Eigen::MatrixXd cubeMatrix(3, 100000 * d);
    /*for (int row = 0; row < cubeMatrix.rows(); ++row) {
      for (int col = 0; col < cubeMatrix.cols(); ++col) {
        cubeMatrix(row, col) = distribution(generator);
      }
    }*/
    // std::cout << cubeMatrix << "\n";
    time::Stopwatch stopwatch;
    auto convHull = geo::ConvexPolytope::convexHull(cubeMatrix);
    stopwatch.log();
    std::cout << "Euler characteristic: " << eulerCharacteristic(*convHull) << "\n";
  }
  /*auto cubeMatrix = sfc::KDCurveSpecification::generateUnitCube(3);
  // cubeMatrix.conservativeResize(3, 6);
  // cubeMatrix(2, 3) = 1;
  // Eigen::MatrixXd cubeMatrix(2, 4);
  // cubeMatrix << 0, 1, 0, 0, 0, 0, 1, 2;
  std::cout << cubeMatrix << "\n";
  time::Stopwatch stopwatch;
  auto convHull = geo::ConvexPolytope::convexHull(cubeMatrix);
  stopwatch.log();
  std::cout << *convHull << "\n";
  std::cout << "Euler characteristic: " << eulerCharacteristic(*convHull) <<
  "\n";*/
}

double getMinDist(Eigen::MatrixXd mat) {
  double minDist = std::numeric_limits<double>::infinity();
  for (int i = 0; i + 1 < mat.cols(); ++i) {
    for (int j = i + 1; j < mat.cols(); ++j) {
      Eigen::VectorXd diff = mat.col(i) - mat.col(j);
      double norm = diff.norm();
      if (norm < minDist) {
        minDist = norm;
      }
    }
  }

  return minDist;
}

double getMaxDist(Eigen::MatrixXd mat) {
  double maxDist = -std::numeric_limits<double>::infinity();
  for (int i = 0; i + 1 < mat.cols(); ++i) {
    for (int j = i + 1; j < mat.cols(); ++j) {
      Eigen::VectorXd diff = mat.col(i) - mat.col(j);
      double norm = diff.norm();
      if (norm > maxDist) {
        maxDist = norm;
      }
    }
  }

  return maxDist;
}

bool maxDistSmaller(Eigen::MatrixXd mat, double threshold = 1.0) {
  for (int i = 0; i + 1 < mat.cols(); ++i) {
    for (int j = i + 1; j < mat.cols(); ++j) {
      Eigen::VectorXd diff = mat.col(i) - mat.col(j);
      double norm = diff.norm();
      if (norm >= threshold) {
        return false;
      }
    }
  }

  return true;
}

bool testConvergenceRecursive(std::shared_ptr<sfc::CurveSpecification> spec, Eigen::MatrixXd mat,
                              size_t maxDepth, size_t state, double threshold = 1.0) {
  // std::cout << "Mat: \n" << mat << "\n";

  if (maxDistSmaller(mat, threshold)) {
    // std::cout << "Remaining depth: " << maxDepth << "\n";
    // std::cout << "Matrix:\n" << mat << "\n";
    return true;
  }

  if (maxDepth == 0) {
    return false;
  }

  for (size_t i = 0; i < spec->transitionMats[state].size(); ++i) {
    Eigen::MatrixXd newMat = mat * spec->transitionMats[state][i];
    bool success = testConvergenceRecursive(spec, newMat, maxDepth - 1, spec->grammar[state][i]);

    if (!success) {
      return false;
    }
  }
  return true;
}

bool testConvergence(std::shared_ptr<sfc::CurveSpecification> spec, size_t maxDepth = 10) {
  for (size_t state = 0; state < spec->grammar.size(); ++state) {
    size_t initDim = spec->transitionMats[state][0].rows();
    Eigen::MatrixXd initialMat = Eigen::MatrixXd::Identity(initDim, initDim);
    double threshold = 0.9 * getMinDist(initialMat);
    bool success = testConvergenceRecursive(spec, initialMat, maxDepth, state, threshold);
    if (!success) {
      return false;
    }
  }
  return true;
}

void computeDeformationSpecsRecursive(std::vector<double> &maxLengthRatios,
                                      std::shared_ptr<sfc::CurveInformation> info,
                                      sfc::GeometricTreeNode const &node, size_t depth,
                                      size_t maxDepth) {
  if (depth > maxDepth) {
    return;
  }

  double maxDist = getMaxDist(node.points);
  double minDist = getMinDist(node.points);
  double quot = maxDist / minDist;

  if (quot > maxLengthRatios[depth]) {
    maxLengthRatios[depth] = quot;
  }

  auto children = info->getChildren(node);
  for (auto &child : children) {
    computeDeformationSpecsRecursive(maxLengthRatios, info, child, depth + 1, maxDepth);
  }
}

void computeDeformationSpecs(std::shared_ptr<sfc::CurveInformation> info, size_t maxDepth = 10) {
  std::vector<double> maxLengthRatios(maxDepth + 1, 0.0);
  computeDeformationSpecsRecursive(maxLengthRatios, info, info->getRootNode(), 0, maxDepth);
  std::cout << "Length ratios: \n";
  std::cout << strings::toString(maxLengthRatios, "\n");
}

void drawCurve() {
  // auto spec = sfc::KDCurveSpecification::getPeanoCurveSpecification(4, 3)
  //                 ->getCurveSpecification();
  // auto spec = sfc::KDCurveSpecification::getMortonCurveSpecification(2, 10)
  //                 ->getCurveSpecification();
  // auto spec =
  //    sfc::KDCurveSpecification::getHilbertCurveSpecification(2)->getLocalCurveSpecification();
  // auto spec = sfc::CurveSpecification::getBetaOmegaCurveSpecification();
  auto spec = sfc::CurveSpecification::getSierpinskiCurveSpecification(8);
  // auto spec = sfc::CurveSpecification::getGosperCurveSpecification();
  std::cout << *spec << "\n";
  std::cout << "Grammar:\n" << strings::toString(spec->grammar, "\n", ", ") << "\n\n";

  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem =
      worldSystem->createChild(geo::linearTransform(1, 0, 0.1, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  time::Stopwatch stopwatch;
  auto info = std::make_shared<sfc::CurveInformation>(spec);
  stopwatch.log();
  // std::cout << *info << "\n";

  // info->saveTableDefinitions("Hilbert3DDeclarations.cpp");

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(10);

  // print state sequence
  // renderer->renderLeaves([](sfc::SpacetreeNode const &node) { std::cout <<
  // node.state << "\n";
  // });
  /*auto group = grammarGroup(spec->grammar);
  std::cout << group << "\n";
  std::cout << "Grammar group size: " << group.getSize() << "\n";*/

  /*container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::Gray)));*/
  container->addElement(renderer->renderCurve(test::startColor, test::endColor));
  /*renderer->renderLeaves([&](sfc::SpacetreeNode const &node) {
    auto firstPoint = renderer->point(node.pointMatrix.col(0));
    auto secondPoint = renderer->point(node.pointMatrix.col(3));
    auto config = latex::tikz::TikzElementConfiguration::arrowConfiguration();
    container->addElement(std::make_shared<latex::tikz::TikzLine>(firstPoint,
  secondPoint, config));
  });*/

  /*renderer->setTreeStructure(2);
  container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::DarkGreen)));

  renderer->setTreeStructure(1);
  container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::LightBlue)));

  renderer->setTreeStructure(0);
  container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::LightRed)));*/

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  // document.saveAndCompile("TexCode/s-5d-10-gr.tex");
}

int main() {
  // testPoissonSolvers3();
  /*const double numSamples = 5000000;
  test::createPeanoDimPerformancePlots(numSamples);
  test::createPeanoDepthPerformancePlots(numSamples);
  test::create2DPerformancePlots(numSamples);
  test::createStatePerformancePlots(numSamples);*/

  try {
    // bool result = testConvergence(sfc::CurveSpecification::getSierpinskiCurveSpecification(7),
    // 16);
    // bool result = testConvergence(sfc::CurveSpecification::getGosperCurveSpecification(), 3);
    // bool result = testConvergence(sfc::CurveSpecification::getBetaOmegaCurveSpecification(), 10);
    // bool result = testConvergence(sfc::CurveSpecification::getCustomCurveSpecification1());
    // std::cout << "Convergence result: " << result << "\n";
    computeDeformationSpecs(std::make_shared<sfc::CurveInformation>(
                                sfc::CurveSpecification::getSierpinskiCurveSpecification(5)),
                            16);
    // testCubes();
    /*{ // initialize stack memory for debugging
        size_t data[10000] = {0};
        size_t myvar = 5;
        for (size_t i = 0; i < 10000; ++i) {
          myvar = (myvar + 1) ^ data[i];
        }
        std::cout << myvar << "\n";
      }*/
    // drawCurve();
    for (size_t depth = 0; depth <= 3; ++depth) {
      std::string dstr = strings::toString(depth);
      // test::renderHilbert2D("h-2d-" + dstr + "-gr.tex",
      //                      depth, depth);
      // test::renderCustom1("c-2d-" + dstr + "-grn.tex", depth, depth);
      // test::renderHilbert2DLocalCorners("h-2d-" + dstr + "+-grc.tex", depth,
      //                                   depth + 1);
    }
    /*for (size_t depth = 0; depth <= 2; ++depth) {
      std::string dstr = strings::toString(depth);
      test::renderHilbert3D("h-3d-" + dstr + "-grb.tex", depth, true);
    }
    for (size_t depth = 0; depth <= 3; ++depth) {
      std::string dstr = strings::toString(depth);
      test::renderHilbert2DGlobalSymbols(
          "h-2d-" + strings::toString(depth) + "+-grsg.tex", depth, depth + 1);
      test::renderHilbert2DLocalSymbols(
          "h-2d-" + strings::toString(depth) + "+-grsl.tex", depth, depth + 1);
      test::renderHilbert2DGlobalSymbols(
          "h-2d-" + strings::toString(depth) + "-grsg.tex", depth, depth);
      test::renderHilbert2DLocalSymbols(
          "h-2d-" + strings::toString(depth) + "-grsl.tex", depth, depth);
      test::renderPeano2DGlobalSymbols(
          "p3-2d-" + strings::toString(depth) + "-grsg.tex", depth, depth);
      test::renderHilbert2DOrder(
          "h-2d-" + strings::toString(depth) + "-gro.tex", depth);
      test::renderHilbert2DOrder(
          "h-2d-" + strings::toString(depth) + "-gro4.tex", depth, 4,
          std::max(1ul, depth));
      test::renderPeano2DLocalSymbols("p3-2d-" + dstr + "-grsl.tex", depth,
                                      depth);
      test::renderBetaOmega2DLocalSymbols("b-2d-" + dstr + "+-grsl.tex", depth,
                                          depth + 1);
      test::renderGosper2DLocalSymbols("g-2d-" + dstr + "-grsl.tex", depth,
                                       depth);
      test::renderMorton2DGlobalSymbols("m-2d-" + dstr + "-grsg.tex", depth,
                                        depth);
      test::renderHilbert3D("h-3d-" + dstr + "-gr.tex", depth, false);
    }

    for (size_t depth = 0; depth <= 6; ++depth) {
      std::string dstr = strings::toString(depth);
      test::renderSierpinski2DLocalSymbols("s-2d-" + dstr + "-grsl.tex", depth,
                                           depth);
    }*/

    /*for (size_t d = 2; d <= 20; ++d) {
      auto spec = sfc::CurveSpecification::getSierpinskiCurveSpecification(d);
      auto info = std::make_shared<sfc::CurveInformation>(spec);
    }*/

    // test::renderHilbert2DTree("h-2d-2-tree.tex", 2);
    // test::renderHilbert2DTree("h-2d-3-tree.tex", 3);
    // test::renderHilbert2DTree("h-2d-4-tree.tex", 4);

    /*test::renderHilbert2DAdaptive("h-2d-a-gr.tex",
                                  "11010100001000010000110000001000011000000011"
                                  "01000010000100001100000000000");*/

  } catch (std::runtime_error &error) {
    std::cout << "Exception: " << error.what() << "\n";
  }
}
