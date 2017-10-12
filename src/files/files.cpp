// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#include "files.hpp"

#include <fstream>
#include <sstream>

namespace sfcpp {
namespace files {

std::string readFromFile(std::string filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    return "";
  }

  std::ostringstream stream;
  stream << file.rdbuf();
  return stream.str();
}

void writeToFile(std::string filename, std::string content) {
  std::ofstream file(filename);
  file << content;
}
}
}
