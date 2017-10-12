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



#ifndef SRC_GEO_GEO_HPP_
#define SRC_GEO_GEO_HPP_

#include <Eigen/Dense>

namespace sfcpp {
namespace geo {

Eigen::Matrix4d linearTransform(double u1, double v1, double w1, double u2, double v2, double w2, double u3, double v3, double w3);
Eigen::Matrix4d linearTransform(Eigen::Vector3d u, Eigen::Vector3d v, Eigen::Vector3d w);
Eigen::Matrix4d linearTransform2D(double u1, double v1, double u2, double v2);

Eigen::Matrix4d zRotationTransform(double theta);
Eigen::Matrix4d rotationTransform(double theta, Eigen::Vector3d axis);

Eigen::Matrix4d translationTransform(Eigen::Vector3d v);
Eigen::Matrix4d translationTransform(double tx, double ty, double tz);

Eigen::Matrix4d scaleTransform(double sx, double sy, double sz);

extern Eigen::Matrix4d swapXYTransform;

} /* namespace geo */
} /* namespace utils */

#endif /* SRC_GEO_GEO_HPP_ */
