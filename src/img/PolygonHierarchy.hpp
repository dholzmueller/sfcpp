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


#ifndef POLYGONHIERARCHY_HPP
#define POLYGONHIERARCHY_HPP

//#include "Image.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace sfcpp {
namespace img {

	class PolygonHierarchy{
		public:
			/*
				Constructors
			*/
			PolygonHierarchy();
			PolygonHierarchy(cv::Mat& img);

			//void addChild(PolygonHierarchy child);

			std::vector<cv::Point> getData();

			std::vector<PolygonHierarchy> getSubTree();

		private:
			bool id;
			std::vector<cv::Point> data;
			std::vector<PolygonHierarchy> subTree;

			PolygonHierarchy(std::vector<cv::Point> contour, std::vector<PolygonHierarchy> subTreeParam);

			std::vector<PolygonHierarchy> createSubTree(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy, int startIndex);
	};

}
}

#endif /* POLYGONHIERARCHY_HPP */

