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



#include <cmath>
#include <geo/geo.hpp>

namespace sfcpp {
namespace geo {

Eigen::Matrix4d linearTransform(double u1, double v1, double w1, double u2,
                                double v2, double w2, double u3, double v3,
                                double w3) {
  Eigen::Matrix4d result;
  result << u1, v1, w1, 0.0, u2, v2, w2, 0.0, u3, v3, w3, 0.0, 0.0, 0.0, 0.0,
      1.0;
  return result;
}

Eigen::Matrix4d linearTransform(Eigen::Vector3d u, Eigen::Vector3d v,
                                Eigen::Vector3d w) {
  return linearTransform(u[0], v[0], w[0], u[1], v[1], w[1], u[2], v[2], w[2]);
}

Eigen::Matrix4d linearTransform2D(double u1, double v1, double u2, double v2) {
  return linearTransform(u1, v1, 0, u2, v2, 0, 0, 0, 1);
}

Eigen::Matrix4d zRotationTransform(double theta) {
  double cosTheta = cos(theta);
  double sinTheta = sin(theta);
  return linearTransform(cosTheta, -sinTheta, 0.0, sinTheta, cosTheta, 0.0, 0.0,
                         0.0, 1.0);
}

Eigen::Matrix4d rotationTransform(double theta, Eigen::Vector3d axis) {
  axis.normalize();

  double eps = 0.01;

  Eigen::Vector3d first;

  if (axis[0] >= eps || axis[1] >= eps) {
    first = Eigen::Vector3d(-axis[1], axis[0], 0.0).normalized();
  } else {
    first = Eigen::Vector3d(0.0, -axis[2], axis[1]).normalized();
  }

  Eigen::Vector3d second = axis.cross(first);

  Eigen::Matrix4d m = linearTransform(first, second, axis);
  Eigen::Matrix4d rot = zRotationTransform(theta);
  return m * rot * m.transpose();
}

Eigen::Matrix4d translationTransform(Eigen::Vector3d v) {
  Eigen::Matrix4d result;
  result << 1.0, 0.0, 0.0, v[0], 0.0, 1.0, 0.0, v[1], 0.0, 0.0, 1.0, v[2], 0.0,
      0.0, 0.0, 1.0;
  return result;
}

Eigen::Matrix4d translationTransform(double tx, double ty, double tz) {
  return translationTransform(Eigen::Vector3d(tx, ty, tz));
}

Eigen::Matrix4d scaleTransform(double sx, double sy, double sz) {
  return linearTransform(sx, 0, 0, 0, sy, 0, 0, 0, sz);
}

Eigen::Matrix4d swapXYTransform = linearTransform2D(0, 1, 1, 0);

} /* namespace geo */
} /* namespace utils */
