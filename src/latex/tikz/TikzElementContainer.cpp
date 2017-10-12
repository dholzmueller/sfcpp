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



#include <latex/tikz/TikzElementContainer.hpp>

namespace sfcpp {
namespace latex {
namespace tikz {

TikzElementContainer::TikzElementContainer() : elements() {

}

TikzElementContainer::TikzElementContainer(const TikzElementContainer& other) : elements(other.elements) {
}

TikzElementContainer::~TikzElementContainer() {
}

void TikzElementContainer::addElement(std::shared_ptr<TikzElement> element) {
	elements.push_back(element);
}

std::shared_ptr<TikzElement> TikzElementContainer::clone() const {
	std::shared_ptr<TikzElementContainer> container(new TikzElementContainer());

	for(auto &element : elements) {
		container->addElement(element->clone());
	}

	return container;
}

void TikzElementContainer::setCoordinateSystem(std::shared_ptr<sfcpp::geo::CoordinateSystem> const &coordinateSystem) {
	for(auto &element : elements) {
		element->setCoordinateSystem(coordinateSystem);
	}
}

std::string TikzElementContainer::getCode() const {
	std::string result;

	for(auto &element : elements) {
		result += element->getCode();
	}

	return result;
}

} /* namespace tikz */
} /* namespace latex */
} /* namespace utils */
