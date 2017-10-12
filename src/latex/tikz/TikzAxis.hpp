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



#pragma once

#include <latex/tikz/TikzAxisConfiguration.hpp>
#include <latex/tikz/TikzElement.hpp>
#include <latex/tikz/TikzElementContainer.hpp>

namespace sfcpp {
namespace latex {
namespace tikz {

class TikzAxis : public TikzElement {
  TikzAxisConfiguration config;
  TikzElementContainer container;

 public:
  TikzAxis(TikzAxisConfiguration const &config);
  TikzAxis(TikzAxis const &other);

  void addElement(std::shared_ptr<TikzElement> element);

  virtual std::string getCode() const;
  virtual std::shared_ptr<TikzElement> clone() const;
  virtual void setCoordinateSystem(
      std::shared_ptr<sfcpp::geo::CoordinateSystem> const &coordinateSystem);
};

} /* namespace tikz */
} /* namespace latex */
} /* namespace sfcpp */
