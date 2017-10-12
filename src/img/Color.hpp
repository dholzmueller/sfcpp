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



#ifndef UTILS_IMG_COLOR_HPP_
#define UTILS_IMG_COLOR_HPP_

namespace sfcpp {
namespace img {

struct HSVColor {
	HSVColor(double h, double s, double v, double a = 1.0);
	HSVColor() : h(0.0), s(0.0), v(0.0), a(1.0) {}

	double h;       // angle in degrees
	double s;       // percent
	double v;       // percent
	double a;

	HSVColor &operator*=(double factor);
	HSVColor &operator+=(HSVColor const &other);

	friend HSVColor operator*(double factor, HSVColor const &c);
	friend HSVColor operator+(HSVColor const &first, HSVColor const &second);

	static HSVColor White, Black, Red, Green, Blue, Cyan, Magenta, Yellow;
};

class Color32;

class Color {
	double r, g, b, a;

public:
	Color(double r, double g, double b, double a = 1.0) :
			r(r), g(g), b(b), a(a) {

	}

	static Color fromHSV(HSVColor hsv);
	static Color fromHSV(double h, double s, double v, double a = 1.0);

	/**
	 * Bounds the values of the components to the range [0, 1].
	 */
	void cutoff();

	Color32 toColor32() const;

	HSVColor getHSV() const;

	// getters and setters

	double getR() const {
		return r;
	}

	void setR(double r) {
		this->r = r;
	}

	double getA() const {
		return a;
	}

	void setA(double a) {
		this->a = a;
	}

	double getB() const {
		return b;
	}

	void setB(double b) {
		this->b = b;
	}

	double getG() const {
		return g;
	}

	void setG(double g) {
		this->g = g;
	}

	Color &operator*=(double factor);
	Color &operator+=(Color const &other);

	friend Color operator*(double factor, Color const &c);
	friend Color operator+(Color const &first, Color const &second);

	// TODO: maybe blending operations

	static Color White, Black, Blue, Green, Red, Yellow, Magenta, Cyan, LightBlue, DarkBlue, LightRed, DarkRed, LightGreen, DarkGreen, Gray, DarkGray,
			LightGray, TransparentWhite, Transparent;
};

}
}

#endif /* UTILS_IMG_COLOR_HPP_ */
