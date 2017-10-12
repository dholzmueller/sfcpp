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



#ifndef SRC_LATEX_CODEGENERATOR_HPP_
#define SRC_LATEX_CODEGENERATOR_HPP_

#include <string>

namespace sfcpp {
namespace latex {

class CodeGenerator {
public:
	virtual ~CodeGenerator();

	virtual std::string getCode() const = 0;
};

} /* namespace latex */
} /* namespace utils */

#endif /* SRC_LATEX_CODEGENERATOR_HPP_ */
