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



#ifndef SRC_IMG_UTILS_HPP_
#define SRC_IMG_UTILS_HPP_

#include <stdint.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include "ClusterPoint.hpp"
#include "Image.hpp"

namespace sfcpp {
namespace img {

/**
 * this method does not have any implementation, it is just used for type
 * inference
 */
template <typename T>
T createSFINAE();

int round(double value);

template <typename T>
Image<int32_t> houghLines(Image<T> img, int angleSteps, int distanceSteps) {
  // TODO: nicht optimal, es muss nur der halbe Winkelbereich durchgegangen
  // werden
  // und es wäre besser für die Distanzen, wenn der Ursprung im Zentrum sitzt
  int width = img.data.cols;
  int height = img.data.rows;
  float angleStepwidth = 2 * M_PI / angleSteps;
  float maxDist = sqrt(width * width + height * height);
  int halfDistanceSteps = distanceSteps / 2;
  float stepsPerDist = halfDistanceSteps / maxDist;

  Image<int32_t> result = cv::Mat::zeros(distanceSteps, angleSteps, CV_32S);

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      if (img.at(x, y) > 0) {
        float angle = 0.0;
        for (int i = 0; i < angleSteps; ++i) {
          // TODO: Rundung hier ist nicht korrekt und optimalerweise muss das
          // min und das max auch nicht hier hin
          int32_t distanceIndex =
              halfDistanceSteps +
              static_cast<int32_t>(
                  stepsPerDist * (x * sin(angle) + y * cos(angle)) + 0.5);
          ++result.at(i,
                      std::max(0, std::min(distanceSteps - 1, distanceIndex)));
          angle += angleStepwidth;
        }
      }
    }
  }

  return result;
}

template <typename T>
T max(ImagePtr<T> imgPtr) {
  auto &img = *imgPtr;
  T maxValue = std::numeric_limits<T>::min();
  for (size_t x = 0; x < img.cols(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) {
      T val = img.at(x, y);
      if (val > maxValue) {
        maxValue = val;
      }
    }
  }

  return maxValue;
}

template <typename T>
T min(ImagePtr<T> imgPtr) {
  auto &img = *imgPtr;
  T minValue = std::numeric_limits<T>::max();
  for (size_t x = 0; x < img.cols(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) {
      T val = img.at(x, y);
      if (val < minValue) {
        minValue = val;
      }
    }
  }

  return minValue;
}

template <typename To, typename From>
ImagePtr<To> normalize(ImagePtr<From> imgPtr, bool useMin = true,
                       To newMin = std::numeric_limits<To>::min(),
                       To newMax = std::numeric_limits<To>::max()) {
  auto &img = *imgPtr;
  From minVal = useMin ? min<From>(imgPtr) : From(0);
  From maxVal = max<From>(imgPtr);
  From fromDist = maxVal - minVal;

  if (fromDist <= 0) {
    fromDist = 1;
  }

  ImagePtr<To> dest = createImagePtr<To>(img.cols(), img.rows());

  for (size_t x = 0; x < img.cols(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) {
      auto val =
          newMin + (img.at(x, y) - minVal) * (newMax - newMin) / fromDist;
      dest->at(x, y) = static_cast<To>(
          std::max(static_cast<decltype(val)>(newMin),
                   std::min(static_cast<decltype(val)>(newMax), val)));
    }
  }

  return dest;
}

template <typename To, typename From>
ImagePtr<To> convert(ImagePtr<From> imgPtr) {
  auto &img = *imgPtr;
  ImagePtr<To> dest = createImagePtr<To>(img.cols(), img.rows());

  for (size_t x = 0; x < img.cols(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) {
      dest->at(x, y) = static_cast<To>(img.at(x, y));
    }
  }

  return dest;
}

