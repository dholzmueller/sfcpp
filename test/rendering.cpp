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



#include "rendering.hpp"

#include <geo/ConvexPolytope.hpp>
#include <geo/geo.hpp>
#include <img/Color.hpp>
#include <latex/LatexDocument.hpp>
#include <latex/tikz/TikzElementConfiguration.hpp>
#include <latex/tikz/TikzLine.hpp>
#include <latex/tikz/TikzNode.hpp>
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

namespace sfcpp {
namespace test {

void renderCurveWithSymbols(
    std::string filename, size_t squareDepth, size_t curveDepth,
    std::shared_ptr<sfc::CurveSpecification> spec,
    std::function<std::string(sfc::SpacetreeNode const &)> symbolFunc,
    double fontsize = latex::tikz::TikzFontsize::large) {
  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem = worldSystem->createChild(
      geo::linearTransform(1, 0, 0.1, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(
      geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  auto info = std::make_shared<sfc::CurveInformation>(spec);

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(squareDepth);
  container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::Gray)));

  renderer->setTreeStructure(curveDepth);
  container->addElement(
      renderer->renderCurve(test::startColor, test::endColor));

  renderer->setTreeStructure(squareDepth);
  renderer->renderLeaves([&](sfc::SpacetreeNode const &node) {
    auto midpoint = node.getBarycenter();
    auto firstVector = node.pointMatrix.col(1) - node.pointMatrix.col(0);
    auto secondVector = node.pointMatrix.col(2) - node.pointMatrix.col(0);
    auto label = std::make_shared<latex::tikz::TikzNode>(
        symbolFunc(node), renderer->point(midpoint),
        latex::tikz::TikzElementConfiguration::labelConfiguration(fontsize,
                                                                  true));
    label->setTextOrientation(firstVector, secondVector);
    container->addElement(label);
  });

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/" + filename);
}

void renderCurveWithStateSymbols(
    std::string filename, size_t squareDepth, size_t curveDepth,
    std::shared_ptr<sfc::CurveSpecification> spec,
    std::function<std::string(size_t)> symbolFunc) {
  renderCurveWithSymbols(
      filename, squareDepth, curveDepth, spec,
      [&](sfc::SpacetreeNode const &node) { return symbolFunc(node.state); });
}

void renderHilbert2DGlobalSymbols(std::string filename, size_t squareDepth,
                                  size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getCurveSpecification();
  renderCurveWithStateSymbols(
      filename, squareDepth, curveDepth, spec,
      [](size_t state) { return hilbert2DSymbols[state]; });
}

void renderHilbert2DAdaptive(std::string filename, std::string treeString) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getCurveSpecification();
  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem = worldSystem->createChild(
      geo::linearTransform(1, 0, 0.1, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(
      geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  auto info = std::make_shared<sfc::CurveInformation>(spec);

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(treeString);
  container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::Gray)));

