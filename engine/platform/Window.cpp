#include "dei_platform/Window.hpp"

namespace {



}

namespace dei::platform {

auto CreateWindowSystem() -> std::shared_ptr<std::nullptr_t> {
    glfwInit();
    return std::shared_ptr<std::nullptr_t>(
        nullptr, [](std::nullptr_t){ glfwTerminate(); });
}

auto WindowDestroyer::operator()(GLFWwindow* window) -> void {
    if (window == nullptr) {
        return;
    }
    glfwDestroyWindow(window);
}

auto CreateWindow(CreateWindowArgs&& args) -> WindowHandle {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    auto* window = glfwCreateWindow(args.Width, args.Height, args.Title, nullptr, nullptr);
    return WindowHandle{window};
}

auto PollWindowEvents(const WindowHandle& window) -> void {
    glfwPollEvents();
}

auto IsWindowClosing(const WindowHandle& window) -> bool {
   return window == nullptr || glfwWindowShouldClose(window.get());
}

}