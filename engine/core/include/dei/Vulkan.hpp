#pragma once

#include "dei/Prelude.hpp"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace dei::render {

struct VulkanContext {
   VkInstance instance;
	VkSurfaceKHR windowSurface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	// VkSemaphore imageAvailableSemaphore;
	// VkSemaphore renderingFinishedSemaphore;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkCommandPool commandPool;
   std::vector<VkCommandBuffer> presentCommandBuffers;
};

auto CreateVulkanInstance(const char** requiredExtensions, u32 requiredExtensionsCount) -> VkInstance {
   auto appInfo = VkApplicationInfo{VK_STRUCTURE_TYPE_APPLICATION_INFO};
   appInfo.pNext = nullptr;
   appInfo.pApplicationName = "Deimos App";
   appInfo.pEngineName = "Deimos Engine";
   appInfo.engineVersion = 1;
   appInfo.applicationVersion = 1;
   appInfo.apiVersion = VK_MAKE_API_VERSION(0,1,3,0);

   auto info = VkInstanceCreateInfo{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
   info.pNext = nullptr;
   info.pApplicationInfo = &appInfo;
   info.enabledExtensionCount = requiredExtensionsCount;
   info.ppEnabledExtensionNames = requiredExtensions;
   info.enabledLayerCount = 0;
   info.ppEnabledLayerNames = nullptr;
   info.flags = 0; //VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

   auto instance = VkInstance{};
   auto result = vkCreateInstance(&info, nullptr, &instance);

   return instance;
}

}