  container->addElement(
      renderer->renderCurve(test::startColor, test::endColor));

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/" + filename);
}

void renderHilbert2DLocalCorners(std::string filename, size_t squareDepth,
                                 size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getLocalCurveSpecification();
  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem = worldSystem->createChild(
      geo::linearTransform(1, 0, 0.1, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(
      geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  auto info = std::make_shared<sfc::CurveInformation>(spec);

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(squareDepth);
  container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::Gray)));

  renderer->setTreeStructure(curveDepth);

  container->addElement(
      renderer->renderCurve(test::startColor, test::endColor));

  renderer->setTreeStructure(squareDepth);

  renderer->renderLeaves([&](sfc::SpacetreeNode const &node) {
    auto corner1 =
        0.8 * node.pointMatrix.col(0) + 0.2 * node.pointMatrix.col(3);
    auto corner2 =
        0.8 * node.pointMatrix.col(1) + 0.2 * node.pointMatrix.col(2);
    auto corner3 =
        0.8 * node.pointMatrix.col(2) + 0.2 * node.pointMatrix.col(1);
    auto corner4 =
        0.8 * node.pointMatrix.col(3) + 0.2 * node.pointMatrix.col(0);
    auto firstVector = node.pointMatrix.col(1) - node.pointMatrix.col(0);
    auto secondVector = node.pointMatrix.col(2) - node.pointMatrix.col(0);
    double scale = sqrt(fabs(firstVector(0) * secondVector(1) -
                             secondVector(0) * firstVector(1)));
    auto config = latex::tikz::TikzElementConfiguration::labelConfiguration(
        latex::tikz::TikzFontsize::Small * scale, true);
    auto label1 = std::make_shared<latex::tikz::TikzNode>(
        "1", renderer->point(corner1), config);
    auto label2 = std::make_shared<latex::tikz::TikzNode>(
        "2", renderer->point(corner2), config);
    auto label3 = std::make_shared<latex::tikz::TikzNode>(
        "3", renderer->point(corner3), config);
    auto label4 = std::make_shared<latex::tikz::TikzNode>(
        "4", renderer->point(corner4), config);
    container->addElement(label1);
    container->addElement(label2);
    container->addElement(label3);
    container->addElement(label4);
  });

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/" + filename);
}

void renderHilbert3D(std::string filename, size_t depth, bool drawBoundary) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(3)
                  ->getCurveSpecification();
  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem = worldSystem->createChild(
      geo::linearTransform(1, 0, 0.2, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(
      geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  auto info = std::make_shared<sfc::CurveInformation>(spec);

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(depth);
  if (drawBoundary) {
    container->addElement(renderer->renderBoundaries(
        latex::tikz::TikzElementConfiguration(img::Color::LightGray)));
  }

  auto curveConfig = latex::tikz::TikzElementConfiguration();
  curveConfig.thicknessCm = latex::tikz::TikzThickness::Thick;

  container->addElement(
      renderer->renderCurve(test::startColor, test::endColor, curveConfig));

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/" + filename);
}

void renderHilbert2D(std::string filename, size_t squareDepth,
                     size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getLocalCurveSpecification();

  renderCurveWithStateSymbols(filename, squareDepth, curveDepth, spec,
                              [](size_t) { return ""; });
}

void renderHilbert2DLocalSymbols(std::string filename, size_t squareDepth,
                                 size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getLocalCurveSpecification();

  renderCurveWithStateSymbols(filename, squareDepth, curveDepth, spec,
                              [](size_t) { return hilbert2DLocalSymbol; });
}

void renderHilbert2DTree(std::string filename, size_t depth) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getCurveSpecification();
  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem = worldSystem->createChild(
      geo::linearTransform(1, 0, 0.1, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(
      geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  auto info = std::make_shared<sfc::CurveInformation>(spec);

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(depth);
  auto connectionConfig = latex::tikz::TikzElementConfiguration();
  auto boundaryConfig = latex::tikz::TikzElementConfiguration(img::Color::Gray);

  const double heightFactor = 0.5;

  renderer->renderNodes([&](sfc::SpacetreeNode const &node) {
    auto midpoint = node.getBarycenter();
    Eigen::VectorXd midpointVec(3);
    midpointVec << midpoint(0), -static_cast<double>(node.level) * heightFactor,
        midpoint(1);
    for (auto &child : node.children) {
      auto childMidpoint = child->getBarycenter();
      Eigen::VectorXd childMidpointVec(3);
      childMidpointVec << childMidpoint(0),
          -static_cast<double>(child->level) * heightFactor, childMidpoint(1);

      auto line = std::make_shared<latex::tikz::TikzLine>(
          renderer->point(midpointVec), renderer->point(childMidpointVec),
          connectionConfig);
      container->addElement(line);
    }
  });

  renderer->renderFaces([&](sfc::SpacetreeFace const &face) {
    if (face.dim == 1 && face.vertices.size() == 2) {
      Eigen::VectorXd firstVec(3), secondVec(3);
      firstVec << face.vertices[0](0),
          -static_cast<double>(face.node.level) * heightFactor,
          face.vertices[0](1);
      secondVec << face.vertices[1](0),
          -static_cast<double>(face.node.level) * heightFactor,
          face.vertices[1](1);
      auto line = std::make_shared<latex::tikz::TikzLine>(
          renderer->point(firstVec), renderer->point(secondVec),
          boundaryConfig);
      container->addElement(line);
    }
  });

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/" + filename);
}

void renderPeano2DGlobalSymbols(std::string filename, size_t squareDepth,
                                size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getPeanoCurveSpecification(2, 3)
                  ->getCurveSpecification();

  renderCurveWithStateSymbols(
      filename, squareDepth, curveDepth, spec,
      [](size_t state) { return peano2DSymbols[state]; });
}

void renderHilbert2DOrder(std::string filename, size_t depth, size_t base,
                          size_t minNumDigits) {
  auto spec = sfc::KDCurveSpecification::getHilbertCurveSpecification(2)
                  ->getCurveSpecification();
  renderCurveWithSymbols(filename, depth, depth, spec,
                         [base, minNumDigits](sfc::SpacetreeNode const &node) {
                           return strings::toStringFromBase(node.index, base,
                                                            minNumDigits);
                         });
}

void renderPeano2DLocalSymbols(std::string filename, size_t squareDepth,
                               size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getPeanoCurveSpecification(2, 3)
                  ->getLocalCurveSpecification();

  renderCurveWithStateSymbols(filename, squareDepth, curveDepth, spec,
                              [](size_t) { return peano2DLocalSymbol; });
}

void renderSierpinski2DLocalSymbols(std::string filename, size_t squareDepth,
                                    size_t curveDepth) {
  auto spec = sfc::CurveSpecification::getSierpinskiCurveSpecification();

  renderCurveWithStateSymbols(filename, squareDepth, curveDepth, spec,
                              [](size_t) { return sierpinski2DLocalSymbol; });
}

void renderBetaOmega2DLocalSymbols(std::string filename, size_t squareDepth,
                                   size_t curveDepth) {
  auto spec = sfc::CurveSpecification::getBetaOmegaCurveSpecification();

  renderCurveWithStateSymbols(
      filename, squareDepth, curveDepth, spec,
      [](size_t state) { return betaOmega2DLocalSymbols[state]; });
}

void renderGosper2DLocalSymbols(std::string filename, size_t squareDepth,
                                size_t curveDepth) {
  auto spec = sfc::CurveSpecification::getGosperCurveSpecification();

  renderCurveWithSymbols(filename, squareDepth, curveDepth, spec,
                         [](sfc::SpacetreeNode const &node) {
                           return gosper2DLocalSymbols[node.state];
                         },
                         latex::tikz::TikzFontsize::large * 2.5);
}

void renderMorton2DGlobalSymbols(std::string filename, size_t squareDepth,
                                 size_t curveDepth) {
  auto spec = sfc::KDCurveSpecification::getMortonCurveSpecification(2, 2)
                  ->getCurveSpecification();

  renderCurveWithStateSymbols(filename, squareDepth, curveDepth, spec,
                              [](size_t) { return morton2DGlobalSymbol; });
}

void renderCustom1(std::string filename, size_t squareDepth,
                   size_t curveDepth) {
  auto spec = sfc::CurveSpecification::getCustomCurveSpecification1();
  /*renderCurveWithStateSymbols(filename, squareDepth, curveDepth, spec,
                              [](size_t) { return ""; });*/

  double factor = 5.0;
  std::shared_ptr<latex::tikz::TikzPicture> picture(
      new latex::tikz::TikzPicture());
  auto worldSystem = picture->getWorldSystem();
  auto perspectiveSystem = worldSystem->createChild(
      geo::linearTransform(1, 0, 0.1, 0, 1, 0, 0, 0, -1));
  auto unitSystem = perspectiveSystem->createChild(
      geo::scaleTransform(factor, factor, factor));

  auto container = std::make_shared<latex::tikz::TikzElementContainer>();
  picture->addElement(container);

  auto info = std::make_shared<sfc::CurveInformation>(spec);

  auto renderer = std::make_shared<sfc::CurveRenderer>(spec, info);
  renderer->setTreeStructure(squareDepth);
  /*container->addElement(renderer->renderBoundaries(
      latex::tikz::TikzElementConfiguration(img::Color::Gray)));*/

  renderer->setTreeStructure(curveDepth);
  container->addElement(
      renderer->renderCurve(test::startColor, test::endColor));

  container->setCoordinateSystem(unitSystem);

  latex::LatexDocument document;
  document.addElement(picture);
  document.saveAndCompile("TexCode/" + filename);
}

} /* namespace sfc */
} /* namespace sfcpp */