template <typename Func, typename In1, typename In2>
auto applyFunction(ImagePtr<In1> firstPtr, ImagePtr<In2> secondPtr, Func func)
    -> ImagePtr<decltype(func(createSFINAE<In1>(), createSFINAE<In2>()))> {
  typedef decltype(func(createSFINAE<In1>(), createSFINAE<In2>())) Out;

  auto &first = *firstPtr;
  auto &second = *secondPtr;

  if (first.cols() != second.cols() || first.rows() != second.rows()) {
    std::cout << "No matching size!\n";
    return createImagePtr<Out>(1, 1);
  }

  ImagePtr<Out> result = createImagePtr<Out>(first.cols(), first.rows());

  for (size_t x = 0; x < first.cols(); ++x) {
    for (size_t y = 0; y < first.rows(); ++y) {
      result->at(x, y) = func(first.at(x, y), second.at(x, y));
    }
  }

  return result;
}

template <typename Func, typename In1>
auto applyFunction(ImagePtr<In1> firstPtr, Func func)
    -> ImagePtr<decltype(func(createSFINAE<In1>()))> {
  typedef decltype(func(createSFINAE<In1>())) Out;

  auto &first = *firstPtr;
  ImagePtr<Out> result = createImagePtr<Out>(first.cols(), first.rows());

  for (size_t x = 0; x < first.cols(); ++x) {
    for (size_t y = 0; y < first.rows(); ++y) {
      result->at(x, y) = func(first.at(x, y));
    }
  }

  return result;
}

template <typename T>
ImagePtr<T> flip(ImagePtr<T> const &imgPtr, bool flipY, bool flipX) {
  auto &img = *imgPtr;
  ImagePtr<T> result(createImagePtr<T>());

  if (flipX) {
    if (flipY) {
      cv::flip(img.data, result->data, -1);
    } else {
      cv::flip(img.data, result->data, 1);
    }
  } else {
    if (flipY) {
      cv::flip(img.data, result->data, 0);
    } else {
      return imgPtr;
    }
  }

  return result;
}

template <typename T>
ImagePtr<T> gaussianFilter(ImagePtr<T> const &imgPtr, float sigmaX,
                           float sigmaY = 0.0, size_t kx = 0, size_t ky = 0) {
  auto &img = *imgPtr;
  ImagePtr<T> result = createImagePtr<T>(img.cols(), img.rows());

  cv::GaussianBlur(img.data, result->data, cv::Size(kx, ky == 0 ? kx : ky),
                   sigmaX, sigmaY, cv::BORDER_REFLECT);

  return result;
}

template <typename T>
ImagePtr<T> applyFilter(ImagePtr<T> const &imgPtr, ImagePtr<T> const &kernelPtr,
                        cv::Point anchor = cv::Point(-1, -1)) {
  auto result = createImagePtr<T>();

  cv::filter2D(imgPtr->data, result->data, -1, kernelPtr->data, anchor, 0,
               cv::BORDER_REFLECT_101);

  return result;
}

template <typename T>
ImagePtr<T> erode(ImagePtr<T> const &imgPtr, ImagePtr<bool> const &kernelPtr,
                  int numIterations = 1, cv::Point anchor = cv::Point(-1, -1)) {
  auto &img = *imgPtr;
  ImagePtr<T> result = createImagePtr<T>(img.cols(), img.rows());

  cv::erode(img.data, result.data, kernelPtr->data, anchor, numIterations);

  return result;
}

template <typename T>
ImagePtr<T> dilate(ImagePtr<T> const &imgPtr, ImagePtr<bool> const &kernelPtr,
                   int numIterations = 1,
                   cv::Point anchor = cv::Point(-1, -1)) {
  auto &img = *imgPtr;
  ImagePtr<T> result = createImagePtr<T>(img.cols(), img.rows());

  cv::dilate(img.data, result.data, kernelPtr->data, anchor, numIterations);

  return result;
}

template <typename T>
ImagePtr<T> closing(ImagePtr<T> const &img, ImagePtr<bool> const &kernel,
                    int numIterations = 1,
                    cv::Point anchor = cv::Point(-1, -1)) {
  return erode(dilate(img, kernel, numIterations, anchor), kernel,
               numIterations, anchor);
}

