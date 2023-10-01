#pragma once

#include "dei/Prelude.hpp"

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

auto CreateVulkanInstance(const char** requiredExtensions, u32 requiredExtensionsCount) -> VkInstance;

class PhysicalDevice {
public:
	static auto QueryAll(VkInstance) -> std::optional<std::vector<PhysicalDevice>>;
	static auto QueryAll(
		VkInstance,
		const VkPhysicalDeviceFeatures& requiredFeatures)
		-> std::optional<std::vector<PhysicalDevice>>;
	static auto QueryAll(
		VkInstance,
		const VkPhysicalDeviceLimits& requiredLimits)
		-> std::optional<std::vector<PhysicalDevice>>;
	static auto QueryAll(
		VkInstance,
		const VkPhysicalDeviceFeatures& requiredFeatures,
		const VkPhysicalDeviceLimits& requiredLimits)
		-> std::optional<std::vector<PhysicalDevice>>;
	PhysicalDevice(PhysicalDevice&&) = default;
	auto GetVendorName() const -> const char*;
	auto GetDeviceTypeName() const -> const char*;
	auto GetMaxFramebufferSamples(bool ofColor=true, bool ofDepth=false, bool ofStencil=false) const -> VkSampleCountFlagBits;
	auto GetMaxFramebufferSamplesNoAttachments() const -> VkSampleCountFlagBits;
	auto GetDevice() const& -> const VkPhysicalDevice& { return _device; }
	auto GetDevice() && -> VkPhysicalDevice&& { return std::move(_device); }
	auto GetFeatures() const -> const VkPhysicalDeviceFeatures& { return _features; }
	auto GetProperties() const -> const VkPhysicalDeviceProperties& { return _properties; }
	auto GetLimits() const -> const VkPhysicalDeviceLimits& {return _properties.limits; }
	auto HasFeatures(const VkPhysicalDeviceFeatures&) const -> b8;
	auto HasLimits(const VkPhysicalDeviceLimits&) const -> b8;
private:
	PhysicalDevice(VkPhysicalDevice&& d, VkPhysicalDeviceFeatures&& f, VkPhysicalDeviceProperties&& p)
		: _device(std::move(d)), _features(std::move(f)), _properties(std::move(p)) {}
	VkPhysicalDevice _device;
	const VkPhysicalDeviceFeatures _features;
	const VkPhysicalDeviceProperties _properties;
};

auto PrintPhysicalDevice(const PhysicalDevice&) -> void;

} // namespace dei