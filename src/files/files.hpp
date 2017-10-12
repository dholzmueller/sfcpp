// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

#pragma once

#include <string>

namespace sfcpp {
namespace files {

/**
 * @return Returns a string with the content of the given file if the file exists and "" otherwise.
 */
std::string readFromFile(std::string filename);

/**
 * Writes the string content into the specified file. If the file already exists, its content is
 * overwritten.
 */
void writeToFile(std::string filename, std::string content);
}
}
