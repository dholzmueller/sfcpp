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



#include "Color.hpp"
#include "Color32.hpp"
#include <math/math.hpp>

using namespace sfcpp::math;

namespace sfcpp {
namespace img {

struct RGBColor {
	RGBColor(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) {}
	RGBColor() : r(0.0), g(0.0), b(0.0), a(1.0) {}

    double r;
    double g;
    double b;
    double a;
};

HSVColor::HSVColor(double h, double s, double v, double a) :
			h(h), s(s), v(v), a(a) {
}

HSVColor& HSVColor::operator *=(double factor) {
	h *= factor;
	s *= factor;
	v *= factor;
	a *= factor;

	return *this;
}

HSVColor& HSVColor::operator +=(HSVColor const &other) {
	h += other.h;
	s += other.s;
	v += other.v;
	a += other.a;

	return *this;
}

HSVColor operator*(double factor, HSVColor const &c) {
	HSVColor result = c;
	result *= factor;
	return result;
}

HSVColor operator+(HSVColor const &first, HSVColor const &second) {
	HSVColor result = first;
	result += second;
	return result;
}

HSVColor HSVColor::White(0,0,1);
HSVColor HSVColor::Black(0,0,0);
HSVColor HSVColor::Red(0,1,1);
HSVColor HSVColor::Green(120,1,1);
HSVColor HSVColor::Blue(240,1,1);
HSVColor HSVColor::Cyan(180,1,1);
HSVColor HSVColor::Magenta(300,1,1);
HSVColor HSVColor::Yellow(60,1,1);

static HSVColor   rgb2hsv(RGBColor in);
static RGBColor   hsv2rgb(HSVColor in);

void Color::cutoff() {
	r = bound(r, 0.0, 1.0);
	g = bound(g, 0.0, 1.0);
	b = bound(b, 0.0, 1.0);
	a = bound(a, 0.0, 1.0);
}

Color32 Color::toColor32() const {
	Color c = *this; // TODO: could be made more efficient
	c.cutoff();
	return Color32(c.r*255 + 0.5, c.g*255 + 0.5, c.b*255 + 0.5, c.a*255 + 0.5);
}

Color& Color::operator *=(double factor) {
	r *= factor;
	g *= factor;
	b *= factor;
	a *= factor;

	return *this;
}

Color& Color::operator +=(const Color& other) {
	r += other.r;
	g += other.g;
	b += other.b;
	a += other.a;

	return *this;
}

Color operator*(double factor, Color const &c) {
	Color result = c;
	result *= factor;
	return result;
}

Color Color::fromHSV(HSVColor hsv) {
	RGBColor c = hsv2rgb(hsv);
	return Color(c.r, c.g, c.b, c.a);
}

Color Color::fromHSV(double h, double s, double v, double a) {
	return fromHSV(HSVColor(h, s, v, a));
}

HSVColor Color::getHSV() const {
	return rgb2hsv(RGBColor(r, g, b, a));
}

Color operator+(Color const &first, Color const &second) {
	Color result = first;
	result += second;
	return result;
}

Color Color::White(1,1,1);
Color Color::Black(0,0,0);
Color Color::Blue(0,0,1);
Color Color::Green(0,1,0);
Color Color::Red(1,0,0);
Color Color::Yellow(1,1,0);
Color Color::Magenta(1,0,1);
Color Color::Cyan(0,1,1);
Color Color::LightBlue(0.5,0.5,1);
Color Color::DarkBlue(0,0,0.5);
Color Color::LightRed(1,0.5,0.5);
Color Color::DarkRed(0.5,0,0);
Color Color::LightGreen(0.5,1,0.5);
Color Color::DarkGreen(0,0.5,0);
Color Color::Gray(0.5,0.5,0.5);
Color Color::DarkGray(0.25,0.25,0.25);
Color Color::LightGray(0.75,0.75,0.75);
Color Color::TransparentWhite(1,1,1,0);
Color Color::Transparent(0,0,0,0);



// Code from http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
// adapted for alpha component

HSVColor rgb2hsv(RGBColor in)
{
    HSVColor         out;
    double      min, max, delta;

    out.a = in.a;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = 0.0;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compiler happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


RGBColor hsv2rgb(HSVColor in)
{
    double      hh, p, q, t, ff;
    long        i;
    RGBColor         out;
    out.a = in.a;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = mod(in.h, 360.0); // adapted because we allow arbitrary angles
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }


    return out;
}

}
}
