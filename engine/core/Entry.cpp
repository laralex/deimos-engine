#include "dei/Entry.hpp"
#include "dei/Vulkan.hpp"
#include "dei/Camera.hpp"
#include "dei_platform/TypesVec.hpp"
#include "dei_platform/TypesMat.hpp"

#include <iostream>

namespace {

void RunSandboxLogic() {
    auto extensionCount = u32{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported\n";
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
    std::cout << "Supported extensions:" << std::endl;
    for (const auto& extension : availableExtensions) {
        std::cout << "\t" << extension.extensionName << std::endl;
    }
    auto c = dei::MakeCamera(-5.0f, dei::vec2f{0.0f, 0.0f});
    std::cout << c[0][0] << ' ' << c[0][1] << ' ' << c[0][2] << ' ' << c[0][3] << '\n';
    std::cout << c[1][0] << ' ' << c[1][1] << ' ' << c[1][2] << ' ' << c[1][3] << '\n';
    std::cout << c[2][0] << ' ' << c[2][1] << ' ' << c[2][2] << ' ' << c[2][3] << '\n';
    std::cout << c[3][0] << ' ' << c[3][1] << ' ' << c[3][2] << ' ' << c[3][3] << '\n';
}

}

namespace dei {

b8 EngineColdStartup(EngineState& destinationState, const EngineDependencies& dependencies) {
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

    VkPhysicalDeviceFeatures requiredDeviceFeatures;
    requiredDeviceFeatures.robustBufferAccess                         = false;
    requiredDeviceFeatures.fullDrawIndexUint32                        = false;
    requiredDeviceFeatures.imageCubeArray                             = false;
    requiredDeviceFeatures.independentBlend                           = false;
    requiredDeviceFeatures.geometryShader                             = false;
    requiredDeviceFeatures.tessellationShader                         = false;
    requiredDeviceFeatures.sampleRateShading                          = false;
    requiredDeviceFeatures.dualSrcBlend                               = false;
    requiredDeviceFeatures.logicOp                                    = false;
    requiredDeviceFeatures.multiDrawIndirect                          = false;
    requiredDeviceFeatures.drawIndirectFirstInstance                  = false;
    requiredDeviceFeatures.depthClamp                                 = false;
    requiredDeviceFeatures.depthBiasClamp                             = false;
    requiredDeviceFeatures.fillModeNonSolid                           = false;
    requiredDeviceFeatures.depthBounds                                = false;
    requiredDeviceFeatures.wideLines                                  = false;
    requiredDeviceFeatures.largePoints                                = false;
    requiredDeviceFeatures.alphaToOne                                 = false;
    requiredDeviceFeatures.multiViewport                              = false;
    requiredDeviceFeatures.samplerAnisotropy                          = true;
    requiredDeviceFeatures.textureCompressionETC2                     = false;
    requiredDeviceFeatures.textureCompressionASTC_LDR                 = false;
    requiredDeviceFeatures.textureCompressionBC                       = false;
    requiredDeviceFeatures.occlusionQueryPrecise                      = false;
    requiredDeviceFeatures.pipelineStatisticsQuery                    = false;
    requiredDeviceFeatures.vertexPipelineStoresAndAtomics             = false;
    requiredDeviceFeatures.fragmentStoresAndAtomics                   = false;
    requiredDeviceFeatures.shaderTessellationAndGeometryPointSize     = false;
    requiredDeviceFeatures.shaderImageGatherExtended                  = false;
    requiredDeviceFeatures.shaderStorageImageExtendedFormats          = false;
    requiredDeviceFeatures.shaderStorageImageMultisample              = false;
    requiredDeviceFeatures.shaderStorageImageReadWithoutFormat        = false;
    requiredDeviceFeatures.shaderStorageImageWriteWithoutFormat       = false;
    requiredDeviceFeatures.shaderUniformBufferArrayDynamicIndexing    = false;
    requiredDeviceFeatures.shaderSampledImageArrayDynamicIndexing     = false;
    requiredDeviceFeatures.shaderStorageBufferArrayDynamicIndexing    = false;
    requiredDeviceFeatures.shaderStorageImageArrayDynamicIndexing     = false;
    requiredDeviceFeatures.shaderClipDistance                         = false;
    requiredDeviceFeatures.shaderCullDistance                         = false;
    requiredDeviceFeatures.shaderFloat64                              = false;
    requiredDeviceFeatures.shaderInt64                                = false;
    requiredDeviceFeatures.shaderInt16                                = false;
    requiredDeviceFeatures.shaderResourceResidency                    = false;
    requiredDeviceFeatures.shaderResourceMinLod                       = false;
    requiredDeviceFeatures.sparseBinding                              = false;
    requiredDeviceFeatures.sparseResidencyBuffer                      = false;
    requiredDeviceFeatures.sparseResidencyImage2D                     = false;
    requiredDeviceFeatures.sparseResidencyImage3D                     = false;
    requiredDeviceFeatures.sparseResidency2Samples                    = false;
    requiredDeviceFeatures.sparseResidency4Samples                    = false;
    requiredDeviceFeatures.sparseResidency8Samples                    = false;
    requiredDeviceFeatures.sparseResidency16Samples                   = false;
    requiredDeviceFeatures.sparseResidencyAliased                     = false;
    requiredDeviceFeatures.variableMultisampleRate                    = false;
    requiredDeviceFeatures.inheritedQueries                           = false;
    dei::render::GetVulkanPhysicalDevices(
        destinationState.VulkanInstance,
        requiredDeviceFeatures);

    return true;
}

b8 EngineHotStartup(EngineState& engineState) {
    ::RunSandboxLogic();
    return true;
}

b8 EngineTick(EngineState& engineState) {
   ++engineState.DrawCounter;
   return true;
}

b8 EngineReleaseResources(EngineState& engineState) {
   return true;
}

b8 EngineTerminate(EngineState& engineState) {
   auto allocCallback = VkAllocationCallbacks{};
   vkDestroySurfaceKHR(engineState.VulkanInstance, engineState.WindowSurface, &allocCallback);
   engineState.WindowSurface = VK_NULL_HANDLE;
   vkDestroyInstance(engineState.VulkanInstance, &allocCallback);
   engineState.VulkanInstance = VK_NULL_HANDLE;
   return true;
}

}