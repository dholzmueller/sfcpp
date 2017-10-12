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



#include <geo/Point3D.hpp>

namespace sfcpp {
namespace geo {

Eigen::Vector4d Point3D::getVec4D() const {
  return Eigen::Vector4d(vec[0], vec[1], vec[2], 1.0);
}

Point3D::Point3D(const Eigen::Vector3d &vec,
                 std::shared_ptr<CoordinateSystem> const &coordinateSystem)
    : vec(vec), coordinateSystem(coordinateSystem) {}

Point3D::Point3D(const Point3D &other)
    : vec(other.vec), coordinateSystem(other.coordinateSystem) {}

Eigen::Vector3d Point3D::getCoordinates(
    std::shared_ptr<CoordinateSystem> const &targetSystem) const {
  Eigen::Vector4d x = coordinateSystem->transform(getVec4D(), targetSystem);
  return Eigen::Vector3d(x[0] / x[3], x[1] / x[3], x[2] / x[3]);
}

Eigen::Vector3d Point3D::getWorldCoordinates() const {
  Eigen::Vector4d x = coordinateSystem->toWorldCoordinates(getVec4D());
  return Eigen::Vector3d(x[0] / x[3], x[1] / x[3], x[2] / x[3]);
}

Point3D &Point3D::operator+=(const Vector3D &vector) {
  Eigen::Vector3d coord = vector.getCoordinates(coordinateSystem);
  vec += coord;
  return *this;
}

Point3D operator+(Point3D const &first, Vector3D const &second) {
  Point3D result = first;
  result += second;
  return result;
}

Vector3D operator-(Point3D const &first, Point3D const &second) {
  // TODO: maybe use world coordinate system
  Eigen::Vector3d firstCoord = first.getCoordinates(first.coordinateSystem);
  Eigen::Vector3d secondCoord = second.getCoordinates(first.coordinateSystem);
  return Vector3D(firstCoord - secondCoord, first.coordinateSystem);
}

} /* namespace geo */
} /* namespace utils */
