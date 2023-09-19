#pragma once

#include "dei/Prelude.hpp"
#include "dei/Vulkan.hpp"
#include "dei/Camera.hpp"

#include <iostream>

namespace {

void RunSandboxLogic() {
    auto extensionCount = uint32_t{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported\n";
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
    std::cout << "Supported extensions:" << std::endl;
    for (const auto& extension : availableExtensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }
    auto c = dei::MakeCamera(-5.0f, vec2{0.0f, 0.0f});
    std::cout << c[0][0] << ' ' << c[0][1] << ' ' << c[0][2] << ' ' << c[0][3] << '\n';
    std::cout << c[1][0] << ' ' << c[1][1] << ' ' << c[1][2] << ' ' << c[1][3] << '\n';
    std::cout << c[2][0] << ' ' << c[2][1] << ' ' << c[2][2] << ' ' << c[2][3] << '\n';
    std::cout << c[3][0] << ' ' << c[3][1] << ' ' << c[3][2] << ' ' << c[3][3] << '\n';
}

}

namespace dei {

bool EngineColdStartup(EngineState& destinationState, const EngineDependencies& dependencies) {
    auto vkInstance = dei::render::CreateVulkanInstance(
        dependencies.RequiredHostExtensions,
        dependencies.RequiredHostExtensionCount);
    if (vkInstance == VK_NULL_HANDLE) {
      return false;
    }
    auto vkSurface = dependencies.CreateVkSurfaceCallback(vkInstance);
    if (vkSurface == VK_NULL_HANDLE) {
      return false;
    }
    destinationState.VulkanInstance = vkInstance;
    destinationState.WindowSurface = vkSurface;

    std::cout << "Created VkInstance: " << vkInstance
              << " VkSurfaceKHR: " << vkSurface << std::endl;

    return true;
}

bool EngineHotStartup(EngineState& engineState) {
    ::RunSandboxLogic();
    return true;
}

bool EngineTick(EngineState& engineState) {
   ++engineState.DrawCounter;
   return true;
}

bool EngineReleaseResources(EngineState& engineState) {
   return true;
}

bool EngineTerminate(EngineState& engineState) {
   auto allocCallback = VkAllocationCallbacks{};
   vkDestroySurfaceKHR(engineState.VulkanInstance, engineState.WindowSurface, &allocCallback);
   engineState.WindowSurface = VK_NULL_HANDLE;
   vkDestroyInstance(engineState.VulkanInstance, &allocCallback);
   engineState.VulkanInstance = VK_NULL_HANDLE;
   return true;
}

}