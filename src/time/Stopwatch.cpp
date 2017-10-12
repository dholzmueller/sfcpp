// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include "Stopwatch.hpp"

#include <iostream>

namespace sfcpp {
namespace time {

Stopwatch::Stopwatch() : startTime(std::chrono::high_resolution_clock::now()) {}

void Stopwatch::start() { startTime = std::chrono::high_resolution_clock::now(); }

double Stopwatch::elapsedSeconds() {
  std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - startTime;
  return diff.count();
}

void Stopwatch::log() { std::cout << "Time: " << elapsedSeconds() << "s." << std::endl; }
}
} /* namespace SGPP */
