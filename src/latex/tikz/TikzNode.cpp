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



#include <latex/tikz/TikzNode.hpp>
#include <latex/tikz/tikz.hpp>
#include <strings/strings.hpp>

#include <cmath>
#include <iostream>

namespace sfcpp {
namespace latex {
namespace tikz {

TikzNode::TikzNode(std::string text, Eigen::Vector3d pos, TikzElementConfiguration config)
    : text(text), pos(pos), config(config), coordinateSystem() {}

TikzNode::TikzNode(const TikzNode& other)
    : text(other.text),
      pos(other.pos),
      config(other.config),
      coordinateSystem(other.coordinateSystem) {}

TikzNode::~TikzNode() {}

std::string TikzNode::getCode() const {
  double factor = 1.0;

  if (coordinateSystem) {
    auto matrix = coordinateSystem->getTransformationTo(coordinateSystem->getWorldSystem());
    Eigen::Vector4d v1 = matrix * Eigen::Vector4d(1, 0, 0, 0);
    Eigen::Vector4d v2 = matrix * Eigen::Vector4d(0, 1, 0, 0);
    Eigen::Vector3d w1(v1[0], v1[1], v1[2]);
    Eigen::Vector3d w2(v2[0], v2[1], v2[2]);
    factor = sqrt(w1.cross(w2).norm());
  }

  return "\\node[" + config.getCode(textFactor * factor) + "] (a) at " +
         coordCode(pos, coordinateSystem) + " {" + textPrefix + text + textPostfix + "};\n";
}

std::shared_ptr<TikzElement> TikzNode::clone() const { return std::make_shared<TikzNode>(*this); }

void TikzNode::setCoordinateSystem(
    const std::shared_ptr<sfcpp::geo::CoordinateSystem>& coordinateSystem) {
  this->coordinateSystem = coordinateSystem;
}

void TikzNode::setTextOrientation(Eigen::VectorXd first, Eigen::VectorXd second) {
  // TODO check that vectors are 2D

  double det = first(0) * second(1) - first(1) * second(0);
  if (det == 0) {
    throw std::runtime_error("TikzNode::setTextOrientation(): det = 0");
  }

  Eigen::Matrix2d mat;

  textFactor = sqrt(fabs(det));
  first.normalize();
  second.normalize();

  if (det < 0) {
    first = -first;
    textPrefix = "\\reflectbox{";
    textPostfix = "}";
  }

  double angle =
      180 / M_PI * std::atan2(static_cast<float>(first(1)), static_cast<float>(first(0)));

  textPrefix = "\\rotatebox{" + strings::toStringFixedPrecision(angle, 5) + "}{" + textPrefix;
  textPostfix += "}";
}

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */
