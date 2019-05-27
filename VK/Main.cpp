/**
	Implements the main entry point for the VKEngine

	@author		D3PSI
	@version	0.0.1 02.12.2019

	@file		Main.cpp
	@brief		Implementation of the Vulkan-part for my Vulkan vs. OpenGL comparison (Maturaarbeit)
*/

#include "Version.hpp"

#if !defined VK_DEVELOPMENT && defined VK_RELEASE && (defined WIN_64 || defined WIN_32)
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <iostream>
#if defined WIN_64 || defined WIN_32
	#define NOMINMAX		// Fixes std::min/std::max/std::clamp error
	#include <Windows.h>
#endif

#include "VK.hpp"
#include "VKEngine.hpp"
#include "VK_STATUS_CODE.hpp"

namespace vk {

	VKEngine							engine;
	const unsigned int					WIDTH						= 1280;
	const unsigned int					HEIGHT						= 720;
	const char*							TITLE						= "VK by D3PSI";
	const unsigned int					MAX_IN_FLIGHT_FRAMES		= 4;
    VkQueue								transferQueue               = VK_NULL_HANDLE;
    VkCommandPool						transferCommandPool;

	const std::vector< BaseVertex >		vertices					= {
	
        {{-0.5f, -0.5f}, { 1.0f,  1.0f,  0.0f}},
        {{ 0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}},
        {{ 0.5f,  0.5f}, { 0.0f,  1.0f,  1.0f}},
        {{-0.5f,  0.5f}, { 1.0f,  0.0f,  1.0f}}

	};

    const std::vector< uint32_t >       indices                     = {
    
        0, 1, 2, 2, 3, 0
    
    };

	VK_STATUS_CODE init() {
	
		try {

			return engine.init();
		
		} 
		catch (const std::exception& e) {

			std::cerr << e.what() << std::endl;

			return VK_SC_UKNOWN_ERROR;

		}
	
	}

	VkResult createDebugUtilsMessenger(
		VkInstance										instance_,
		const VkDebugUtilsMessengerCreateInfoEXT*		pCreateInfo_,
		const VkAllocationCallbacks*					pAllocator_,
		VkDebugUtilsMessengerEXT*						pDebugMessenger_
		) {
	
		logger::log(EVENT_LOG, "Gathering proc-address for 'vkCreateDebugUtilsMessengerEXT'");
		// Get proc-address from Vulkan
		auto function = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
	
		if (function != nullptr) {

			return function(
				instance_, 
				pCreateInfo_, 
				pAllocator_, 
				pDebugMessenger_
				);
		
		}
		else {
		
			return VK_ERROR_EXTENSION_NOT_PRESENT;

		}
	
	}

	VK_STATUS_CODE destroyDebugUtilsMessenger(
		VkInstance							instance_, 
		VkDebugUtilsMessengerEXT			debugMessenger_, 
		const VkAllocationCallbacks*		pAllocator_
		) {

		logger::log(EVENT_LOG, "Gathering proc-address for 'vkDestroyDebugUtilsMessengerEXT'");
		auto function = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
		if (function != nullptr) {

			function(instance_, debugMessenger_, pAllocator_);
			return VK_SC_SUCCESS;

		}
		else {
		
			return VK_SC_DEBUG_UTILS_MESSENGER_CLEANUP_ERROR;
		
		}
	
	}

	void hideConsole() {
#if defined WIN_64 || defined WIN_32
		::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif
	
	}

	void showConsole() {
#if defined WIN_64 || defined WIN_32
		::ShowWindow(::GetConsoleWindow(), SW_SHOW);
#endif
	}

	bool isConsoleVisible() {
#if defined WIN_64 || defined WIN_32
		return ::IsWindowVisible(::GetConsoleWindow()) != FALSE; 
#else
			return false;
#endif
	}

	const std::vector< char > loadFile(const std::string& filePath_) {
	
		logger::log(EVENT_LOG, "Loading file at '" + filePath_ + "'");

		std::ifstream file(filePath_, std::ios::ate | std::ios::binary);		// Start reading at end of file --> determine the buffer size needed

		if (!file.is_open()) {
		
			logger::log(ERROR_LOG, "Failed to load file at '" + filePath_ + "'");

		}

		size_t bufferSize = (size_t)file.tellg();		// Find read position and thus necessary buffer size
		std::vector< char > buffer(bufferSize);

		file.seekg(0);		// Translate back to the beginning of the file
		file.read(buffer.data(), bufferSize);
		file.close();

		return buffer;
	
	}

