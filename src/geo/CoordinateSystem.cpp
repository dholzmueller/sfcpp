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



#include <geo/CoordinateSystem.hpp>

namespace sfcpp {
namespace geo {

CoordinateSystem::CoordinateSystem(std::shared_ptr<CoordinateSystemManager> manager, std::shared_ptr<CoordinateSystemTreeNode> node) :
		manager(manager), node(node) {
}

std::shared_ptr<CoordinateSystem> CoordinateSystem::worldSystem(std::shared_ptr<CoordinateSystemManager> manager) {
	return std::make_shared<CoordinateSystem>(manager, manager->root);
}

std::shared_ptr<CoordinateSystem> CoordinateSystem::getWorldSystem() const {
	return worldSystem(manager);
}

std::shared_ptr<CoordinateSystem> CoordinateSystem::createChild(Eigen::Matrix4d toParent) {
	return std::make_shared<CoordinateSystem>(manager, node->createChild(toParent));
}

std::shared_ptr<CoordinateSystem> CoordinateSystem::createChild(Eigen::Matrix4d toParent, Eigen::Matrix4d fromParent) {
	return std::make_shared<CoordinateSystem>(manager, node->createChild(toParent, fromParent));
}

void CoordinateSystem::addChild(std::shared_ptr<CoordinateSystem> const &other) {
	node->addChild(other->node);

	// do this operation after adding the child because it might delete the old other.manager and its tree,
	// especially the parent of other.node from which other.node is removed
	other->manager = manager;
}

void CoordinateSystem::setTransformationToParent(Eigen::Matrix4d toParent) {
	node->setTransformationToParent(toParent);
}

void CoordinateSystem::setTransformationFromParent(Eigen::Matrix4d fromParent) {
	node->setTransformationFromParent(fromParent);
}

Eigen::Matrix4d CoordinateSystem::getTransformationTo(std::shared_ptr<CoordinateSystem> const &other) const {
	if (manager.get() != other->manager.get()) {
		throw std::runtime_error("CoordinateSystem::getTransformationTo(): Can't get transformation between two nodes of two different CoordinateSystemManager objects");
	}

	return node->getTransformationTo(other->node);
}

Eigen::Vector4d CoordinateSystem::transform(const Eigen::Vector4d& vec, std::shared_ptr<CoordinateSystem> const &other) const {
	if (manager.get() != other->manager.get()) {
		throw std::runtime_error("CoordinateSystem::transform(): Can't get transformation between two nodes of two different CoordinateSystemManager objects");
	}

	return node->transform(vec, other->node);
}

Eigen::Vector4d CoordinateSystem::toWorldCoordinates(const Eigen::Vector4d& vec) const {
	return node->toWorldCoordinates(vec);
}

Eigen::Vector4d CoordinateSystem::fromWorldCoordinates(const Eigen::Vector4d& vec) const {
	return node->fromWorldCoordinates(vec);
}

} /* namespace geo */
} /* namespace utils */
