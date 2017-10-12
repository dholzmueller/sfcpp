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



#ifndef SRC_GEO_VECTOR3D_HPP_
#define SRC_GEO_VECTOR3D_HPP_

#include <geo/CoordinateSystem.hpp>
#include <Eigen/Dense>
#include <memory>

namespace sfcpp {
namespace geo {

class Vector3D {
	Eigen::Vector3d vec;
	std::shared_ptr<CoordinateSystem> coordinateSystem;

	Eigen::Vector4d getVec4D() const;
public:
	Vector3D(Eigen::Vector3d const &vec, std::shared_ptr<CoordinateSystem> const &coordinateSystem);
	Vector3D(Vector3D const &other);

	Eigen::Vector3d getCoordinates(std::shared_ptr<CoordinateSystem> const &targetSystem) const;
	Eigen::Vector3d getWorldCoordinates() const;

	Vector3D &operator+=(Vector3D const &other);
	Vector3D &operator-=(Vector3D const &other);
	Vector3D &operator*=(double factor);

	friend Vector3D operator+(Vector3D const &first, Vector3D const &second);
	friend Vector3D operator-(Vector3D const &first, Vector3D const &second);
	friend Vector3D operator*(double factor, Vector3D const &vec);
};

} /* namespace geo */
} /* namespace utils */

#endif /* SRC_GEO_VECTOR3D_HPP_ */
