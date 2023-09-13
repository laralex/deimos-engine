#include "dei_platform/Window.hpp"
#include "dei_platform/Unicode.hpp"

namespace {

using namespace dei;

struct WindowState {
    platform::WindowSystemHandle WindowSystem;
    platform::input::KeyMap KeyMap;
    std::string InputTextUtf8;
    platform::WindowPositionCallback WindowPositionCallback;
    platform::WindowResizeCallback WindowResizeCallback;
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

auto WindowPositionCallback(GLFWwindow* window, int leftUpCornerX, int leftUpCornerY) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState->MouseEntersWindowCallback == nullptr) {
        return;
    }
    windowState->WindowPositionCallback(leftUpCornerX, leftUpCornerY);
}

auto WindowResizeCallback(GLFWwindow* window, int widthPx, int heightPx) {
    auto* windowState = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (windowState->MouseEntersWindowCallback == nullptr) {
        return;
    }
    windowState->WindowResizeCallback(widthPx, heightPx);
}

} // namespace

namespace dei::platform {

auto CreateWindowSystem(void (*errorCallback)(int, const char*)) -> WindowSystemHandle {
    glfwInit();
    glfwSetErrorCallback(errorCallback);
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

auto SetVerticalSync(const WindowSystemHandle&, bool enableVerticalSync) -> void {
    glfwSwapInterval(enableVerticalSync ? 1 : 0);
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

auto WindowBuilder::WithSize(size_t width, size_t height) -> WindowBuilder& {
    _args.Width = width;
    _args.Height = height;
    return *this;
}

auto WindowBuilder::WithSizeMin(size_t width, size_t height) -> WindowBuilder& {
    _args.WidthMin = width;
    _args.HeightMin = height;
    return *this;
}

auto WindowBuilder::WithSizeMax(size_t width, size_t height) -> WindowBuilder& {
    _args.WidthMax = width;
    _args.HeightMax = height;
    return *this;
}

auto WindowBuilder::WithAspectRatioForceCurrent() -> WindowBuilder& {
    _args.UseAspectRatio = true;
    _args.UseSizeAsAspectRatio = true;
    return *this;
}

auto WindowBuilder::WithAspectRatio(size_t aspectNumerator, size_t aspectDenominator) -> WindowBuilder& {
    _args.UseAspectRatio = true;
    _args.UseSizeAsAspectRatio = false;
    _args.AspectNumerator = aspectNumerator;
    _args.AspectDenominator = aspectDenominator;
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

auto WindowBuilder::WithRawMouseMotion(bool isRawMouseMotionUsed) -> WindowBuilder& {
    _args.TryRawMouseMotion = isRawMouseMotionUsed;
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

auto WindowBuilder::WithPositionCallback(WindowPositionCallback callback) -> WindowBuilder& {
    _args.WindowPositionCallback = callback;
    return *this;
}

auto WindowBuilder::WithResizeCallback(WindowResizeCallback callback) -> WindowBuilder& {
    _args.WindowResizeCallback = callback;
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
    if (args.Width == 0 || args.Height == 0) {
        return std::nullopt;
    }
    switch (args.GraphicalBackend) {
        case CreateWindowArgs::GraphicsBackend::VULKAN:
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;
        default:
            // TODO: assert / panic
            break;
    }
    auto* windowState = new WindowState{};
    windowState->WindowSystem = windowSystem;
    windowState->KeyMap = std::move(args.KeyMap);
    windowState->WindowPositionCallback = std::move(args.WindowPositionCallback);
    windowState->WindowResizeCallback = std::move(args.WindowResizeCallback);
    windowState->InputTextUtf8 = std::string{};
    windowState->InputTextCallback = std::move(args.InputTextCallback);
    windowState->MousePositionCallback = std::move(args.MousePositionCallback);
    windowState->MouseButtonCallback = std::move(args.MouseButtonCallback);
    windowState->MouseScrollCallback = std::move(args.MouseScrollCallback);
    windowState->MouseEntersWindowCallback = std::move(args.MouseEntersWindowCallback);

    auto* window = glfwCreateWindow(args.Width, args.Height, args.TitleUtf8, nullptr, nullptr);
    glfwSetWindowUserPointer(window, windowState);
    glfwSetKeyCallback(window, &::KeyboardCallback);
    glfwSetCharCallback(window, &::TextInputCallback);
    glfwSetCursorPosCallback(window, &::MousePositionCallback);
    glfwSetScrollCallback(window, &::MouseScrollCallback);
    glfwSetMouseButtonCallback(window, &::MouseButtonCallback);
    glfwSetCursorEnterCallback(window, &::MouseEntersWindowCallback);
    glfwSetWindowPosCallback(window, &::WindowPositionCallback);
    glfwSetWindowSizeCallback(window, &::WindowResizeCallback);
    glfwSetWindowSizeLimits(window,
        args.WidthMin > 0 ? args.WidthMin : GLFW_DONT_CARE,
        args.HeightMin > 0 ? args.HeightMin : GLFW_DONT_CARE,
        args.WidthMax < (1 << 30) ? args.WidthMax : GLFW_DONT_CARE,
        args.HeightMax < (1 << 30) ? args.HeightMax : GLFW_DONT_CARE);
    if (args.UseAspectRatio) {
        if (args.UseSizeAsAspectRatio) {
            args.AspectNumerator = args.Width;
            args.AspectDenominator = args.Height;
        }
        glfwSetWindowAspectRatio(window, args.AspectNumerator, args.AspectDenominator);
    }

    if (args.TryRawMouseMotion && glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
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

auto WindowSetCursorMode(const WindowHandle& window, input::CursorMode newMode) -> void {
    glfwSetInputMode(window.get(), GLFW_CURSOR, static_cast<int>(newMode));
}

auto WindowGetCursorMode(const WindowHandle& window) -> input::CursorMode {
    return static_cast<input::CursorMode>(glfwGetInputMode(window.get(), GLFW_CURSOR));
}

auto WindowSetSizeMode(const WindowHandle& window, WindowSizeMode mode) -> void {
    switch (mode) {
    case WindowSizeMode::NORMAL:
        glfwRestoreWindow(window.get());
        break;
    case WindowSizeMode::MINIMIZED:
        glfwIconifyWindow(window.get());
        break;
    case WindowSizeMode::MAXIMIZED:
        glfwMaximizeWindow(window.get());
        break;
    }
}

auto WindowGetSizeMode(const WindowHandle& window) -> WindowSizeMode {
    if (glfwGetWindowAttrib(window.get(), GLFW_ICONIFIED) > 0) {
        return WindowSizeMode::MINIMIZED;
    } else if (glfwGetWindowAttrib(window.get(), GLFW_MAXIMIZED) > 0) {
        return WindowSizeMode::MAXIMIZED;
    }
    return WindowSizeMode::NORMAL;
}

}