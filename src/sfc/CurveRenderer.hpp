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

#include <Eigen/Dense>

#include <latex/tikz/TikzElement.hpp>
#include <latex/tikz/TikzElementConfiguration.hpp>
#include <sfc/CurveInformation.hpp>
#include <sfc/CurveSpecification.hpp>

#include <memory>
#include <string>

namespace sfcpp {
namespace sfc {

/**
 * Helper class that realizes a node of a tree.
 */
struct SpacetreeNode {
  size_t state;
  size_t level;
  size_t index;
  bool isLeaf;
  Eigen::MatrixXd pointMatrix;
  std::vector<std::shared_ptr<SpacetreeNode>> children;

  SpacetreeNode(size_t state, size_t level, size_t index, bool isLeaf,
                Eigen::MatrixXd const &pointMatrix)
      : state(state),
        level(level),
        index(index),
        isLeaf(isLeaf),
        pointMatrix(pointMatrix) {}

  SpacetreeNode()
      : state(0), level(0), index(0), isLeaf(true), pointMatrix(), children() {}

  Eigen::VectorXd getBarycenter() const {
    return (1.0 / pointMatrix.cols()) * pointMatrix *
           Eigen::VectorXd::Ones(pointMatrix.cols());
  }
};

/**
 * Helper class for rendering faces.
 */
struct SpacetreeFace {
  size_t dim;
  size_t faceNumber;
  SpacetreeNode node;
  std::vector<Eigen::VectorXd> vertices;

  SpacetreeFace(size_t dim, size_t faceNumber, SpacetreeNode node,
                std::vector<Eigen::VectorXd> vertices)
      : dim(dim), faceNumber(faceNumber), node(node), vertices(vertices) {}
};

/**
 * This class can be used to generate Latex/TikZ code for CurveSpecification
 * objects.
 */
class CurveRenderer {
 public:
  typedef std::function<void(SpacetreeNode const &)> RenderFunction;
  typedef std::function<void(SpacetreeFace const &)> FaceRenderFunction;

 private:
  std::shared_ptr<CurveSpecification> spec;
  std::shared_ptr<CurveInformation> info;

  std::string structureString;
  std::shared_ptr<SpacetreeNode> root;
  size_t numLeaves;

  std::string getStructureString(size_t numLevels);
  size_t refineRecursively(std::shared_ptr<SpacetreeNode> const &node,
                           std::string &structure, size_t index);
  void renderNodesRecursively(std::shared_ptr<SpacetreeNode> const &node,
                              RenderFunction const &func,
                              bool notOnlyLeaves) const;

 public:
  CurveRenderer(std::shared_ptr<CurveSpecification> spec,
                std::shared_ptr<CurveInformation> info);

  /**
   * Sets the structure of the tree to be rendered.
   *
   * @param structure a string that corresponds to the output of a preorder
   * traversal where each inner node outputs a "1" and each leaf outputs a "0".
   */
  void setTreeStructure(std::string const &structure);

  /**
   * Sets the structure of the tree to a regular grid.
   *
   * @param level level of the leaves in the tree.
   */
  void setTreeStructure(size_t level);

  void renderNodes(RenderFunction const &func) const;
  void renderLeaves(RenderFunction const &func) const;
  void renderFaces(FaceRenderFunction const &func) const;

  size_t getNumLeaves() const;

  /**
   * Renders all 1D faces of all leaves.
   */
  std::shared_ptr<latex::tikz::TikzElement> renderBoundaries(
      latex::tikz::TikzElementConfiguration const &elementConfig) const;

  /**
   * Returns a list of the barycenters of all nodes in the correct order
   */
  std::vector<Eigen::VectorXd> getCurveMidpoints() const;

  /**
   * Connects all barycenters using a linear color transition
   */
  std::shared_ptr<latex::tikz::TikzElement> renderCurve(
      img::HSVColor startColor, img::HSVColor endColor,
      latex::tikz::TikzElementConfiguration config =
          latex::tikz::TikzElementConfiguration(),
      size_t startIndex = 0,
      size_t endIndex = std::numeric_limits<size_t>::max()) const;

  /**
   * @return Returns an appropriate 3D representation of the point
   */
  Eigen::Vector3d point(Eigen::VectorXd coordinates) const;
};

} /* namespace sfc */
} /* namespace sfcpp */
