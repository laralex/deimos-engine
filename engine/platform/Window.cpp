#include "dei_platform/Window.hpp"

namespace {

typedef void (*WindowKeyboardCallback)(GLFWwindow* window, int key, int scancode, int action, int mods);

using namespace dei::platform;
struct WindowState {
    WindowSystemHandle WindowSystem;
    KeyMap KeyMap;
};

auto KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
    auto windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    auto keyCode = static_cast<KeyCode>(key);
    auto foundAction = windowState->KeyMap.find(keyCode);
    if (foundAction != windowState->KeyMap.end()) {
        foundAction->second.operator()(
            static_cast<KeyCode>(key),
            static_cast<KeyState>(action)
        );
        return;
    }
    // fallback to default if present
    foundAction = windowState->KeyMap.find(KeyCode::ANYTHING);
    if (foundAction != windowState->KeyMap.end()) {
        foundAction->second.operator()(
            static_cast<KeyCode>(key),
            static_cast<KeyState>(action)
        );
    }
        
}

} // namespace

namespace dei::platform {

auto CreateWindowSystem() -> WindowSystemHandle {
    glfwInit();
    return WindowSystemHandle(
        nullptr, [](std::nullptr_t){ glfwTerminate(); });
}

auto PollWindowEvents(const WindowSystemHandle& window) -> void {
    glfwPollEvents();
}

auto WindowDestroyer::operator()(GLFWwindow* window) -> void {
    if (window == nullptr) {
        return;
    }
    glfwDestroyWindow(window);
}

auto WindowBuilder::WithDimensions(size_t width, size_t height) -> WindowBuilder& {
    _args.Width = width;
    _args.Height = height;
    return *this;
}

auto WindowBuilder::WithTitleUtf8(const char* titleUtf8) -> WindowBuilder& {
    _args.TitleUtf8 = titleUtf8;
    return *this;
}

auto WindowBuilder::WithGraphicsBackend(CreateWindowArgs::GraphicsBackend graphicsBackend) -> WindowBuilder& {
    _args.GraphicalBackend = graphicsBackend;
    return *this;
}

auto WindowBuilder::WithKeymap(KeyMap&& keymap) -> WindowBuilder& {
    _args.KeyMap = std::move(keymap);
    return *this;
}

auto WindowBuilder::IsValid() const -> bool {
    return _args.Height > 0 && _args.Width > 0
           && _args.GraphicalBackend == CreateWindowArgs::GraphicsBackend::VULKAN;
}

auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle> {
    return CreateWindow(windowSystem, std::move(builder._args));
}

auto CreateWindow(const WindowSystemHandle& windowSystem, CreateWindowArgs&& args) -> std::optional<WindowHandle> {
    switch (args.GraphicalBackend) {
        case CreateWindowArgs::GraphicsBackend::VULKAN:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;
        default:
            // TODO: assert / panic
            break;
    }
    auto* windowState = new WindowState{windowSystem, std::move(args.KeyMap)};
    auto* window = glfwCreateWindow(args.Width, args.Height, args.TitleUtf8, nullptr, nullptr);
    glfwSetWindowUserPointer(window, windowState);
    glfwSetKeyCallback(window, &::KeyboardCallback);
    return WindowHandle{window};
}

auto WindowIsClosing(const WindowHandle& window) -> bool {
    return glfwWindowShouldClose(window.get());
}

auto WindowGetSize(const WindowHandle& window) -> size2i {
    auto size = size2i{};
    glfwGetWindowSize(window.get(), &size.width, &size.height);
    return size;
}

auto WindowSetTitleUtf8(const WindowHandle& window, const char* titleUtf8) -> void {
    glfwSetWindowTitle(window.get(), titleUtf8);
}

auto WindowSetKeyMap(const WindowHandle& window, KeyMap&& keymap) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
    windowState->KeyMap = std::move(keymap);
}

}