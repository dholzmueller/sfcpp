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



#include <latex/tikz/TikzElementConfiguration.hpp>
#include <strings/strings.hpp>

using namespace sfcpp::strings;

namespace sfcpp {
namespace latex {
namespace tikz {

TikzElementConfiguration TikzElementConfiguration::labelConfiguration(double fontsizeCm,
                                                                      bool scaleFont) {
  TikzElementConfiguration config;
  config.drawBorder = false;
  config.drawColor = img::Color::TransparentWhite;
  config.fillColor = img::Color::TransparentWhite;
  config.scaleFont = scaleFont;
  config.fontsizeCm = fontsizeCm;
  config.baselineSkipCm = fontsizeCm * 1.2;
  return config;
}

TikzElementConfiguration TikzElementConfiguration::arrowConfiguration(img::Color drawColor,
                                                                      img::Color fillColor) {
  TikzElementConfiguration config(drawColor, fillColor);
  config.forwardArrow = true;
  return config;
}

std::string TikzElementConfiguration::colorCode(img::Color c) {
  return "{rgb,1.0:red," + toStringFixedPrecision(c.getR(), 5) + ";green," +
         toStringFixedPrecision(c.getG(), 5) + ";blue," + toStringFixedPrecision(c.getB(), 5) + "}";
}

std::string TikzElementConfiguration::getArrowCode() const {
  std::string code = backwardArrow ? "<-" : "-";
  if (forwardArrow) {
    code += ">";
  }

  if (arrowType == TikzArrowType::To) {
    return code + ", >=to";
  } else if (arrowType == TikzArrowType::Stealth) {
    return code + ", >=stealth";
  } else {
    return code + ", >=latex";
  }
}

std::string TikzElementConfiguration::getBorderCode() const {
  return drawBorder ? "draw" : "draw = none";
}

std::string TikzElementConfiguration::getColorCode() const {
  std::string result = "draw = " + colorCode(drawColor) + ", draw opacity = " +
                       toStringFixedPrecision(drawColor.getA(), 5);
  if (fillColor.getA() >= 0.0001) {
    result += ", fill = " + colorCode(fillColor) + ", fill opacity = " +
              toStringFixedPrecision(fillColor.getA(), 5);
  }
  return result;
}

// TODO: evtl. Pfeil-Stil anpassen

std::string TikzElementConfiguration::getThicknessCode() const {
  return "line width = " + toString(thicknessCm) + "cm";
}

std::string TikzElementConfiguration::getLineTypeCode() const {
  if (lineType == TikzLineType::Solid) {
    return "solid";
  } else if (lineType == TikzLineType::Dashdotted) {
    return "dashdotted";
  } else if (lineType == TikzLineType::Dashed) {
    return "dashed";
  } else if (lineType == TikzLineType::DenselyDotted) {
    return "densely dotted";
  } else if (lineType == TikzLineType::Dotted) {
    return "dotted";
  } else if (lineType == TikzLineType::LooselyDotted) {
    return "loosely dotted";
  } else if (lineType == TikzLineType::Double) {
    return "double";
  }

  return "";
}

std::string sfcpp::latex::tikz::TikzElementConfiguration::getLineCapCode() const {
  if (lineCap == TikzLineCap::Round) {
    return "line cap = round";
  } else if (lineCap == TikzLineCap::Butt) {
    return "line cap = butt";
  } else if (lineCap == TikzLineCap::Rect) {
    return "line cap = rect";
  }

  return "";
}

std::string TikzElementConfiguration::getFontCode(double fontScalingFactor) const {
  double factor = scaleFont ? fontScalingFactor : 1.0;
  return "font = \\fontsize{" + toStringFixedPrecision(factor * fontsizeCm, 5) + "cm}{" +
         toString(factor * baselineSkipCm) + "cm}\\selectfont";
}

std::string TikzElementConfiguration::getCode(double fontScalingFactor) const {
  return getArrowCode() + ", " + getBorderCode() + ", " + getColorCode() + ", " +
         getThicknessCode() + ", " + getLineTypeCode() + ", " + getLineCapCode() + ", " +
         getFontCode(fontScalingFactor);
}

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */
