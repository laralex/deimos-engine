#pragma once

#include "dei_platform/TypesFwd.hpp"

#include <vulkan/vulkan.hpp>

namespace dei {

struct EngineDependencies {
    std::function<VkSurfaceKHR(VkInstance)> CreateVkSurfaceCallback;
    u32 RequiredHostExtensionCount;
    const char** RequiredHostExtensions;
};

struct EngineState {
    u32 DrawCounter{0};
    VkSurfaceKHR WindowSurface;
    VkInstance VulkanInstance;
    VkPhysicalDevice PhysicalDevice;
};

}