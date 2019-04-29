/**
	Declares the VKEngine class

	@author		D3PSI
	@version	0.0.1 02.12.2019

	@file		VKEngine.hpp
	@brief		Declaration of the VKEngine class
*/

#pragma once
#include "Version.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <conio.h>
#include <set>
#include <map>
#include <thread>
#include <algorithm>
#include <string>

#include "VK_STATUS_CODE.hpp"
#include "Logger.hpp"
#include "ASSERT.cpp"
#include "QueueFamily.cpp"
#include "LoadingScreen.hpp"
#include "SwapchainDetails.cpp"

class VKEngine {
public:

	/**
		Initializes VKEngine and loads dependencies

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE init(void);

private:

	VkResult								result								= VK_SUCCESS;
	GLFWwindow*								window								= nullptr;
	VkAllocationCallbacks*					allocator							= nullptr;
	VkInstance								instance							= VK_NULL_HANDLE;
	const std::vector< const char* >		validationLayers					= {
	
		"VK_LAYER_LUNARG_standard_validation"
	
	};
#ifdef VK_DEVELOPMENT
	const bool								validationLayersEnabled				= true;
#else
	const bool								validationLayersEnabled				= false;
#endif
	VkDebugUtilsMessengerEXT				validationLayerDebugMessenger		= VK_NULL_HANDLE;
	VkPhysicalDevice						physicalDevice						= VK_NULL_HANDLE;
	VkDevice								logicalDevice						= VK_NULL_HANDLE;
	VkQueue									graphicsQueue						= VK_NULL_HANDLE;
	VkQueue									presentationQueue					= VK_NULL_HANDLE;
	VkSurfaceKHR							surface								= VK_NULL_HANDLE;
	const std::vector< const char* >		requiredExtensions					= {
	
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	
	};
	LoadingScreen*							loadingScreen						= nullptr;
	VkSwapchainKHR							swapchain							= VK_NULL_HANDLE;
	VkFormat								swapchainImageFormat;
	VkExtent2D								swapchainImageExtent;
	std::vector< VkImage >					swapchainImages;
	std::vector< VkImageView >				swapchainImageViews;

	/**
		Initializes the logger

		@return		Returns LOGGER_SC_SUCCESS on success
		@return		Returns LOGGER_SC_UNKNOWN_ERROR on error
	*/
	LOGGER_STATUS_CODE initLogger(void);

	/**
		Initializes the windowing library

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE initWindow(void);

	/**
		Initializes the Vulkan API

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE initVulkan(void);

	/**
		Contains the main loop

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE loop(void);

	/**
		Handles cleaning of resources on exit

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE clean(void);

	/**
		Creates the VkInstance class member

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE createInstance(void);

	/**
		Checks whether validation layers are supported

		@return		Returns true if validation layers are supported
		@return		Returns false if validation layers are not supported
	*/
	bool validationLayersSupported(void);

	/**
		Get the required extensions for Vulkan to activate

		@return		Returns an std::vector< const char* > with extension names
	*/
	std::vector< const char* > queryRequiredExtensions(void);

	/**
		Message callback function for Vulkan validation layers so they can actually display their messages

		@param		messageSeverity_	Message severity level (internal)
		@param		messageType_		Message type (internal)
		@param		pCallbackData_		Actual message (internal)
		@param		pUserData_			User specified data

		@return		Returns VK_FALSE

	*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerDebugMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT			messageSeverity_,
		VkDebugUtilsMessageTypeFlagsEXT					messageType_,
		const VkDebugUtilsMessengerCallbackDataEXT*		pCallbackData_,
		void*											pUserData_
		);

	/**
		Creates the VkDebugUtilsMessengerEXT for validation layer messages

		@return		Returns VK_SC_SUCCESS on success
		@return		Returns VK_SC_DEBUG_UTILS_MESSENGER_CREATION_ERROR on error
	*/
	VK_STATUS_CODE debugUtilsMessenger(void);

	/**
		Enumerates and selects the best physical device (GPU)

		@return		Returns VK_SC_SUCCESS on success
		@return		Returns VK_SC_PHYSICAL_DEVICE_ERROR on error
	*/
	VK_STATUS_CODE selectBestPhysicalDevice(void);

	/**
		Rates GPU for the operations that are about to be performed

		@param		device_		The VkPhysicalDevice to be rated

		@return		Returns an integer value as score
	*/
	int evaluateDeviceSuitabilityScore(VkPhysicalDevice device_);

	/**
		Prints information about GPU !!!NOT YET FULLY IMPLEMENTED!!!

		@param		device_		The VkPhysicalDevice to print information about

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE printPhysicalDevicePropertiesAndFeatures(VkPhysicalDevice device_);

	/**
		Finds queue families that are suitable for the operations that are about to be performed on them
	
		@param		device_		A valid VkPhysicalDevice handle whose queue families are to be tested

		@return		Returns a QueueFamily struct
	*/
	QueueFamily findSuitableQueueFamilies(VkPhysicalDevice device_);

	/**
		Creates a VkDevice handle from a VkPhysicalDevice (class member)

		@return		Returns VK_SC_SUCCESS on success
		@return		Returns VK_SC_LOGICAL_DEVICE_ERROR on error
	*/
	VK_STATUS_CODE createLogicalDeviceFromPhysicalDevice(void);

	/**
		Creates a VkSurfaceKHR handle via GLFW

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE createSurfaceGLFW(void);

	/**
		Checks whether the device supports the swapchain extension (should be implicit if it supports Vulkan, but double checking never hurts, right?)
		
		@param		device_		The VkPhysicalDevice handle that should be tested

		@return		Returns true if the device supports the VK_KHR_swapchain extension
		@return		Returns false if the device does not support the VK_KHR_swapchain extensions
	*/
	bool checkDeviceSwapchainExtensionSupport(VkPhysicalDevice device_);

	/**
		Initializes the loading screen
	*/
	void initLoadingScreen(void);

	/**
		Queries the specified device's swap chain capabilities and details

		@param		device_		The device whose details are to be enumerated

		@return		Returns a SwapchainDetails struct containing the enumerated information
	*/
	SwapchainDetails querySwapchainDetails(VkPhysicalDevice device_);

	/**
		Determines the best swapchain-format to use (in the entire application)

		@param		availableFormats_		A reference to an std::vector containing formats to choose from

		@return		Returns the preferred VkSurfaceFormatKHR
	*/
	VkSurfaceFormatKHR evaluateBestSwapchainSurfaceFormat(const std::vector< VkSurfaceFormatKHR >& availableFormats_);

	/**
		Determines the best swapchain-present-mode to use

		@param		availablePresentModes_		A reference to an std::vector containing present modes to choose from

		@return		Returns the preferred VkPresentModeKHR
	*/
	VkPresentModeKHR evaluateBestSwapchainSurfacePresentMode(const std::vector< VkPresentModeKHR >& availablePresentModes_);

	/**
		Determines the swapchain extent from a set of given surface capabilities

		@param		capabilities_		The supported swapchain-extents of the GPU can be fount in a VkSurfaceCapabilitiesKHR struct

		@return		Returns a VkExtent2D which should be specified in VkSwapchainCreateInfoKHR
	*/
	VkExtent2D evaluateSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities_); 

	/**
		Creates a VkSwapchain handle

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE createSwapchain(void);

	/**
		Creates an array of VkImageViews for the VkImages in the swapchain

		@return		Returns VK_SC_SUCCESS on success
	*/
	VK_STATUS_CODE createSwapchainImageViews(void);

};
