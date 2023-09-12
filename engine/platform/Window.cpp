#include "dei_platform/Window.hpp"
#include "dei_platform/Unicode.hpp"

namespace {

using namespace dei;

struct WindowState {
    platform::WindowSystemHandle WindowSystem;
    platform::input::KeyMap KeyMap;
    std::string InputTextUtf8;
    platform::input::InputTextCallback InputTextCallback;
    platform::input::MousePositionCallback MousePositionCallback;
    platform::input::MouseButtonCallback MouseButtonCallback;
    platform::input::MouseScrollCallback MouseScrollCallback;
    platform::input::MouseEntersWindowCallback MouseEntersWindowCallback;
};

auto IsKeyPressed(GLFWwindow* window, int key, int keyAlias) -> bool {
    return glfwGetKey(window, key) == GLFW_PRESS
        || glfwGetKey(window, keyAlias) == GLFW_PRESS;
}

auto KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
    using platform::input::KeyCode;
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    printf("@ %d %d", key, scancode);
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
    if (!isAppended || windowState->InputTextCallback == nullptr) {
        return;
    }
    windowState->InputTextCallback(windowState->InputTextUtf8, codepoint);
}

auto MousePositionCallback(GLFWwindow* window, double windowX, double windowY) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState->MousePositionCallback == nullptr) {
        return;
    }
    windowState->MousePositionCallback(windowX, windowY);
}

auto MouseScrollCallback(GLFWwindow* window, double directionX, double directionY) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState->MouseScrollCallback == nullptr) {
        return;
    }
    windowState->MouseScrollCallback(directionX, directionY);
}

auto MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState->MouseButtonCallback == nullptr) {
        return;
    }
    windowState->MouseButtonCallback(
        static_cast<dei::platform::input::MouseButton>(button),
        static_cast<dei::platform::input::MouseButtonState>(action)
    );
}

auto MouseEntersWindowCallback(GLFWwindow* window, int entered) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState->MouseEntersWindowCallback == nullptr) {
        return;
    }
    windowState->MouseEntersWindowCallback(entered != 0);
}

} // namespace

namespace dei::platform {

auto CreateWindowSystem() -> WindowSystemHandle {
    glfwInit();
#if NDEBUG == 0
    printf("GLFW version: %s\n", glfwGetVersionString());
#endif
    return WindowSystemHandle(
        nullptr, [](std::nullptr_t){ glfwTerminate(); });
}

auto PollWindowEvents(const WindowSystemHandle& window) -> void {
    glfwPollEvents();
}

auto GetKeyName(input::KeyCode key) -> const char* {
    return glfwGetKeyName(static_cast<int>(key), 0);
}

auto GetClipboardUtf8(const WindowSystemHandle&) -> const char * {
    return glfwGetClipboardString(NULL);
}

auto SetClipboardUtf8(const WindowSystemHandle&, const char* textUtff8) -> void {
    glfwSetClipboardString(NULL, textUtff8);
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

auto WindowBuilder::WithMousePositionCallback(input::MousePositionCallback callback) -> WindowBuilder& {
    _args.MousePositionCallback = callback;
    return *this;
}

auto WindowBuilder::WithMouseButtonCallback(input::MouseButtonCallback callback) -> WindowBuilder& {
    _args.MouseButtonCallback = callback;
    return *this;
}

auto WindowBuilder::WithMouseScrollCallback(input::MouseScrollCallback callback) -> WindowBuilder& {
    _args.MouseScrollCallback = callback;
    return *this;
}

auto WindowBuilder::WithMouseEntersWindowCallback(input::MouseEntersWindowCallback callback) -> WindowBuilder& {
    _args.MouseEntersWindowCallback = callback;
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
        std::move(args.InputTextCallback),
        std::move(args.MousePositionCallback),
        std::move(args.MouseButtonCallback),
        std::move(args.MouseScrollCallback),
        std::move(args.MouseEntersWindowCallback),
    };
    auto* window = glfwCreateWindow(args.Width, args.Height, args.TitleUtf8, nullptr, nullptr);
    glfwSetWindowUserPointer(window, windowState);
    glfwSetKeyCallback(window, &::KeyboardCallback);
    glfwSetCharCallback(window, &::TextInputCallback);
    glfwSetCursorPosCallback(window, &::MousePositionCallback);
    glfwSetScrollCallback(window, &::MouseScrollCallback);
    glfwSetMouseButtonCallback(window, &::MouseButtonCallback);
    glfwSetCursorEnterCallback(window, &::MouseEntersWindowCallback);

    return WindowHandle{window};
}

auto WindowIsClosing(const WindowHandle& window) -> bool {
    return glfwWindowShouldClose(window.get());
}

auto WindowGetSize(const WindowHandle& window) -> isize2 {
    auto size = isize2{};
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

auto WindowAppendInputUtf8(const WindowHandle& window, const char* textUtf8) -> void {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
    windowState->InputTextUtf8.append(textUtf8);
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

auto WindowGetMousePosition(const WindowHandle& window) -> dvec2 {
    dvec2 pos;
    glfwGetCursorPos(window.get(), &pos.x, &pos.y);
    return pos;
}

auto WindowGetMousePosition(const WindowHandle& window, dvec2& destination) -> void {
    glfwGetCursorPos(window.get(), &destination.x, &destination.y);
}

}