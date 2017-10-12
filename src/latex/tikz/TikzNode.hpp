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



#ifndef SRC_LATEX_TIKZ_TIKZNODE_HPP_
#define SRC_LATEX_TIKZ_TIKZNODE_HPP_

#include "TikzElement.hpp"
#include "TikzElementConfiguration.hpp"

namespace sfcpp {
namespace latex {
namespace tikz {

class TikzNode : public TikzElement {
  std::string text;
  Eigen::Vector3d pos;
  TikzElementConfiguration config;
  std::shared_ptr<sfcpp::geo::CoordinateSystem> coordinateSystem;

  double textFactor = 1.0;
  std::string textPrefix = "";
  std::string textPostfix = "";

 public:
  TikzNode(std::string text, Eigen::Vector3d pos,
           TikzElementConfiguration config = TikzElementConfiguration::labelConfiguration());
  TikzNode(TikzNode const &other);
  virtual ~TikzNode();

  virtual std::string getCode() const;
  virtual std::shared_ptr<TikzElement> clone() const;
  virtual void setTextOrientation(Eigen::VectorXd first, Eigen::VectorXd second);
  virtual void setCoordinateSystem(
      std::shared_ptr<sfcpp::geo::CoordinateSystem> const &coordinateSystem);
};

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */

#endif /* SRC_LATEX_TIKZ_TIKZNODE_HPP_ */