template <typename T>
ImagePtr<T> opening(ImagePtr<T> const &img, ImagePtr<bool> const &kernel,
                    int numIterations = 1,
                    cv::Point anchor = cv::Point(-1, -1)) {
  return dilate(erode(img, kernel, numIterations, anchor), kernel,
                numIterations, anchor);
}

template <typename T>
ImagePtr<T> resize(ImagePtr<T> const &img, size_t width, size_t height) {
  ImagePtr<T> result = createImagePtr<T>(width, height);

  // use cubic interpolation for enlarging and area interpolation for shrinking
  cv::resize(img->data, result->data, cv::Size(width, height),
             width * height > img.cols() * img.rows() ? cv::INTER_CUBIC
                                                      : cv::INTER_AREA);

  return result;
}

template <typename T>
ImagePtr<T> resizeKeepAspectRatio(ImagePtr<T> const &imgPtr, int width,
                                  int height, T fillValue) {
  auto &img = *imgPtr;
  double oldWidth = img.cols();
  double oldHeight = img.rows();

  double widthRatio = width / oldWidth;
  double heightRatio = height / oldHeight;

  ImagePtr<T> intermediate = createImagePtr<T>();

  if (widthRatio < heightRatio) {
    intermediate =
        resize(img, width, std::min(height, round(oldHeight * widthRatio)));
  } else {
    intermediate =
        resize(img, std::min(width, round(oldWidth * heightRatio)), height);
  }

  ImagePtr<T> result = createImagePtr<T>(width, height, fillValue);

  result->overlay((width - intermediate.width()) / 2,
                  (height - intermediate.height()) / 2, intermediate);

  return result;
}

template <typename T>
ImagePtr<T> crop(ImagePtr<T> const &imgPtr, cv::Rect rect) {
  ImagePtr<T> result = createImagePtr<T>();

  result.data = imgPtr->data(rect).clone();

  return result;
}

template <typename T>
ImagePtr<T> clusterThresholding(ImagePtr<cv::Vec3b> const &imgPtr,
                                std::vector<ClusterPoint<T>> clusterPoints) {
  auto &img = *imgPtr;
  ImagePtr<T> result = createImagePtr<T>(img.cols(), img.rows());

  for (size_t y = 0; y < img.rows(); ++y) {
    for (size_t x = 0; x < img.cols(); ++x) {
      double minDist = std::numeric_limits<double>::max();
      T color;
      for (auto point : clusterPoints) {
        double db = (int)img.at(x, y)[0] - (int)point.clusterCenter[0];
        double dg = (int)img.at(x, y)[1] - (int)point.clusterCenter[1];
        double dr = (int)img.at(x, y)[2] - (int)point.clusterCenter[2];
        double currentDist = sqrt(db * db + dg * dg + dr * dr);
        if (currentDist < minDist) {
          minDist = currentDist;
          color = point.resultingColor;
        }
      }
      result->at(x, y) = color;
    }
  }

  return result;
}

template <typename T>
ImagePtr<T> overlay(ImagePtr<T> const &imgPtr, ImagePtr<bool> const &maskPtr,
                    T const &color) {
  auto &img = *imgPtr;
  auto &mask = *maskPtr;
  if (img.rows() != mask.rows() || img.cols() != mask.cols()) {
    std::cout << "No matching size in overlay()\n";
    return Image<T>(1, 1);
  }

  ImagePtr<T> result = createImagePtr<T>(img);

  for (size_t y = 0; y < img.rows(); ++y) {
    for (size_t x = 0; x < img.cols(); ++x) {
      if (mask.at(x, y)) {
        result->at(x, y) = color;
      }
    }
  }

  return result;
}

