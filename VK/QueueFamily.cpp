/**
	Defines a struct to handle queue families

	@author		D3PSI
	@version	0.0.1 02.12.2019

	@file		QueueFamily.cpp
	@brief		Definition of QueueFamily struct
*/
#pragma once
#include <optional>

/**
	Contains index to queue family
*/
struct QueueFamily {

	std::optional< uint32_t > graphicsFamilyIndex;		// using std::optional here allows to use one boolean variable less and is a neat C++17 feature
	std::optional< uint32_t > presentationFamilyIndex;

	/**
		Checks whether a queue family index has been found

		@return		Returns true if an index exists
	*/
	bool isComplete(void) {
	
		return graphicsFamilyIndex.has_value() && presentationFamilyIndex.has_value();

	}

};