    void copyBuffer(VkBuffer srcBuf_, VkBuffer dstBuf_, VkDeviceSize size_) {
    
        VkCommandBufferAllocateInfo allocateInfo                = {};
        allocateInfo.sType                                      = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level                                      = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool                                = transferCommandPool;
        allocateInfo.commandBufferCount                         = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(engine.logicalDevice, &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo commandBufferBeginInfo         = {};
        commandBufferBeginInfo.sType                            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags                            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

        VkBufferCopy copy                                       = {};
        copy.srcOffset                                          = 0;
        copy.dstOffset                                          = 0;
        copy.size                                               = size_;
        vkCmdCopyBuffer(commandBuffer, srcBuf_, dstBuf_, 1, &copy);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo                                 = {};
        submitInfo.sType                                        = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount                           = 1;
        submitInfo.pCommandBuffers                              = &commandBuffer;

        vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(transferQueue);

        vkFreeCommandBuffers(engine.logicalDevice, transferCommandPool, 1, &commandBuffer);

    }

    VkCommandBuffer startCommandBuffer() {
    
        VkCommandBufferAllocateInfo allocInfo       = {};
        allocInfo.sType                             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level                             = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool                       = transferCommandPool;
        allocInfo.commandBufferCount                = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(engine.logicalDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo          = {};
        beginInfo.sType                             = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags                             = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    
    }

    void endCommandBuffer(VkCommandBuffer commandBuffer_) {
    
        vkEndCommandBuffer(commandBuffer_);

        VkSubmitInfo submitInfo             = {};
        submitInfo.sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount       = 1;
        submitInfo.pCommandBuffers          = &commandBuffer_;

        vkQueueSubmit(
            transferQueue, 
            1, 
            &submitInfo, 
            VK_NULL_HANDLE
            );

        vkQueueWaitIdle(transferQueue);

        vkFreeCommandBuffers(
            engine.logicalDevice, 
            transferCommandPool,
            1, 
            &commandBuffer_
            );
    
    }

    void imageLayoutTransition(
        VkImage         image_, 
        VkFormat        format_, 
        VkImageLayout   oldLayout_, 
        VkImageLayout   newLayout_
        ) {
    
        VkCommandBuffer commandBuffer               = startCommandBuffer();

        VkImageMemoryBarrier barrier                = {};
        barrier.sType                               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                           = oldLayout_;
        barrier.newLayout                           = newLayout_;
        barrier.srcQueueFamilyIndex                 = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex                 = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                               = image_;
        barrier.subresourceRange.aspectMask         = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel       = 0;
        barrier.subresourceRange.levelCount         = 1;
        barrier.subresourceRange.baseArrayLayer     = 0 ;
        barrier.subresourceRange.layerCount         = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout_ == VK_IMAGE_LAYOUT_UNDEFINED && newLayout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

            barrier.srcAccessMask       = 0;
            barrier.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage                 = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage            = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout_ == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

            barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask       = VK_ACCESS_SHADER_READ_BIT;

            sourceStage                 = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage            = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        }
        else {

            logger::log(ERROR_LOG, "Unsupported layout transition");

        }


        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, 
            destinationStage, 
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
            );

        endCommandBuffer(commandBuffer);
    
    }

    void copyBufferToImage(
        VkBuffer        buffer_, 
        VkImage         image_, 
        uint32_t        width_, 
        uint32_t        height_
        ) {

        VkCommandBuffer commandBuffer                   = startCommandBuffer();

        VkBufferImageCopy copyRegion                        = {};
        copyRegion.bufferOffset                             = 0;
        copyRegion.bufferRowLength                          = 0;
        copyRegion.bufferImageHeight                        = 0;

        copyRegion.imageSubresource.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.mipLevel                = 0;
        copyRegion.imageSubresource.baseArrayLayer          = 0;
        copyRegion.imageSubresource.layerCount              = 1;

        copyRegion.imageOffset                              = { 0, 0, 0 };
        copyRegion.imageExtent                              = {width_, height_, 1};

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer_,
            image_,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &copyRegion
            );

        endCommandBuffer(commandBuffer);

    }

}

/**
	Entry point for the application
*/
int main() {

	return vk::init();

}