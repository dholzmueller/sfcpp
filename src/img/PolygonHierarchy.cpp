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


#include "PolygonHierarchy.hpp"
#include <iostream>

namespace sfcpp {
namespace img{

	//public functions
	PolygonHierarchy::PolygonHierarchy(): id(false), data(), subTree() {}

	PolygonHierarchy::PolygonHierarchy(cv::Mat& img):data(), subTree(){
		std::vector<std::vector<cv::Point>> contoursFine;
		std::vector<std::vector<cv::Point>> contoursCoarsened;
		std::vector<cv::Vec4i> hierarchy;
		findContours(img, contoursFine, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS);

		contoursCoarsened.resize(contoursFine.size());
		for (size_t i = 0; i < contoursFine.size(); i++){
            //std::cout << "NoFinePoints: " << contoursFine[i].size() << std::endl;
            approxPolyDP(contoursFine[i], contoursCoarsened[i], 0.5/*50.0/contoursFine[i].size()*/, true);
            //std::cout << "NoCoarsePoints: " << contoursCoarsened[i].size() << std::endl;
            //std::cout << "Coarsening Factor: " << contoursFine[i].size()/contoursCoarsened[i].size() << std::endl;
		}
		if (contoursCoarsened.size() > 0){
			subTree = createSubTree(contoursCoarsened, hierarchy, 0);
		}
	}

	//private functions

	PolygonHierarchy::PolygonHierarchy(std::vector<cv::Point> contour, std::vector<PolygonHierarchy> subTreeParam) :id(false), data(contour), subTree(subTreeParam){}

	std::vector<PolygonHierarchy> PolygonHierarchy::createSubTree(std::vector<std::vector<cv::Point>> contours, std::vector<cv::Vec4i> hierarchy, int startIndex){
		std::vector<PolygonHierarchy> subTreeLevel;
		int index = startIndex;

		while(index >= 0){
			std::vector<PolygonHierarchy> subTree;
			if(hierarchy[index][2] >= 0){
				//inner node = has children
				subTree = createSubTree(contours, hierarchy, hierarchy[index][2]);
			}
			else{
				//leaf node = no children
				//subTree.resize(0);
			}
			PolygonHierarchy element(contours[index], subTree);
			subTreeLevel.resize(subTreeLevel.size() + 1);
			subTreeLevel[subTreeLevel.size() - 1] = element;

			index = hierarchy[index][0];
		}
		return subTreeLevel;
	}

	std::vector<cv::Point> PolygonHierarchy::getData(){
		return data;
	}

	std::vector<PolygonHierarchy> PolygonHierarchy::getSubTree(){
		return subTree;
	}
}
}


