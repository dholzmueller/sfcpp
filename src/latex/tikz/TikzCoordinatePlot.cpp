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



#include "TikzCoordinatePlot.hpp"

#include <strings/strings.hpp>

namespace sfcpp {
namespace latex {
namespace tikz {

TikzCoordinatePlot::TikzCoordinatePlot(Eigen::MatrixXd coordinates, std::string legend)
    : coordinates(coordinates), legend(legend) {}

TikzCoordinatePlot::TikzCoordinatePlot(const TikzCoordinatePlot& other)
    : coordinates(other.coordinates), legend(other.legend) {}

std::string TikzCoordinatePlot::getCode() const {
  std::string result = "\\addplot ";
  result += "coordinates {\n";
  for (int i = 0; i < coordinates.rows(); ++i) {
    result += "(" + strings::toStringFixedPrecision(coordinates(i, 0), 5) + ", " +
              strings::toStringFixedPrecision(coordinates(i, 1), 5) + ")\n";
  }
  result += "};\n";
  if (legend.length() > 0) {
    result += "\\addlegendentry{" + legend + "}\n";
  }
  return result;
}

std::shared_ptr<TikzElement> TikzCoordinatePlot::clone() const {
  return std::make_shared<TikzCoordinatePlot>(*this);
}

void TikzCoordinatePlot::setCoordinateSystem(const std::shared_ptr<sfcpp::geo::CoordinateSystem>&) {
  // not handled here
}

} /* namespace tikz */
} /* namespace latex */
} /* namespace sfcpp */
