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



#ifndef UTILS_IMG_COLOR32_HPP_
#define UTILS_IMG_COLOR32_HPP_

#include <cstdint>

namespace sfcpp {
namespace img {

class Color;

class Color32 {
	uint8_t r, g, b, a;

public:
	Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: r(r), g(g), b(b), a(a) {

	}

	Color toColor() const;

	uint8_t getA() const {
		return a;
	}

	void setA(uint8_t a) {
		this->a = a;
	}

	uint8_t getB() const {
		return b;
	}

	void setB(uint8_t b) {
		this->b = b;
	}

	uint8_t getG() const {
		return g;
	}

	void setG(uint8_t g) {
		this->g = g;
	}

	uint8_t getR() const {
		return r;
	}

	void setR(uint8_t r) {
		this->r = r;
	}
};

}
}

#endif /* UTILS_IMG_COLOR32_HPP_ */
