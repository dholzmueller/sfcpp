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



#ifndef SRC_IMG_CLUSTERPOINT_HPP_
#define SRC_IMG_CLUSTERPOINT_HPP_

#include <opencv2/imgproc/imgproc.hpp>


namespace sfcpp {
namespace img {

template<typename T> class ClusterPoint {
public:
	typedef T value_type;

	cv::Vec3b clusterCenter;
	T resultingColor;

	ClusterPoint(cv::Vec3b const &clusterCenter, T const &resultingColor)
		: clusterCenter(clusterCenter), resultingColor(resultingColor) {}
};

}
}

#endif /* SRC_IMG_CLUSTERPOINT_HPP_ */
