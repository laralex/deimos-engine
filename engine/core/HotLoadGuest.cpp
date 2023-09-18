#include "dei/Prelude.hpp"
#include "dei/Camera.hpp"
#include "dei/Vulkan.hpp"

#include <cr.h>

#include <iostream>
#include <functional>


struct HotReloadState {
    u32 DrawCounter{0};
    VkSurfaceKHR WindowSurface;
    VkInstance VulkanInstance;
    std::function<VkSurfaceKHR(VkInstance)> CreateVkSurfaceCallback;
    u32 RequiredHostExtensionCount;
    const char** RequiredHostExtensions;
};

static HotReloadState* state{nullptr};

auto OnLoad(cr_plugin *ctx) -> int {
    std::cout << "dei::OnLoad() v" << ctx->version << " e" << ctx->failure << '\n';

    state = reinterpret_cast<HotReloadState*>(ctx->userdata);

    auto vkInstance = dei::render::CreateVulkanInstance(
        state->RequiredHostExtensions,
        state->RequiredHostExtensionCount);
    auto vkSurface = state->CreateVkSurfaceCallback(vkInstance);
    std::cout << "Loaded VkInstance: " << vkInstance << " VkSurfaceKHR: " << vkSurface << std::endl;

    auto extensionCount = uint32_t{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    auto c = dei::MakeCamera(-5.0f, vec2{0.0f, 0.0f});
    std::cout << c[0][0] << ' ' << c[0][1] << ' ' << c[0][2] << ' ' << c[0][3] << '\n';
    std::cout << c[1][0] << ' ' << c[1][1] << ' ' << c[1][2] << ' ' << c[1][3] << '\n';
    std::cout << c[2][0] << ' ' << c[2][1] << ' ' << c[2][2] << ' ' << c[2][3] << '\n';
    std::cout << c[3][0] << ' ' << c[3][1] << ' ' << c[3][2] << ' ' << c[3][3] << '\n';

    return 0;
}

auto OnUpdate(cr_plugin *ctx) -> int {
    ++state->DrawCounter;
    return 0;
}

auto OnUnload(cr_plugin *ctx) -> int {
    std::cout << "dei::OnUnload() v" << ctx->version << " e" << ctx->failure << '\n';
    return 0;
}

auto OnClose(cr_plugin *ctx) -> int {
    std::cout << "dei::OnClose() v" << ctx->version << '\n';
    return 0;
}

CR_EXPORT
auto cr_main(cr_plugin *ctx, cr_op operation) -> int {
    assert(ctx);
    switch (operation) {
        case CR_STEP:   return OnUpdate(ctx);
        case CR_LOAD:   return OnLoad(ctx); // loading back from a reload
        case CR_UNLOAD: return OnUnload(ctx); // preparing to a new reload
        case CR_CLOSE:  return OnClose(ctx); // the plugin will close and not reload anymore
    }
}

