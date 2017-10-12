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



#include "CurveRenderer.hpp"
#include <iostream>
#include <latex/tikz/TikzElementContainer.hpp>
#include <latex/tikz/TikzLine.hpp>
#include <math/math.hpp>
#include <strings/strings.hpp>

#include <stdexcept>

namespace sfcpp {
namespace sfc {

std::string CurveRenderer::getStructureString(size_t numLevels) {
  if (numLevels == 0) {
    return "0";
  }

  return "1" + strings::repeat(getStructureString(numLevels - 1), spec->getNumChildren());
}

size_t CurveRenderer::refineRecursively(const std::shared_ptr<SpacetreeNode>& node,
                                        std::string& structure, size_t stringIndex) {
  if (stringIndex >= structure.size()) {
    // throw std::runtime_error(
    //    "CurveRenderer::refineRecursively(): Tree description is an invalid string");
    std::cout << "CurveRenderer::refineRecursively(): Tree description is an invalid string - Too "
                 "few characters";
    structure += "0";
  }

  if (structure[stringIndex] == '0') {
    // don't refine
    ++numLeaves;
    ++stringIndex;

  } else {
    ++stringIndex;

    node->isLeaf = false;
    // refine
    size_t b = spec->getNumChildren();
    for (size_t i = 0; i < b; ++i) {
      auto childNode = std::make_shared<SpacetreeNode>(
          spec->grammar[node->state][i], node->level + 1, b * node->index + i, true,
          node->pointMatrix * spec->transitionMats[node->state][i]);
      node->children.push_back(childNode);
      stringIndex = refineRecursively(childNode, structure, stringIndex);
    }
  }
  return stringIndex;
}

void CurveRenderer::renderNodesRecursively(std::shared_ptr<SpacetreeNode> const& node,
                                           const RenderFunction& func, bool notOnlyLeaves) const {
  if (node->isLeaf || notOnlyLeaves) {
    func(*node);
  }

  for (auto& child : node->children) {
    renderNodesRecursively(child, func, notOnlyLeaves);
  }
}

CurveRenderer::CurveRenderer(std::shared_ptr<CurveSpecification> spec,
                             std::shared_ptr<CurveInformation> info)
    : spec(spec), info(info), root(nullptr), numLeaves(0) {
  setTreeStructure("0");
}

void CurveRenderer::setTreeStructure(std::string const& structure) {
  std::string structureCopy = structure;
  numLeaves = 0;
  root = std::make_shared<SpacetreeNode>(0, 0, 0, true, spec->rootPoints);
  size_t stringIndex = refineRecursively(root, structureCopy, 0);
  if (stringIndex != structure.size()) {
    std::cout << "CurveRenderer::setTreeStructure(): Tree description is an invalid string - Too "
                 "many characters";
    // throw std::runtime_error(
    //     "CurveRenderer::setTreeStructure(): Tree description is an invalid string");
  }
}

void CurveRenderer::setTreeStructure(size_t numLevels) {
  setTreeStructure(getStructureString(numLevels));
}

void CurveRenderer::renderNodes(const RenderFunction& func) const {
  renderNodesRecursively(root, func, true);
}

void CurveRenderer::renderLeaves(const RenderFunction& func) const {
  renderNodesRecursively(root, func, false);
}

size_t CurveRenderer::getNumLeaves() const { return numLeaves; }

Eigen::Vector3d CurveRenderer::point(Eigen::VectorXd coordinates) const {
  Eigen::Vector3d result = Eigen::Vector3d::Zero();

  // TODO(holzmudd)
  for (int i = 0; i < coordinates.rows() && i < 3; ++i) {
    result[i] = coordinates[i];
  }

  for (int i = 3; i < coordinates.rows(); ++i) {
    result[0] += -0.3 * coordinates[i];
    result[1] += 0.8 * coordinates[i];
  }

  return result;
}

std::shared_ptr<latex::tikz::TikzElement> CurveRenderer::renderBoundaries(
    const latex::tikz::TikzElementConfiguration& elementConfig) const {
  auto element = std::make_shared<latex::tikz::TikzElementContainer>();

  renderFaces([this, &element, &elementConfig](SpacetreeFace const& face) {
    if (face.node.isLeaf && face.dim == 1) {
      if (face.vertices.size() != 2) {
        throw std::runtime_error("CurveRenderer::renderBoundaries(): edge doesn't have 2 vertices");
      }

      element->addElement(std::make_shared<latex::tikz::TikzLine>(
          point(face.vertices[0]), point(face.vertices[1]), elementConfig));
    }
  });

  return element;
}

void CurveRenderer::renderFaces(FaceRenderFunction const& func) const {
  renderNodes([this, &func](SpacetreeNode const& node) {
    auto polytope = info->getPolytopeForState(node.state);
    for (size_t dim = 0; dim <= polytope->getDimension(); ++dim) {
      for (size_t faceIndex = 0; faceIndex < polytope->faces[dim].size(); ++faceIndex) {
        std::vector<Eigen::VectorXd> vertices;
        auto& face = polytope->faces[dim][faceIndex];
        for (auto i : face.vertices) {
          vertices.push_back(node.pointMatrix.col(i));
        }

        SpacetreeFace spacetreeFace(dim, faceIndex, node, vertices);

        func(spacetreeFace);
      }
    }
  });
}

std::vector<Eigen::VectorXd> CurveRenderer::getCurveMidpoints() const {
  std::vector<Eigen::VectorXd> curveMidpoints;
  curveMidpoints.reserve(numLeaves);

  renderLeaves([&curveMidpoints](SpacetreeNode const& node) {
    curveMidpoints.push_back(node.getBarycenter());
  });

  return curveMidpoints;
}

std::shared_ptr<latex::tikz::TikzElement> CurveRenderer::renderCurve(
    img::HSVColor startColor, img::HSVColor endColor, latex::tikz::TikzElementConfiguration config,
    size_t startIndex, size_t endIndex) const {
  auto curvePoints = getCurveMidpoints();
  std::shared_ptr<latex::tikz::TikzElementContainer> container(
      new latex::tikz::TikzElementContainer());
  if (curvePoints.size() == 0) {
    return container;
  }

  endIndex = std::min(endIndex, curvePoints.size() - 1);
  if (endIndex <= startIndex) {
    return container;
  }

  double n = endIndex - startIndex;

  for (size_t i = startIndex; i < endIndex; ++i) {
    double t = math::uniformPointWithBorder(n, i - startIndex);
    img::HSVColor hsvColor = math::interpolateLinear(startColor, endColor, t);
    config.drawColor = img::Color::fromHSV(hsvColor);
    container->addElement(std::make_shared<latex::tikz::TikzLine>(
        point(curvePoints[i]), point(curvePoints[i + 1]), config));
  }

  return container;
}

} /* namespace sfc */
} /* namespace sfcpp */
