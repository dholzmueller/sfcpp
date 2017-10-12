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



#include "utils.hpp"
#include <cmath>
#include "functional.hpp"

using namespace cv;
using namespace std;

namespace sfcpp {
namespace img {

ImagePtr<int32_t> watershedTransform(const ImagePtr<cv::Vec3b>& img,
                                     ImagePtr<int32_t> markers) {
  ImagePtr<int32_t> result = createImagePtr<int32_t>(*markers);

  cv::watershed(img->data, result->data);

  return result;
}

ImagePtr<float> distanceTransform(ImagePtr<bool> const& img) {
  ImagePtr<float> result = createImagePtr<float>();

  cv::distanceTransform(img->data, result->data, CV_DIST_L2,
                        CV_DIST_MASK_PRECISE);

  return result;
}

ImagePtr<bool> circularMorphologyKernel(float distance) {
  int roundedDistance = (int)abs(distance) + 1;
  int kernelSize = 2 * roundedDistance + 1;
  float squareDist = distance * distance;
  ImagePtr<bool> result = createImagePtr<bool>(kernelSize, kernelSize);

  for (size_t y = 0; y < result->rows(); ++y) {
    for (size_t x = 0; x < result->cols(); ++x) {
      float dx = roundedDistance - (float)x;
      float dy = roundedDistance - (float)y;

      result->at(x, y) = (dx * dx + dy * dy <= squareDist);
    }
  }

  return result;
}

ImagePtr<bool> floodFill(const ImagePtr<bool>& img, size_t x, size_t y) {
  ImagePtr<bool> negated = img::applyFunction(img, img::lnot);

  cv::floodFill(negated->data, cv::Point(x, y), cv::Scalar(0));

  return img::applyFunction(negated, img::lnot);
}

int round(double value) { return static_cast<int>(value + 0.5); }

ImagePtr<cv::Vec3b> gray2BGR(const ImagePtr<uint8_t>& imgPtr) {
  auto result = createImagePtr<cv::Vec3b>();
  cv::cvtColor(imgPtr->data, result->data, CV_GRAY2BGR);
  return result;
}

ImagePtr<uint8_t> BGR2Gray(const ImagePtr<uint8_t>& imgPtr) {
  auto result = createImagePtr<uint8_t>();
  cv::cvtColor(imgPtr->data, result->data, CV_BGR2GRAY);
  return result;
}

} /* namespace img */
}
