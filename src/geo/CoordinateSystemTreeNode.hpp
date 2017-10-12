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



#ifndef SRC_GEO_COORDINATESYSTEMTREENODE_HPP_
#define SRC_GEO_COORDINATESYSTEMTREENODE_HPP_

#include <Eigen/Dense>

#include <memory>
#include <vector>

namespace sfcpp {
namespace geo {

/**
 * It has to be assured that the world coordinate system lives at least as long
 * as the other coordinate systems are used.
 */
class CoordinateSystemTreeNode {
  CoordinateSystemTreeNode *parent;
  std::vector<std::shared_ptr<CoordinateSystemTreeNode>> children;

  Eigen::Matrix4d fromParent;
  Eigen::Matrix4d toParent;

  Eigen::Matrix4d fromWorld;
  Eigen::Matrix4d toWorld;

  void parentUpdated();
  void updateChildren();

  /**
   * This method can't set itself as a child of the parent because it does not
   * have a shared_ptr to itself.
   * But in can update its own parent variable.
   */
  void setParent(CoordinateSystemTreeNode *parent);
  void removeChild(CoordinateSystemTreeNode *child);

  CoordinateSystemTreeNode(CoordinateSystemTreeNode *parent,
                           Eigen::Matrix4d toParent);
  CoordinateSystemTreeNode(CoordinateSystemTreeNode *parent,
                           Eigen::Matrix4d toParent,
                           Eigen::Matrix4d fromParent);

 public:
  CoordinateSystemTreeNode();  // create world coordinate system

  /**
   * If the child already has a parent, the child is removed from the other
   * parent.
   */
  void addChild(std::shared_ptr<CoordinateSystemTreeNode> child);
  std::shared_ptr<CoordinateSystemTreeNode> createChild(
      Eigen::Matrix4d toParent);
  std::shared_ptr<CoordinateSystemTreeNode> createChild(
      Eigen::Matrix4d toParent, Eigen::Matrix4d fromParent);

  void setTransformationToParent(Eigen::Matrix4d toParent);
  void setTransformationFromParent(Eigen::Matrix4d fromParent);

  Eigen::Matrix4d getTransformationTo(
      std::shared_ptr<CoordinateSystemTreeNode> other) const;
  Eigen::Vector4d transform(Eigen::Vector4d const &vec,
                      std::shared_ptr<CoordinateSystemTreeNode> other) const;
  Eigen::Vector4d toWorldCoordinates(Eigen::Vector4d const &vec) const;
  Eigen::Vector4d fromWorldCoordinates(Eigen::Vector4d const &vec) const;
};

} /* namespace geo */
} /* namespace utils */

#endif /* SRC_GEO_COORDINATESYSTEMTREENODE_HPP_ */
