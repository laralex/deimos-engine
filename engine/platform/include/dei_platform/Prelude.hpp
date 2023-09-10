#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#if defined(_WIN32)
#define DEI_WINDOWS
#elif defined(__linux__)
#define DEI_LINUX
#elif defined(__APPLE__)
#define DEI_OSX
#else
#error "Unsupported platform"
#endif // DEI_WINDOWS || DEI_LINUX || DEI_OSX

namespace dei::platform {

enum class GraphicsApi {
   OPENGL_33,
   VULKAN_10,
   DIRECTX_11
};

struct vec2i {
   int x, y;
};

struct size2i {
   int width, height;
};

using WindowSystemHandle = std::shared_ptr<std::nullptr_t>;

struct WindowDestroyer {
   auto operator()(GLFWwindow* window) -> void;
};
using WindowHandle = std::unique_ptr<GLFWwindow, WindowDestroyer>;

};
