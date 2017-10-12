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



#include "CurveSpecification.hpp"
#include <sfc/KDCurveSpecification.hpp>
#include <strings/strings.hpp>

#include <stdexcept>

namespace sfcpp {
namespace sfc {

std::shared_ptr<CurveSpecification> CurveSpecification::getSierpinskiCurveSpecification(size_t d) {
  if (d < 2) {
    throw std::runtime_error("CurveSpecification::getSierpinskiCurveSpecification(): d < 2");
  }

  auto spec = std::make_shared<CurveSpecification>();
  spec->d = d;
  spec->rootPoints = Eigen::MatrixXd::Zero(d, d + 1);
  spec->rootPoints.block(0, 0, d, d) = Eigen::MatrixXd::Identity(d, d);
  spec->grammar.push_back(std::vector<size_t>(2, 0));  // only one grammar element

  Eigen::MatrixXd firstTransform = Eigen::MatrixXd::Zero(d + 1, d + 1);
  firstTransform(0, d) = 0.5;
  firstTransform(1, d) = 0.5;

  for (size_t dim = d - 1; dim >= 2; --dim) {
    firstTransform(dim + 1, dim) = 1.0;
  }

  Eigen::MatrixXd secondTransform = firstTransform;

  firstTransform(0, 0) = 1.0;
  firstTransform(2, 1) = 1.0;
  secondTransform(2, 0) = 1.0;
  secondTransform(1, 1) = 1.0;

  spec->transitionMats.push_back(std::vector<Eigen::MatrixXd>{firstTransform, secondTransform});

  return spec;
}

std::shared_ptr<CurveSpecification> CurveSpecification::getCustomCurveSpecification1() {
  auto spec = std::make_shared<CurveSpecification>();
  spec->d = 2;
  spec->rootPoints = Eigen::MatrixXd::Zero(2, 3);
  spec->rootPoints << 0, 1, 0.5,  //
      /*           */ 0, 0, (0.5 * sqrt(3));
  spec->grammar.push_back(std::vector<size_t>(3, 0));  // only one grammar element

  Eigen::MatrixXd m1(3, 3);
  Eigen::MatrixXd m2(3, 3);
  Eigen::MatrixXd m3(3, 3);
  double c = 1.0 / 3.0;
  m1 << 0, 1, c,  //
      1, 0, c,    //
      0, 0, c;
  m2 << c, 0, 0,  //
      c, 1, 0,    //
      c, 0, 1;
  m3 << 0, c, 1,  //
      0, c, 0,    //
      1, c, 0;

  spec->transitionMats.push_back(std::vector<Eigen::MatrixXd>{m1, m2, m3});

  return spec;
}

std::shared_ptr<CurveSpecification> sfcpp::sfc::CurveSpecification::getGosperCurveSpecification() {
  auto spec = std::make_shared<CurveSpecification>();
  spec->d = 2;
  spec->grammar = std::vector<std::vector<size_t>>{{0, 1, 1, 0, 0, 0, 1}, {0, 1, 1, 1, 0, 0, 1}};

  // midpoint of the root hexagon is (0, 0), x and y values of points are given
  // by dx and dy in
  // different combinations
  double sidelength = 0.5;
  double dx = sidelength / 2;
  double dy = sqrt(3) * dx;
  Eigen::MatrixXd rootPoints(2, 6);
  rootPoints << -dx, dx, sidelength, dx, -dx, -sidelength, -dy, -dy, 0, dy, dy, 0;
  spec->rootPoints = rootPoints;

  Eigen::MatrixXd transposedInnerToOuter(6, 6);  // not uniquely determined
  transposedInnerToOuter << 0, 0, 0, -1, 0, 2,   // first combination
      2, 0, 0, 0, -1, 0,                         // second combination
      0, 2, 0, 0, 0, -1,                         // third combination
      -1, 0, 2, 0, 0, 0,                         // fourth combination
      0, -1, 0, 2, 0, 0,                         // fifth combination
      0, 0, -1, 0, 2, 0;                         // sixth combination

  Eigen::MatrixXd outerToInner = transposedInnerToOuter.transpose().inverse();

  // affine combination coefficient vectors of points in inner hexagon
  std::vector<Eigen::VectorXd> c;
  for (size_t i = 0; i < 6; ++i) {
    c.push_back(outerToInner.col(i));
  }

  // affine combination coefficient vectors of points in refined hexagonal grid
  // also not unique
  std::vector<Eigen::VectorXd> p(24);
  p[0] = c[5] + c[0] - c[3];
  p[1] = c[1] + c[0] - c[3];
  p[2] = 2 * c[5] - c[3];
  p[3] = c[0] + c[1] - c[2];
  p[4] = 2 * c[1] - c[3];
  p[5] = c[5] + c[4] - c[3];
  p[6] = c[0];
  p[7] = c[1] + c[2] - c[3];
  p[8] = 2 * c[5] - c[1];
  p[9] = c[5];
  p[10] = c[1];
  p[11] = 2 * c[1] - c[5];
  p[12] = 2 * c[4] - c[2];
  p[13] = c[4];
  p[14] = c[2];
  p[15] = 2 * c[2] - c[4];
  p[16] = c[3] + c[5] - c[1];
  p[17] = c[3];
  p[18] = c[3] + c[1] - c[5];
  p[19] = c[3] + c[4] - c[1];
  p[20] = c[3] + c[4] - c[5];
  p[21] = c[3] + c[2] - c[5];
  p[22] = 2 * c[3] - c[1];
  p[23] = 2 * c[3] - c[5];

  auto v = std::vector<std::vector<Eigen::MatrixXd>>(
      2, std::vector<Eigen::MatrixXd>(7, Eigen::MatrixXd(6, 6)));

  // first grammar element

  v[0][0].col(0) = p[2];
  v[0][0].col(1) = p[0];
  v[0][0].col(2) = p[3];
  v[0][0].col(3) = p[6];
  v[0][0].col(4) = p[9];
  v[0][0].col(5) = p[5];

  v[0][1].col(0) = p[3];
  v[0][1].col(1) = p[1];
  v[0][1].col(2) = p[4];
  v[0][1].col(3) = p[7];
  v[0][1].col(4) = p[10];
  v[0][1].col(5) = p[6];

  v[0][2].col(0) = p[10];
  v[0][2].col(1) = p[14];
  v[0][2].col(2) = p[17];
  v[0][2].col(3) = p[13];
  v[0][2].col(4) = p[9];
  v[0][2].col(5) = p[6];

  v[0][3].col(0) = p[9];
  v[0][3].col(1) = p[13];
  v[0][3].col(2) = p[16];
  v[0][3].col(3) = p[12];
  v[0][3].col(4) = p[8];
  v[0][3].col(5) = p[5];

  v[0][4].col(0) = p[16];
  v[0][4].col(1) = p[13];
  v[0][4].col(2) = p[17];
  v[0][4].col(3) = p[20];
  v[0][4].col(4) = p[22];
  v[0][4].col(5) = p[19];

  v[0][5].col(0) = p[17];
  v[0][5].col(1) = p[14];
  v[0][5].col(2) = p[18];
  v[0][5].col(3) = p[21];
  v[0][5].col(4) = p[23];
  v[0][5].col(5) = p[20];

  v[0][6].col(0) = p[18];
  v[0][6].col(1) = p[14];
  v[0][6].col(2) = p[10];
  v[0][6].col(3) = p[7];
  v[0][6].col(4) = p[11];
  v[0][6].col(5) = p[15];

  // second grammar element

  v[1][0].col(0) = p[2];
  v[1][0].col(1) = p[0];
  v[1][0].col(2) = p[3];
  v[1][0].col(3) = p[6];
  v[1][0].col(4) = p[9];
  v[1][0].col(5) = p[5];

  v[1][1].col(0) = p[3];
  v[1][1].col(1) = p[1];
  v[1][1].col(2) = p[4];
  v[1][1].col(3) = p[7];
  v[1][1].col(4) = p[10];
  v[1][1].col(5) = p[6];

  v[1][2].col(0) = p[10];
  v[1][2].col(1) = p[7];
  v[1][2].col(2) = p[11];
  v[1][2].col(3) = p[15];
  v[1][2].col(4) = p[18];
  v[1][2].col(5) = p[14];

  v[1][3].col(0) = p[18];
  v[1][3].col(1) = p[21];
  v[1][3].col(2) = p[23];
  v[1][3].col(3) = p[20];
  v[1][3].col(4) = p[17];
  v[1][3].col(5) = p[14];

  v[1][4].col(0) = p[17];
  v[1][4].col(1) = p[13];
  v[1][4].col(2) = p[9];
  v[1][4].col(3) = p[6];
  v[1][4].col(4) = p[10];
  v[1][4].col(5) = p[14];

  v[1][5].col(0) = p[9];
  v[1][5].col(1) = p[13];
  v[1][5].col(2) = p[16];
  v[1][5].col(3) = p[12];
  v[1][5].col(4) = p[8];
  v[1][5].col(5) = p[5];

  v[1][6].col(0) = p[16];
  v[1][6].col(1) = p[13];
  v[1][6].col(2) = p[17];
  v[1][6].col(3) = p[20];
  v[1][6].col(4) = p[22];
  v[1][6].col(5) = p[19];

  spec->transitionMats = v;

  return spec;
}

std::shared_ptr<CurveSpecification> CurveSpecification::getBetaOmegaCurveSpecification() {
  auto spec = std::make_shared<CurveSpecification>();
  spec->d = 2;
  size_t numGrammarElements = 7;
  spec->grammar =
      std::vector<std::vector<size_t>>{{1, 3, 1, 3}, {2, 1, 3, 5}, {1, 2, 4, 6}, {5, 1, 3, 4},
                                       {6, 2, 4, 3}, {2, 1, 3, 4}, {1, 2, 4, 3}};
  spec->rootPoints = KDCurveSpecification::generateUnitCube(2);
  spec->transitionMats = std::vector<std::vector<Eigen::MatrixXd>>(
      numGrammarElements, std::vector<Eigen::MatrixXd>(4, Eigen::MatrixXd::Zero(4, 4)));
  std::vector<Eigen::VectorXd> p(9, Eigen::VectorXd(4));
  p[0] << 1, 0, 0, 0;
  p[1] << 0.5, 0.5, 0, 0;
  p[2] << 0, 1, 0, 0;
  p[3] << 0.5, 0, 0.5, 0;
  p[4] << 0.5, 0, 0, 0.5;
  p[5] << 0, 0.5, 0, 0.5;
  p[6] << 0, 0, 1, 0;
  p[7] << 0, 0, 0.5, 0.5;
  p[8] << 0, 0, 0, 1;

  auto &v = spec->transitionMats;

  v[0][0].col(0) = p[4];
  v[0][0].col(1) = p[3];
  v[0][0].col(2) = p[1];
  v[0][0].col(3) = p[0];

  v[0][1].col(0) = p[4];
  v[0][1].col(1) = p[7];
  v[0][1].col(2) = p[3];
  v[0][1].col(3) = p[6];

  v[0][2].col(0) = p[4];
  v[0][2].col(1) = p[5];
  v[0][2].col(2) = p[7];
  v[0][2].col(3) = p[8];

  v[0][3].col(0) = p[4];
  v[0][3].col(1) = p[1];
  v[0][3].col(2) = p[5];
  v[0][3].col(3) = p[2];

  v[1][0].col(0) = p[0];
  v[1][0].col(1) = p[1];
  v[1][0].col(2) = p[3];
  v[1][0].col(3) = p[4];

  v[1][1].col(0) = p[4];
  v[1][1].col(1) = p[7];
  v[1][1].col(2) = p[3];
  v[1][1].col(3) = p[6];

  v[1][2].col(0) = p[4];
  v[1][2].col(1) = p[5];
  v[1][2].col(2) = p[7];
  v[1][2].col(3) = p[8];

  v[1][3].col(0) = p[4];
  v[1][3].col(1) = p[1];
  v[1][3].col(2) = p[5];
  v[1][3].col(3) = p[2];

  v[2][0].col(0) = p[3];
  v[2][0].col(1) = p[4];
  v[2][0].col(2) = p[6];
  v[2][0].col(3) = p[7];

  v[2][1].col(0) = p[3];
  v[2][1].col(1) = p[0];
  v[2][1].col(2) = p[4];
  v[2][1].col(3) = p[1];

  v[2][2].col(0) = p[1];
  v[2][2].col(1) = p[2];
  v[2][2].col(2) = p[4];
  v[2][2].col(3) = p[5];

  v[2][3].col(0) = p[5];
  v[2][3].col(1) = p[8];
  v[2][3].col(2) = p[4];
  v[2][3].col(3) = p[7];

  v[3][0].col(0) = p[3];
  v[3][0].col(1) = p[4];
  v[3][0].col(2) = p[6];
  v[3][0].col(3) = p[7];

  v[3][1].col(0) = p[4];
  v[3][1].col(1) = p[5];
  v[3][1].col(2) = p[7];
  v[3][1].col(3) = p[8];

  v[3][2].col(0) = p[4];
  v[3][2].col(1) = p[1];
  v[3][2].col(2) = p[5];
  v[3][2].col(3) = p[2];

  v[3][3].col(0) = p[4];
  v[3][3].col(1) = p[3];
  v[3][3].col(2) = p[1];
  v[3][3].col(3) = p[0];

  v[4][0].col(0) = p[0];
  v[4][0].col(1) = p[1];
  v[4][0].col(2) = p[3];
  v[4][0].col(3) = p[4];

  v[4][1].col(0) = p[1];
  v[4][1].col(1) = p[2];
  v[4][1].col(2) = p[4];
  v[4][1].col(3) = p[5];

  v[4][2].col(0) = p[5];
  v[4][2].col(1) = p[8];
  v[4][2].col(2) = p[4];
  v[4][2].col(3) = p[7];

  v[4][3].col(0) = p[7];
  v[4][3].col(1) = p[6];
  v[4][3].col(2) = p[4];
  v[4][3].col(3) = p[3];

  v[5][0].col(0) = p[0];
  v[5][0].col(1) = p[1];
  v[5][0].col(2) = p[3];
  v[5][0].col(3) = p[4];

  v[5][1].col(0) = p[4];
  v[5][1].col(1) = p[7];
  v[5][1].col(2) = p[3];
  v[5][1].col(3) = p[6];

  v[5][2].col(0) = p[4];
  v[5][2].col(1) = p[5];
  v[5][2].col(2) = p[7];
  v[5][2].col(3) = p[8];

  v[5][3].col(0) = p[4];
  v[5][3].col(1) = p[1];
  v[5][3].col(2) = p[5];
  v[5][3].col(3) = p[2];

  v[6][0].col(0) = p[3];
  v[6][0].col(1) = p[4];
  v[6][0].col(2) = p[6];
  v[6][0].col(3) = p[7];

  v[6][1].col(0) = p[3];
  v[6][1].col(1) = p[0];
  v[6][1].col(2) = p[4];
  v[6][1].col(3) = p[1];

  v[6][2].col(0) = p[1];
  v[6][2].col(1) = p[2];
  v[6][2].col(2) = p[4];
  v[6][2].col(3) = p[5];

  v[6][3].col(0) = p[5];
  v[6][3].col(1) = p[8];
  v[6][3].col(2) = p[4];
  v[6][3].col(3) = p[7];

  return spec;
}

std::ostream &operator<<(std::ostream &stream, CurveSpecification const &spec) {
  stream << "Grammar:\n" << strings::toString(spec.grammar, "\n", ", ") << "\n\n";
  // stream << "Ordering:\n" << strings::toString(spec.childOrdering, "\n", ",
  // ") << "\n\n";
  stream << "Root points:\n" << spec.rootPoints << "\n\n";
  stream << "Transformation matrices:\n"
         << strings::toString(spec.transitionMats, "\n\n-----\n\n", "\n\n") << "\n\n";

  return stream;
}

} /* namespace sfc */
} /* namespace sfcpp */
