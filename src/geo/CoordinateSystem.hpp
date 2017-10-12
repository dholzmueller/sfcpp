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



#ifndef SRC_GEO_COORDINATESYSTEM_HPP_
#define SRC_GEO_COORDINATESYSTEM_HPP_

#include "CoordinateSystemManager.hpp"

#include <memory>

namespace sfcpp {
namespace geo {

/**
 * CoordinateSystemTreeNodes are not used directly for two main reasons:
 * - Not keeping a shared_ptr to the root system would mean that all nodes are
 * deallocated (they don't keep circular references).
 * 		Because any CoordinateSystem keeps a shared_ptr to the Manager
 * which in turn keeps a shared_ptr to the root,
 * 		there are no deallocation problems.
 * - You can always retrieve a shared_ptr to the root via the manager.
 *
 */
class CoordinateSystem {
  std::shared_ptr<CoordinateSystemManager> manager;
  std::shared_ptr<CoordinateSystemTreeNode>
      node;  // doesn't need a shared pointer since it is managed by the manager

 public:
  CoordinateSystem(std::shared_ptr<CoordinateSystemManager> manager,
                   std::shared_ptr<CoordinateSystemTreeNode> node);

  static std::shared_ptr<CoordinateSystem> worldSystem(
      std::shared_ptr<CoordinateSystemManager> manager);
  std::shared_ptr<CoordinateSystem> getWorldSystem() const;

  std::shared_ptr<CoordinateSystem> createChild(
      Eigen::Matrix4d toParent = Eigen::Matrix4d::Identity());
  std::shared_ptr<CoordinateSystem> createChild(Eigen::Matrix4d toParent,
                                                Eigen::Matrix4d fromParent);

  /**
   * other must have the same manager as this coordinate system
   */
  void addChild(std::shared_ptr<CoordinateSystem> const &other);

  void setTransformationToParent(Eigen::Matrix4d toParent);
  void setTransformationFromParent(Eigen::Matrix4d fromParent);

  Eigen::Matrix4d getTransformationTo(
      std::shared_ptr<CoordinateSystem> const &other) const;
  Eigen::Vector4d transform(
      Eigen::Vector4d const &vec,
      std::shared_ptr<CoordinateSystem> const &other) const;
  Eigen::Vector4d toWorldCoordinates(Eigen::Vector4d const &vec) const;
  Eigen::Vector4d fromWorldCoordinates(Eigen::Vector4d const &vec) const;
};

} /* namespace geo */
} /* namespace utils */

#endif /* SRC_GEO_COORDINATESYSTEM_HPP_ */
