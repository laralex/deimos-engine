#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stddef.h>
#include <memory>

namespace dei::platform {

auto CreateWindowSystem() -> std::shared_ptr<std::nullptr_t>;

struct CreateWindowArgs {
   size_t Width;
   size_t Height;
   const char* Title;
};

struct WindowDestroyer {
   auto operator()(GLFWwindow* window) -> void;
};
using WindowHandle = std::unique_ptr<GLFWwindow, WindowDestroyer>;
auto CreateWindow(CreateWindowArgs&&) -> WindowHandle;
auto PollWindowEvents(const WindowHandle&) -> void;
auto IsWindowClosing(const WindowHandle&) -> bool;

} // dei::platform