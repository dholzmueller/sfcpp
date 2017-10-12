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



#include <latex/tikz/TikzLine.hpp>
#include <latex/tikz/tikz.hpp>

namespace sfcpp {
namespace latex {
namespace tikz {

TikzLine::TikzLine(Eigen::Vector3d startPoint, Eigen::Vector3d endPoint, TikzElementConfiguration config)
	: startPoint(startPoint)
	, endPoint(endPoint)
	, config(config)
	, coordinateSystem() {
}

TikzLine::TikzLine(const TikzLine& other)
	: startPoint(other.startPoint)
	, endPoint(other.endPoint)
	, config(other.config)
	, coordinateSystem(other.coordinateSystem) {
}

TikzLine::~TikzLine() {
}

std::string TikzLine::getCode() const {
	return "\\draw[" + config.getCode() + "] " + coordCode(startPoint, coordinateSystem) + " -- " + coordCode(endPoint, coordinateSystem) + ";\n";
}

std::shared_ptr<TikzElement> TikzLine::clone() const {
	return std::make_shared<TikzLine>(*this);
}

void TikzLine::setCoordinateSystem(const std::shared_ptr<sfcpp::geo::CoordinateSystem>& coordinateSystem) {
	this->coordinateSystem = coordinateSystem; // TODO: clone it? clone it in copy constructor?
}

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */
