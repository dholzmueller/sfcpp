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



#ifndef SRC_IMG_FUNCTIONAL_HPP_
#define SRC_IMG_FUNCTIONAL_HPP_

#include <functional>
#include <limits>

namespace sfcpp {
namespace img {

template<typename T> std::function<bool(T)> equals(T value) {
	return [=](T other) -> bool {return other == value;};
}

template<typename T> T invert(T value) {
	return std::numeric_limits<T>::max() + std::numeric_limits<T>::min() - value;
}

bool lnot(bool value);

bool land(bool a, bool b);

bool lor(bool a, bool b);

}
}

#endif /* SRC_IMG_FUNCTIONAL_HPP_ */
