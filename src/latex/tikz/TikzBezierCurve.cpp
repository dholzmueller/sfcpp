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



#include <latex/tikz/TikzBezierCurve.hpp>
#include <latex/tikz/tikz.hpp>

namespace sfcpp {
namespace latex {
namespace tikz {

TikzBezierCurve::TikzBezierCurve(Eigen::Vector3d startPoint, Eigen::Vector3d firstControlPoint, Eigen::Vector3d endPoint, TikzElementConfiguration config) :
		startPoint(startPoint), endPoint(endPoint), firstControlPoint(firstControlPoint), secondControlPoint(), useSecondControlPoint(false), config(
				config) {
}

TikzBezierCurve::TikzBezierCurve(Eigen::Vector3d startPoint, Eigen::Vector3d firstControlPoint, Eigen::Vector3d secondControlPoint, Eigen::Vector3d endPoint,
		TikzElementConfiguration config) :
		startPoint(startPoint), endPoint(endPoint), firstControlPoint(firstControlPoint), secondControlPoint(secondControlPoint), useSecondControlPoint(
				true), config(config) {
}

TikzBezierCurve::~TikzBezierCurve() {
}

std::string TikzBezierCurve::getCode() const {
	std::string result = "\\draw[" + config.getCode() + "] " + coordCode(startPoint, coordinateSystem) + " .. controls " + coordCode(firstControlPoint, coordinateSystem);
	if(useSecondControlPoint) {
		result += " and " + coordCode(secondControlPoint, coordinateSystem);
	}
	return result + " .. " + coordCode(endPoint, coordinateSystem) + ";\n";
}

std::shared_ptr<TikzElement> TikzBezierCurve::clone() const {
	return std::make_shared<TikzBezierCurve>(*this);
}

void TikzBezierCurve::setCoordinateSystem(const std::shared_ptr<sfcpp::geo::CoordinateSystem>& coordinateSystem) {
	this->coordinateSystem = coordinateSystem;
}

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */
