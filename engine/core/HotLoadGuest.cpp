#include "dei/Camera.hpp"

#include <cr.h>

#include <iostream>
#include <vulkan/vulkan.hpp>
// #include <dlfcn.h>

// class EngineInstance {
// public: 
//     typedef auto (*dei_camera)(f32 Translation, vec2 const& Rotation) -> mat4;
//     static dei_camera camera;

//     static void Open() {
//         _LibraryHandle = dlopen("./build/libdei.so", RTLD_LAZY);
//         if (!_LibraryHandle) {
//             fputs (dlerror(), stderr);
//             exit(1);
//         }

//         camera = (dei_camera)dlsym(_LibraryHandle, "camera");

//         char* error;
//         if ((error = dlerror()) != NULL)  {
//             fputs(error, stderr);
//             exit(1);
//         }
//     }

//     static void Close() {
//         dlclose(_LibraryHandle);
//     }

// private:
//     static void* _LibraryHandle;
// };

// void* EngineInstance::_LibraryHandle{nullptr};
// EngineInstance::dei_camera EngineInstance::camera{nullptr};

struct HotReloadState {
    u32 DrawCounter{0};
};

static HotReloadState* state;

int OnLoad(cr_plugin *ctx) {
    std::cout << "dei::OnLoad() v" << ctx->version << " e" << ctx->failure << '\n';

    state = reinterpret_cast<HotReloadState*>(ctx->userdata);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    mat4 test = dei::camera(-5.0f, vec2{0.0f, 0.0f});
    std::cout << test[0][0] << ' ' << test[0][1] << ' ' << test[0][2] << ' ' << test[0][3] << '\n';
    std::cout << test[1][0] << ' ' << test[1][1] << ' ' << test[1][2] << ' ' << test[1][3] << '\n';
    std::cout << test[2][0] << ' ' << test[2][1] << ' ' << test[2][2] << ' ' << test[2][3] << '\n';
    std::cout << test[3][0] << ' ' << test[3][1] << ' ' << test[3][2] << ' ' << test[3][3] << '\n';

    return 0;
}

int OnUpdate(cr_plugin *ctx) {
    if (state->DrawCounter++ % 50000 == 0) {
        std::cout << "dei::OnUpdate() v" << ctx->version << " f=" << state->DrawCounter << '\n';
    }
    return 0;
}

int OnUnload(cr_plugin *ctx) {
    std::cout << "dei::OnUnload() v" << ctx->version << " e" << ctx->failure << '\n';
    return 0;
}

int OnClose(cr_plugin *ctx) {
    std::cout << "dei::OnClose() v" << ctx->version << '\n';
    return 0;
}

CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation) {
    assert(ctx);
    switch (operation) {
        case CR_STEP:   return OnUpdate(ctx);
        case CR_LOAD:   return OnLoad(ctx); // loading back from a reload
        case CR_UNLOAD: return OnUnload(ctx); // preparing to a new reload
        case CR_CLOSE:  return OnClose(ctx); // the plugin will close and not reload anymore
    }
}

