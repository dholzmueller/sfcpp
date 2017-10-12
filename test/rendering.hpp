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

#include <img/Color.hpp>
#include <string>

namespace sfcpp {
namespace test {

// static const img::HSVColor startColor = img::HSVColor::Green;
static const img::HSVColor startColor = img::HSVColor(120, 1, 0.5);
static const img::HSVColor endColor = img::HSVColor(360, 1, 1);

static const std::string hilbert2DSymbols[] = {"$H$", "$A$", "$B$", "$R$"};
static const std::string peano2DSymbols[] = {"$P$", "$Q$", "$R$", "$S$"};
static const std::string hilbert2DLocalSymbol = "$G$";
static const std::string peano2DLocalSymbol = "$P$";
static const std::string sierpinski2DLocalSymbol = "$G$";
static const std::string betaOmega2DLocalSymbols[] = {
    "$R$", "$B$", "$C$", "$D$", "$E$", "$\\Omega$", "$\\Psi$"};
static const std::string gosper2DLocalSymbols[] = {"$G$", "$R$"};
static const std::string morton2DGlobalSymbol = "$M$";

void renderHilbert2D(std::string filename, size_t squareDepth,
                     size_t curveDepth);
void renderHilbert2DGlobalSymbols(std::string filename, size_t squareDepth,
                                  size_t curveDepth);
void renderHilbert2DAdaptive(std::string filename, std::string treeString);
void renderHilbert2DLocalSymbols(std::string filename, size_t squareDepth,
                                 size_t curveDepth);
void renderHilbert2DOrder(std::string filename, size_t depth, size_t base = 10,
                          size_t minNumDigits = 1);
void renderHilbert2DTree(std::string filename, size_t depth);
void renderHilbert2DLocalCorners(std::string filename, size_t squareDepth,
                                 size_t curveDepth);
void renderHilbert3D(std::string filename, size_t depth, bool drawBoundary);

void renderPeano2DGlobalSymbols(std::string filename, size_t squareDepth,
                                size_t curveDepth);
void renderPeano2DLocalSymbols(std::string filename, size_t squareDepth,
                               size_t curveDepth);
void renderSierpinski2DLocalSymbols(std::string filename, size_t squareDepth,
                                    size_t curveDepth);
void renderBetaOmega2DLocalSymbols(std::string filename, size_t squareDepth,
                                   size_t curveDepth);
void renderGosper2DLocalSymbols(std::string filename, size_t squareDepth,
                                size_t curveDepth);
void renderMorton2DGlobalSymbols(std::string filename, size_t squareDepth,
                                 size_t curveDepth);

void renderCustom1(std::string filename, size_t squareDepth, size_t curveDepth);

} /* namespace sfc */
} /* namespace sfcpp */
