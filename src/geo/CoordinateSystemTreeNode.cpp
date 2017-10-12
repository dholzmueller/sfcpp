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



#include <algorithm>
#include <geo/CoordinateSystemTreeNode.hpp>

#include <iostream>

namespace sfcpp {
namespace geo {

void CoordinateSystemTreeNode::parentUpdated() {
  toWorld = parent->toWorld * toParent;
  fromWorld = fromParent * parent->fromWorld;
}

void CoordinateSystemTreeNode::updateChildren() {
  for (auto child : children) {
    child->parentUpdated();
  }
}

void CoordinateSystemTreeNode::setParent(CoordinateSystemTreeNode* parent) {
  this->parent->removeChild(this);
  this->parent = parent;
  parentUpdated();
}

void CoordinateSystemTreeNode::removeChild(CoordinateSystemTreeNode* child) {
  for (size_t i = 0; i < children.size(); ++i) {
    if (children[i].get() == child) {
      children.erase(children.begin() + i);
    }
  }
}

CoordinateSystemTreeNode::CoordinateSystemTreeNode()
    : parent(NULL),
      children(),
      fromParent(Eigen::Matrix4d::Identity()),
      toParent(Eigen::Matrix4d::Identity()),
      fromWorld(Eigen::Matrix4d::Identity()),
      toWorld(Eigen::Matrix4d::Identity()) {}

CoordinateSystemTreeNode::CoordinateSystemTreeNode(CoordinateSystemTreeNode* parent,
                                                   Eigen::Matrix4d toParent)
    : parent(parent),
      children(),
      fromParent(toParent.inverse()),
      toParent(toParent),
      fromWorld(this->fromParent * parent->fromWorld),
      toWorld(parent->toWorld * this->toParent) {}

CoordinateSystemTreeNode::CoordinateSystemTreeNode(CoordinateSystemTreeNode* parent,
                                                   Eigen::Matrix4d toParent,
                                                   Eigen::Matrix4d fromParent)
    : parent(parent),
      children(),
      fromParent(fromParent),
      toParent(toParent),
      fromWorld(this->fromParent * parent->fromWorld),
      toWorld(parent->toWorld * this->toParent) {}

void CoordinateSystemTreeNode::addChild(std::shared_ptr<CoordinateSystemTreeNode> child) {
  child->setParent(this);
  children.push_back(child);
}

std::shared_ptr<CoordinateSystemTreeNode> CoordinateSystemTreeNode::createChild(
    Eigen::Matrix4d toParent) {
  std::shared_ptr<CoordinateSystemTreeNode> child(new CoordinateSystemTreeNode(this, toParent));
  children.push_back(child);
  return child;
}

std::shared_ptr<CoordinateSystemTreeNode> CoordinateSystemTreeNode::createChild(
    Eigen::Matrix4d toParent, Eigen::Matrix4d fromParent) {
  std::shared_ptr<CoordinateSystemTreeNode> child(
      new CoordinateSystemTreeNode(this, toParent, fromParent));
  children.push_back(child);
  return child;
}

void CoordinateSystemTreeNode::setTransformationToParent(Eigen::Matrix4d toParent) {
  this->toParent = toParent;
  this->fromParent = toParent.inverse();
  parentUpdated();
  updateChildren();
}

void CoordinateSystemTreeNode::setTransformationFromParent(Eigen::Matrix4d fromParent) {
  this->fromParent = fromParent;
  this->toParent = fromParent.inverse();
}

Eigen::Matrix4d CoordinateSystemTreeNode::getTransformationTo(
    std::shared_ptr<CoordinateSystemTreeNode> other) const {
  return other->fromWorld * this->toWorld;
}

Eigen::Vector4d CoordinateSystemTreeNode::transform(
    const Eigen::Vector4d& vec, std::shared_ptr<CoordinateSystemTreeNode> other) const {
  return other->fromWorld * (this->toWorld * vec);
}

Eigen::Vector4d CoordinateSystemTreeNode::toWorldCoordinates(const Eigen::Vector4d& vec) const {
  return this->toWorld * vec;
}

Eigen::Vector4d CoordinateSystemTreeNode::fromWorldCoordinates(const Eigen::Vector4d& vec) const {
  return this->fromWorld * vec;
}

} /* namespace geo */
} /* namespace utils */
