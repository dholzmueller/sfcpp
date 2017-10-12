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



#include "ConvexPolytope.hpp"
#include "QuickHullAlgorithm.hpp"

#include <strings/strings.hpp>

#include <iostream>

namespace sfcpp {
namespace geo {

ConvexPolytope::ConvexPolytope(size_t d) : faces(d + 1) {}

std::shared_ptr<ConvexPolytope> ConvexPolytope::convexHull(Eigen::MatrixXd points) {
  auto hull = QuickHullAlgorithm(points).createPolytope();
  hull->sort();
  return hull;
}

std::ostream &operator<<(std::ostream &stream, ConvexPolytope::Face const &face) {
  stream << face.vertices;
  //<< "/" << strings::toString(face.childIndexes, ",") << "/"
  //<< strings::toString(face.parentIndexes, ",");
  return stream;
}

bool ConvexPolytope::tryFindFace(size_t dim, const math::NatSet &vertexSet,
                                 size_t &faceIndex) const {
  for (size_t i = 0; i < faces[dim].size(); ++i) {
    auto &face = faces[dim][i];
    if (face.vertices == vertexSet) {
      faceIndex = i;
      return true;
    }
  }

  return false;
}

std::ostream &operator<<(std::ostream &stream, ConvexPolytope const &polytope) {
  stream << "[" << strings::toString(polytope.faces, "]\n[", "; ") << "]";
  return stream;
}

void ConvexPolytope::sort() {
  size_t bound = faces[0].size() - 1;
  for (size_t dim = 0; dim < faces.size(); ++dim) {
    // sort faces in dimension dim
    std::sort(faces[dim].begin(), faces[dim].end(), [&](Face const &first, Face const &second) {
      auto f1 = first.vertices;
      auto f2 = f1.reverseAt(bound);
      auto s1 = second.vertices;
      auto s2 = s1.reverseAt(bound);

      auto f = f1.compare(f2) < 0 ? f1 : f2;
      auto s = s1.compare(s2) < 0 ? s1 : s2;
      auto result = f.compare(s);
      if (result == 0) {
        // faces are opposite to each other
        return f1.compare(s1) < 0;
      }
      return result < 0;
    });

    if (dim >= 1) {
      // renew parents of children
      for (auto &child : faces[dim - 1]) {
        child.parentIndexes.clear();
      }
    }

    if (dim + 1 < faces.size()) {
      for (auto &parent : faces[dim + 1]) {
        parent.childIndexes.clear();
      }
    }

    for (size_t i = 0; i < faces[dim].size(); ++i) {
      auto &elem = faces[dim][i];
      for (auto &pIndex : elem.parentIndexes) {
        faces[dim + 1][pIndex].childIndexes.push_back(i);
      }
      for (auto &cIndex : elem.childIndexes) {
        faces[dim - 1][cIndex].parentIndexes.push_back(i);
      }
    }
  }
}

size_t ConvexPolytope::getDimension() { return faces.size() - 1; }

} /* namespace geo */
} /* namespace sfcpp */
