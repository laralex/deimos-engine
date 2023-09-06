#include "dei_platform/Window.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace dei::platform {

static GLFWwindow* s_CurrentWindow;

bool CreateWindow(size_t width, size_t height, const char* title) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    s_CurrentWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
    return s_CurrentWindow != nullptr;
}

void DestroyWindow() {
    if (s_CurrentWindow != nullptr) {
      glfwDestroyWindow(s_CurrentWindow);
    }
    glfwTerminate();
}

void PollWindowEvents() {
    glfwPollEvents();
}

bool IsWindowClosing() {
   return s_CurrentWindow == nullptr || glfwWindowShouldClose(s_CurrentWindow);
}

}