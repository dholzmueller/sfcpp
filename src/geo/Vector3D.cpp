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



#include <geo/Vector3D.hpp>

namespace sfcpp {
namespace geo {

Eigen::Vector4d Vector3D::getVec4D() const {
	return Eigen::Vector4d(vec[0], vec[1], vec[2], 0.0);
}

Vector3D::Vector3D(const Eigen::Vector3d& vec, std::shared_ptr<CoordinateSystem> const &coordinateSystem) :
		vec(vec), coordinateSystem(coordinateSystem) {
}

Vector3D::Vector3D(const Vector3D& other) :
		vec(other.vec), coordinateSystem(other.coordinateSystem) {
}

Eigen::Vector3d Vector3D::getCoordinates(std::shared_ptr<CoordinateSystem> const &targetSystem) const {
	Eigen::Vector4d x = coordinateSystem->transform(getVec4D(), targetSystem);
	return Eigen::Vector3d(x[0], x[1], x[2]);
}

Eigen::Vector3d Vector3D::getWorldCoordinates() const {
	Eigen::Vector4d x = coordinateSystem->toWorldCoordinates(getVec4D());
	return Eigen::Vector3d(x[0], x[1], x[2]);
}

Vector3D& Vector3D::operator +=(const Vector3D& other) {
	auto coord = other.getCoordinates(coordinateSystem);
	vec += coord;
	return *this;
}

Vector3D& Vector3D::operator -=(const Vector3D& other) {
	auto coord = other.getCoordinates(coordinateSystem);
	vec -= coord;
	return *this;
}

Vector3D& Vector3D::operator *=(double factor) {
	vec *= factor;
	return *this;
}

Vector3D operator+(Vector3D const &first, Vector3D const &second) {
	Vector3D result = first;
	result += second;
	return result;
}

Vector3D operator-(Vector3D const &first, Vector3D const &second) {
	Vector3D result = first;
	result -= second;
	return result;
}

Vector3D operator*(double factor, Vector3D const &vec) {
	Vector3D result = vec;
	result *= factor;
	return result;
}

} /* namespace geo */
} /* namespace utils */
