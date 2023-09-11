#include "dei_platform/Window.hpp"
#include "dei_platform/Unicode.hpp"

namespace {

using namespace dei;

struct WindowState {
    platform::WindowSystemHandle WindowSystem;
    platform::input::KeyMap KeyMap;
    std::string InputTextUtf8;
    platform::input::InputTextCallback InputTextCallback;
};

auto IsKeyPressed(GLFWwindow* window, int key, int keyAlias) -> bool {
    return glfwGetKey(window, key) == GLFW_PRESS
        || glfwGetKey(window, keyAlias) == GLFW_PRESS;
}

auto KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
    using platform::input::KeyCode;
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    auto keyCode = static_cast<KeyCode>(key);
    auto keyName = glfwGetKeyName(key, scancode);
    auto modifierKeysState = platform::input::ModifierKeysState{};
    if (mods & GLFW_MOD_CONTROL) {
        modifierKeysState |= platform::input::MODIFIERS_CTRL;
    }
    if (mods & GLFW_MOD_SHIFT) {
        modifierKeysState |= platform::input::MODIFIERS_SHIFT;
    }
    if (mods & GLFW_MOD_ALT) {
        modifierKeysState |= platform::input::MODIFIERS_ALT;
    }
    auto foundAction = windowState->KeyMap.find({keyCode, modifierKeysState});
    if (foundAction == windowState->KeyMap.end()) {
        // fallback to default if present
        foundAction = windowState->KeyMap.find({
            platform::input::KeyCode::ANYTHING,
            platform::input::MODIFIERS_NONE,
        });
    }
    if (foundAction == windowState->KeyMap.end()) {
        return;
    }
    foundAction->second.operator()(
        keyCode,
        static_cast<platform::input::KeyState>(action),
        keyName
    );
}

auto TextInputCallback(GLFWwindow* window, uint32_t codepoint) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    auto isAppended = dei::platform::AppendToUtf8{}(windowState->InputTextUtf8, codepoint);
    if (isAppended && windowState->InputTextCallback != nullptr) {
        windowState->InputTextCallback(windowState->InputTextUtf8, codepoint);
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

auto GetKeyName(input::KeyCode key) -> const char* {
    return glfwGetKeyName(static_cast<int>(key), 0);
}

auto WindowDestroyer::operator()(GLFWwindow* window) -> void {
    if (window == nullptr) {
        return;
    }
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState != nullptr) {
        delete windowState;
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

auto WindowBuilder::WithKeymap(platform::input::KeyMap&& keymap) -> WindowBuilder& {
    _args.KeyMap = std::move(keymap);
    return *this;
}

auto WindowBuilder::WithInputTextCallback(input::InputTextCallback callback) -> WindowBuilder& {
    _args.InputTextCallback = callback;
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
    auto* windowState = new WindowState{
        windowSystem,
        std::move(args.KeyMap),
        std::string{},
        std::move(args.InputTextCallback)
    };
    auto* window = glfwCreateWindow(args.Width, args.Height, args.TitleUtf8, nullptr, nullptr);
    glfwSetWindowUserPointer(window, windowState);
    glfwSetKeyCallback(window, &::KeyboardCallback);
    glfwSetCharCallback(window, &::TextInputCallback);
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

auto WindowSetKeyMap(const WindowHandle& window, platform::input::KeyMap&& keymap) -> void {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
    windowState->KeyMap = std::move(keymap);
}

auto WindowSwapBuffers(const WindowHandle& window) -> void {
    glfwSwapBuffers(window.get());
}

auto WindowClearInput(const WindowHandle& window) -> void {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
    windowState->InputTextUtf8.clear();
}

auto WindowUndoInput(const WindowHandle& window) -> void {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
    if (windowState->InputTextUtf8.size() == 0) {
        return;
    }
    windowState->InputTextUtf8.resize(windowState->InputTextUtf8.size() - 1);
}

}