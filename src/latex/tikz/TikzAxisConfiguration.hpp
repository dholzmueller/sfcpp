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

#include <string>

namespace sfcpp {
namespace latex {
namespace tikz {

struct TikzAxisConfiguration {
  std::string xlabel;
  std::string ylabel;
  std::string xmin, ymin, xmax, ymax;
  std::string legendPos;
  std::string width;
  std::string height;
  std::string additionalOptions;

  std::string getCode() const;
};

} /* namespace tikz */
} /* namespace latex */
} /* namespace sfcpp */
