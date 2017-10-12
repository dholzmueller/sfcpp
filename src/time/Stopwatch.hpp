// Copyright (C) 2008-today The SG++ project

#pragma once

#include <chrono>

namespace sfcpp {
namespace time {

/**
 * Simple stopwatch implementation.
 */
class Stopwatch {
  std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

 public:
  /**
   * Starts the stopwatch.
   */
  Stopwatch();

  /**
   * Re-starts the stopwatch.
   */
  void start();

  /**
   * @return the number of seconds that have passed since the last start.
   */
  double elapsedSeconds();

  /**
   * Prints the result of elapsedSeconds() to the console.
   */
  void log();
};
}
}
