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



#ifndef SRC_LATEX_TIKZ_TIKZELEMENTCONFIGURATION_HPP_
#define SRC_LATEX_TIKZ_TIKZELEMENTCONFIGURATION_HPP_

#include <img/Color.hpp>

#include <string>

namespace sfcpp {
namespace latex {
namespace tikz {

static constexpr double ptToCm = 0.0352778;

struct TikzThickness {
  static constexpr double UltraThin = 0.1 * ptToCm;
  static constexpr double VeryThin = 0.2 * ptToCm;
  static constexpr double Thin = 0.4 * ptToCm;
  static constexpr double Semithick = 0.6 * ptToCm;
  static constexpr double Thick = 0.8 * ptToCm;
  static constexpr double VeryThick = 1.2 * ptToCm;
  static constexpr double UltraThick = 1.6 * ptToCm;
};

// latex font sizes for a 12pt document
struct TikzFontsize {
  static constexpr double Tiny = 6 * ptToCm;
  static constexpr double Scriptsize = 8 * ptToCm;
  static constexpr double Footnotesize = 10 * ptToCm;
  static constexpr double Small = 10.95 * ptToCm;
  static constexpr double Normalsize = 12 * ptToCm;
  static constexpr double large = 14.4 * ptToCm;
  static constexpr double Large = 17.28 * ptToCm;
  static constexpr double LARGE = 20.74 * ptToCm;
  static constexpr double huge = 24.88 * ptToCm;
  static constexpr double Huge = 24.88 * ptToCm;
};

enum class TikzLineCap { Butt, Rect, Round };

enum class TikzLineType { Solid, Dashed, Dotted, Dashdotted, DenselyDotted, LooselyDotted, Double };

enum class TikzArrowType { To, Latex, Stealth };

class TikzElementConfiguration {
 public:
  TikzElementConfiguration(img::Color drawColor = img::Color::Black,
                           img::Color fillColor = img::Color::TransparentWhite)
      : drawColor(drawColor), fillColor(fillColor) {}

  static TikzElementConfiguration arrowConfiguration(
      img::Color drawColor = img::Color::Black,
      img::Color fillColor = img::Color::TransparentWhite);
  static TikzElementConfiguration labelConfiguration(double fontsizeCm = TikzFontsize::Normalsize,
                                                     bool scaleFont = false);

  // state variables
  bool forwardArrow = false;
  bool backwardArrow = false;

  bool drawBorder = true;

  img::Color drawColor = img::Color::Black;
  img::Color fillColor = img::Color::TransparentWhite;

  double thicknessCm = TikzThickness::Thin;

  TikzLineType lineType = TikzLineType::Solid;
  TikzLineCap lineCap = TikzLineCap::Round;

  bool scaleFont = false;
  double fontsizeCm = TikzFontsize::Normalsize;
  double baselineSkipCm = TikzFontsize::Normalsize * 1.2;

  TikzArrowType arrowType = TikzArrowType::Latex;

  // code generation
  static std::string colorCode(img::Color c);

  std::string getArrowCode() const;
  std::string getBorderCode() const;
  std::string getColorCode() const;
  std::string getThicknessCode() const;
  std::string getLineTypeCode() const;
  std::string getLineCapCode() const;
  std::string getFontCode(double fontScalingFactor = 1.0) const;
  std::string getCode(double fontScalingFactor = 1.0) const;
};

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */

#endif /* SRC_LATEX_TIKZ_TIKZELEMENTCONFIGURATION_HPP_ */