template <size_t channels, typename T>
ImagePtr<cv::Vec<T, channels>> extendChannels(ImagePtr<T> const &imgPtr) {
  auto &img = *imgPtr;
  ImagePtr<cv::Vec<T, channels>> result =
      createImagePtr<cv::Vec<T, channels>>(img.cols(), img.rows());

  for (size_t y = 0; y < img.rows(); ++y) {
    for (size_t x = 0; x < img.cols(); ++x) {
      cv::Vec<T, channels> vec;
      for (size_t i = 0; i < channels; ++i) {
        vec[i] = img.at(x, y);
      }
      result->at(x, y) = vec;
    }
  }

  return result;
}

template <typename T, typename Pred>
cv::Rect getPredicateBoundingRect(ImagePtr<T> const &imgPtr, Pred pred) {
  auto &img = *imgPtr;
  size_t x1 = img.width(), x2 = 0, y1 = img.height(), y2 = 0;

  for (size_t x = 0; x < img.width(); ++x) {
    for (size_t y = 0; y < img.height(); ++y) {
      if (pred(img.at(x, y))) {
        x1 = std::min(x, x1);
        x2 = std::max(x, x2);
        y1 = std::min(y, y1);
        y2 = std::max(y, y2);
      }
    }
  }

  if (x2 < x1 || y2 < y1) {
    return cv::Rect(0, 0, 1, 1);
  }

  return cv::Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

template <typename T, typename Pred>
std::vector<cv::Vec2f> getPointsForPred(ImagePtr<T> const &imgPtr, Pred pred) {
  auto &img = *imgPtr;
  std::vector<cv::Vec2f> result;
  for (size_t y = 0; y < img.height(); ++y) {
    for (size_t x = 0; x < img.width(); ++x) {
      if (pred(img.at(x, y))) {
        result.push_back(cv::Vec2f(x, y));
      }
    }
  }

  return result;
}

template <int count>
std::vector<cv::Vec<float, count>> kmeans(
    std::vector<cv::Vec<float, count>> const &data, size_t k) {
  std::vector<cv::Vec<float, count>> result(k, cv::Vec<float, count>(0.0));

  cv::Mat points(data.size(), count, CV_32F);
  for (size_t i = 0; i < data.size(); ++i) {
    for (size_t j = 0; j < count; ++j) {
      points.at<float>(i, j) = data[i][j];
    }
  }

  std::vector<int> labels;
  cv::kmeans(points, k, labels,
             cv::TermCriteria(cv::TermCriteria::COUNT, 100, 1.0), 5,
             cv::KMEANS_PP_CENTERS);

  std::vector<int> clusterCounts(k, 0);

  for (size_t i = 0; i < labels.size(); ++i) {
    result[labels[i]] += data[i];
    clusterCounts[labels[i]]++;
  }

  for (size_t i = 0; i < k; ++i) {
    result[i] /= clusterCounts[i] == 0 ? 1 : clusterCounts[0];
  }

  return result;
}

template <typename T>
ImagePtr<T> minImage(ImagePtr<T> first, ImagePtr<T> second) {
  return applyFunction(first, second, [](T a, T b) { return std::min(a, b); });
}

template <typename T>
ImagePtr<T> maxImage(ImagePtr<T> first, ImagePtr<T> second) {
  return applyFunction(first, second, [](T a, T b) { return std::max(a, b); });
}

ImagePtr<bool> floodFill(ImagePtr<bool> const &imgPtr, size_t x, size_t y);

ImagePtr<bool> circularMorphologyKernel(float distance);

ImagePtr<int32_t> watershedTransform(ImagePtr<cv::Vec3b> const &imgPtr,
                                     ImagePtr<int32_t> markers);

ImagePtr<float> distanceTransform(ImagePtr<bool> const &imgPtr);

ImagePtr<cv::Vec3b> gray2BGR(ImagePtr<uint8_t> const &imgPtr);
ImagePtr<uint8_t> BGR2Gray(ImagePtr<uint8_t> const &imgPtr);

} /* namespace img */
}

#endif /* SRC_IMG_UTILS_HPP_ */
