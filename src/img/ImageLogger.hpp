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


#ifndef IMAGELOGGER_HPP
#define IMAGELOGGER_HPP

#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <stdint.h>
#include <opencv2/highgui/highgui.hpp>
#include "Image.hpp"
#include "utils.hpp"

namespace sfcpp {
namespace img {

class ImageLogger {
 public:
  ImageLogger(bool i_write_switch = true);

  bool writeImage(ImagePtr<bool> const &img) {
    return writeImage(normalize<uint8_t>(img));
  }

  template <typename T>
  bool writeImage(ImagePtr<T> const &img) {
    if (write_switch) {
      std::ostringstream ostr;
      ostr << std::setprecision(3) << image_count++;
      cv::imwrite(
          "./data/ImageLog/" + destination_name + "/img_" + ostr.str() + ".png",
          img->data);
    } else {
      // std::cout << "" << std::endl;
    }

    return true;
  }

 private:
  bool write_switch;
  std::string destination_name;
  int image_count;
};

} /* namespace img */
}

#endif /* IMAGELOGGER_HPP */
