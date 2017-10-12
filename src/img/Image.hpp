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



#ifndef SRC_IMG_IMAGE_HPP_
#define SRC_IMG_IMAGE_HPP_

#include <opencv2/imgproc/imgproc.hpp>
#include "PolygonHierarchy.hpp"

#include <memory>

namespace sfcpp {
namespace img {

template <typename T>
class Image;

template <typename T>
using ImagePtr = std::shared_ptr<Image<T>>;

template <typename T, typename... Args>
ImagePtr<T> createImagePtr(Args &&... args) {
  return std::make_shared<Image<T>>(std::forward<Args>(args)...);
}

template <typename T>
class Image {
 public:
  typedef T value_type;
  cv::Mat data;

  Image(cv::Mat mat) : data(mat) {
    // TODO: make type assert?
  }

  Image(Image<T> const &other) : data(other.data.clone()) {}

  Image(size_t cols, size_t rows) : data(rows, cols, cv::DataType<T>::type) {}

  Image(cv::Size size) : data(size, cv::DataType<T>::type) {}

  Image(size_t cols, size_t rows, T defaultValue)
      : data(rows, cols, cv::DataType<T>::type) {
    for (size_t y = 0; y < rows; ++y) {
      for (size_t x = 0; x < cols; ++x) {
        data.at<T>(y, x) = defaultValue;
      }
    }
  }

  Image(cv::Size size, T defaultValue)
      : Image(size.width, size.height, defaultValue) {}

  Image() : data() {}

  Image<T> &operator=(Image<T> const &img) {
    if (this != &img) {
      data = img.data.clone();
    }

    return *this;
  }

  T &at(int col, int row) { return data.at<T>(row, col); }

  T const &at(int col, int row) const { return data.at<T>(row, col); }

  T atSubpix(cv::Point2f pt) const {
    cv::Mat patch;
    cv::getRectSubPix(data, cv::Size(1, 1), pt, patch);
    return patch.at<T>(0, 0);
  }

  size_t cols() const { return data.cols; }

  size_t rows() const { return data.rows; }

  size_t width() const { return cols(); }

  size_t height() const { return rows(); }

  size_t total() const { return data.total(); }

  cv::Size size() const { return data.size(); }

  ImagePtr<T> clone() { return createImagePtr<T>(*this); }

  void overlay(size_t xOffset, size_t yOffset, ImagePtr<T> const &imgPtr) {
    for (size_t y = 0; y < imgPtr->rows() && y + yOffset < rows(); ++y) {
      for (size_t x = 0; x < imgPtr->cols() && x + xOffset < cols(); ++x) {
        at(x + xOffset, y + yOffset) = imgPtr->at(x, y);
      }
    }
  }

  std::vector<T> columnToVector(size_t col) {
    std::vector<T> result;
    result.reserve(height());

    for (size_t i = 0; i < height(); ++i) {
      result.push_back(at(col, i));
    }

    return result;
  }

  std::vector<T> rowToVector(size_t row) {
    std::vector<T> result;
    result.reserve(width());

    for (size_t i = 0; i < width(); ++i) {
      result.push_back(at(i, row));
    }

    return result;
  }

  void columnFromVector(size_t col, std::vector<T> const &vec) {
    size_t size = std::min(vec.size(), height());

    for (size_t i = 0; i < size; ++i) {
      at(col, i) = vec[i];
    }
  }

  void rowFromVector(size_t row, std::vector<T> const &vec) {
    size_t size = std::min(vec.size(), width());

    for (size_t i = 0; i < size; ++i) {
      at(row, i) = vec[i];
    }
  }

  void save(std::string const &filename) { cv::imwrite(filename, data); }

  PolygonHierarchy getPolygonHierarchyFromImage() {
    PolygonHierarchy tree(this->data);
    return tree;
  }
};

} /* namespace img */
}

#endif /* SRC_IMG_IMAGE_HPP_ */
