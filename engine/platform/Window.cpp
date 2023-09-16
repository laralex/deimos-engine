#include "dei_platform/Window.hpp"
#include "dei_platform/Unicode.hpp"

namespace {

using namespace dei;

struct WindowState {
    platform::isize2 Size;
    platform::isize2 DesktopResolution;
    platform::WindowSystemHandle WindowSystem;
    platform::input::KeyMap KeyMap;
    std::string InputTextUtf8;
    platform::GraphicsBackend GraphicsBackend;
    bool HasContextObject;
    platform::WindowPositionCallback WindowPositionCallback;
    platform::WindowResizeCallback WindowResizeCallback;
    platform::WindowClosingCallback WindowClosingCallback;
    platform::WindowFocusedCallback WindowFocusedCallback;
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

inline auto GetWindowState(const dei::platform::WindowHandle& window) -> WindowState* {
    return static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
}

inline auto GetWindowState(GLFWwindow* window) -> WindowState* {
    return static_cast<WindowState*>(glfwGetWindowUserPointer(window));
}

auto KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
    using platform::input::KeyCode;
    auto* windowState = GetWindowState(window);;
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
    auto* windowState = GetWindowState(window);
    auto isAppended = dei::platform::AppendToUtf8{}(windowState->InputTextUtf8, codepoint);
    if (!isAppended || windowState->InputTextCallback == nullptr) {
        return;
    }
    windowState->InputTextCallback(windowState->InputTextUtf8, codepoint);
}

auto MousePositionCallback(GLFWwindow* window, double windowX, double windowY) {
    auto* windowState = GetWindowState(window);
    if (windowState->MousePositionCallback == nullptr) {
        return;
    }
    windowState->MousePositionCallback(windowX, windowY);
}

auto MouseScrollCallback(GLFWwindow* window, double directionX, double directionY) {
    auto* windowState = GetWindowState(window);
    if (windowState->MouseScrollCallback == nullptr) {
        return;
    }
    windowState->MouseScrollCallback(directionX, directionY);
}

auto MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* windowState = GetWindowState(window);
    if (windowState->MouseButtonCallback == nullptr) {
        return;
    }
    windowState->MouseButtonCallback(
        static_cast<dei::platform::input::MouseButton>(button),
        static_cast<dei::platform::input::MouseButtonState>(action)
    );
}

auto MouseEntersWindowCallback(GLFWwindow* window, int entered) {
    auto* windowState = GetWindowState(window);
    if (windowState->MouseEntersWindowCallback == nullptr) {
        return;
    }
    windowState->MouseEntersWindowCallback(entered != 0);
}

auto WindowPositionCallback(GLFWwindow* window, int leftUpCornerX, int leftUpCornerY) {
    auto* windowState = GetWindowState(window);
    if (windowState->MouseEntersWindowCallback == nullptr) {
        return;
    }
    windowState->WindowPositionCallback(leftUpCornerX, leftUpCornerY);
}

auto WindowResizeCallback(GLFWwindow* window, int widthPx, int heightPx) {
    auto* windowState = GetWindowState(window);
    if (windowState->MouseEntersWindowCallback == nullptr) {
        return;
    }
    windowState->WindowResizeCallback(widthPx, heightPx);
}

auto WindowClosingCallback(GLFWwindow* window) {
    auto* windowState = GetWindowState(window);
    if (windowState->WindowClosingCallback == nullptr) {
        return;
    }
    windowState->WindowClosingCallback();
}

auto WindowFocusedCallback(GLFWwindow* window, int isFocused) {
    auto* windowState = GetWindowState(window);
    if (windowState->WindowFocusedCallback == nullptr) {
        return;
    }
    windowState->WindowFocusedCallback(isFocused != 0);
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

auto GetKeyName(const WindowSystemHandle&, input::KeyCode key) -> const char* {
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
    _args.Size = { (int)width, (int)height };
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

auto WindowBuilder::WithVisible(bool isVisible) -> WindowBuilder& {
    _args.IsVisible = isVisible;
    return *this;
}

auto WindowBuilder::WithFocus(bool isWindowFocused) -> WindowBuilder& {
    _args.IsFocused = isWindowFocused;
    return *this;
}

auto WindowBuilder::WithFocusCallback(WindowFocusedCallback callback) -> WindowBuilder& {
    _args.WindowFocusedCallback = callback;
    return *this;
}

auto WindowBuilder::WithGraphicsBackend(GraphicsBackend graphicsBackend) -> WindowBuilder& {
    _args.GraphicsBackend = graphicsBackend;
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

auto WindowBuilder::WithClosingCallback(WindowClosingCallback callback) -> WindowBuilder& {
    _args.WindowClosingCallback = callback;
    return *this;
}

auto WindowBuilder::WithFullscreen(const MonitorHandle& monitor) -> WindowBuilder& {
    if (monitor == nullptr) {
        return WithWindowed();
    }
    _args.Monitor = monitor;
    _args.FullscreenMode = FullscreenMode::FULLSCREEN;
    return *this;
}

auto WindowBuilder::WithWindowed() -> WindowBuilder& {
    _args.Monitor = nullptr;
    _args.FullscreenMode = FullscreenMode::WINDOWED;
    return *this;
}

auto WindowBuilder::WithWindowedBorderless() -> WindowBuilder& {
    _args.Monitor = nullptr;
    _args.FullscreenMode = FullscreenMode::WINDOWED_BORDERLESS;
    return *this;
}

auto WindowBuilder::IsValid() const -> bool {
    return _args.Size.width > 0 && _args.Size.height > 0
           && _args.GraphicsBackend == GraphicsBackend::VULKAN;
}

auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle> {
    return CreateWindow(windowSystem, std::move(builder._args));
}

auto CreateWindow(const WindowSystemHandle& windowSystem, CreateWindowArgs&& args) -> std::optional<WindowHandle> {
    if (args.Size.width == 0 || args.Size.height == 0) {
        return std::nullopt;
    }
    auto* windowState = new WindowState{};
    windowState->HasContextObject = false;
    switch (args.GraphicsBackend) {
        case GraphicsBackend::VULKAN:
            if (glfwVulkanSupported() == false) {
                printf("Can't use Vulkan on this device");
                std::exit(1);
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            break;
        default:
            windowState->HasContextObject = true;
            printf("Unsupported GraphicsBackend value=%d (%s)",
                args.GraphicsBackend,
                GraphicsBackendToStr(args.GraphicsBackend));
            std::exit(1);
            // TODO: assert / panic
            break;
    }
    auto primaryVideoMode = glfwGetVideoMode(args.Monitor != nullptr ? args.Monitor : glfwGetPrimaryMonitor());
    windowState->DesktopResolution = isize2 { primaryVideoMode->width, primaryVideoMode->height };
    windowState->Size = args.Size;
    windowState->GraphicsBackend = args.GraphicsBackend;
    windowState->WindowSystem = windowSystem;
    windowState->KeyMap = std::move(args.KeyMap);
    windowState->WindowPositionCallback = std::move(args.WindowPositionCallback);
    windowState->WindowResizeCallback = std::move(args.WindowResizeCallback);
    windowState->WindowClosingCallback = std::move(args.WindowClosingCallback);
    windowState->WindowFocusedCallback = std::move(args.WindowFocusedCallback);
    windowState->InputTextUtf8 = std::string{};
    windowState->InputTextCallback = std::move(args.InputTextCallback);
    windowState->MousePositionCallback = std::move(args.MousePositionCallback);
    windowState->MouseButtonCallback = std::move(args.MouseButtonCallback);
    windowState->MouseScrollCallback = std::move(args.MouseScrollCallback);
    windowState->MouseEntersWindowCallback = std::move(args.MouseEntersWindowCallback);

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, args.IsVisible ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_RED_BITS, primaryVideoMode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, primaryVideoMode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, primaryVideoMode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, primaryVideoMode->refreshRate);
    // FIXME: investigate why it still steals focus when passing FALSE
    glfwWindowHint(GLFW_FOCUSED, args.IsFocused ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, args.IsFocused ? GLFW_TRUE : GLFW_FALSE);

    auto* window = glfwCreateWindow(
        args.Size.width, args.Size.height, args.TitleUtf8, args.Monitor, nullptr);
    glfwSetWindowUserPointer(window, windowState);
    glfwSetKeyCallback(window, &::KeyboardCallback);
    glfwSetCharCallback(window, &::TextInputCallback);
    glfwSetCursorPosCallback(window, &::MousePositionCallback);
    glfwSetScrollCallback(window, &::MouseScrollCallback);
    glfwSetMouseButtonCallback(window, &::MouseButtonCallback);
    glfwSetCursorEnterCallback(window, &::MouseEntersWindowCallback);
    glfwSetWindowPosCallback(window, &::WindowPositionCallback);
    glfwSetFramebufferSizeCallback(window, &::WindowResizeCallback);
    glfwSetWindowCloseCallback(window, &::WindowClosingCallback);
    glfwSetWindowFocusCallback(window, &::WindowFocusedCallback);
    glfwSetWindowSizeLimits(window,
        args.WidthMin > 0 ? args.WidthMin : GLFW_DONT_CARE,
        args.HeightMin > 0 ? args.HeightMin : GLFW_DONT_CARE,
        args.WidthMax < (1 << 30) ? args.WidthMax : GLFW_DONT_CARE,
        args.HeightMax < (1 << 30) ? args.HeightMax : GLFW_DONT_CARE);
    if (args.UseAspectRatio) {
        if (args.UseSizeAsAspectRatio) {
            args.AspectNumerator = args.Size.width;
            args.AspectDenominator = args.Size.height;
        }
        glfwSetWindowAspectRatio(window, args.AspectNumerator, args.AspectDenominator);
    }
    if (args.TryRawMouseMotion && glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    auto windowHandle = WindowHandle{window};
    WindowBindToThread(windowHandle);
    return std::move(windowHandle);
}

auto WindowIsClosing(const WindowHandle& window) -> bool {
    return glfwWindowShouldClose(window.get());
}

auto WindowGetSize(const WindowHandle& window) -> isize2 {
    auto size = isize2{};
    glfwGetWindowSize(window.get(), &size.width, &size.height);
    return size;
}

auto WindowSetSize(const WindowHandle& window, isize2 size) -> void {
    glfwSetWindowSize(window.get(), size.width, size.height);
}

auto WindowSetTitleUtf8(const WindowHandle& window, const char* titleUtf8) -> void {
    glfwSetWindowTitle(window.get(), titleUtf8);
}

auto WindowSetKeyMap(const WindowHandle& window, platform::input::KeyMap&& keymap) -> void {
    GetWindowState(window)->KeyMap = std::move(keymap);
}

auto WindowSwapBuffers(const WindowHandle& window) -> void {
    // if (GetWindowState(window)->HasContextObject == false) {
    //     return;
    // }
    glfwSwapBuffers(window.get());
}

auto WindowAppendInputUtf8(const WindowHandle& window, const char* textUtf8) -> void {
    GetWindowState(window)->InputTextUtf8.append(textUtf8);
}

auto WindowClearInput(const WindowHandle& window) -> void {
    GetWindowState(window)->InputTextUtf8.clear();
}

auto WindowUndoInput(const WindowHandle& window) -> void {
    auto* windowState = GetWindowState(window);
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

auto WindowInitializeVulkanBackend(const WindowHandle& window, VkInstance vkInstance) -> std::optional<VkSurfaceKHR> {
    if (GetWindowState(window)->GraphicsBackend != GraphicsBackend::VULKAN) {
        return std::nullopt;
    }
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(vkInstance, window.get(), NULL, &surface);
    if (err) {
        return std::nullopt;
    }
    return surface;
}

auto WindowBindToThread(const WindowHandle& window) -> void {
    if (GetWindowState(window)->GraphicsBackend != GraphicsBackend::OPENGL) {
        return;
    }
    glfwMakeContextCurrent(window.get());
}

auto WindowUnbindFromThread(const WindowHandle& window) -> void {
    glfwMakeContextCurrent(NULL);
}

auto WindowToFullscreen(const WindowHandle& window, const MonitorHandle& monitor) -> void {
    if (monitor == nullptr) {
        return;
    }
    auto videoMode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window.get(), monitor, 0, 0,
        videoMode->width, videoMode->height, videoMode->refreshRate);
}

auto WindowToWindowed(const WindowHandle& window) -> void {
    auto windowSize = GetWindowState(window)->Size;
    ivec2 windowPos;
    // FIXME: after first switch to fullscreen, windowPos will be zeros,
    // i.e. it won't remember old window position
    glfwGetWindowPos(window.get(), &windowPos.x, &windowPos.y);
    glfwSetWindowMonitor(window.get(), nullptr, windowPos.x, windowPos.y,
        windowSize.width, windowSize.height, 0);
}

auto WindowToWindowedBorderless(const WindowHandle& window) -> void {
    auto desktopResolution = GetWindowState(window)->DesktopResolution;
    glfwSetWindowMonitor(window.get(), nullptr, 0, 0,
        desktopResolution.width, desktopResolution.height, 0);
}

auto WindowSetFullscreenMode(const WindowHandle& window, FullscreenMode fullscreenMode, const MonitorHandle& monitor) -> void {
    switch (fullscreenMode) {
        case FullscreenMode::FULLSCREEN:
            WindowToFullscreen(window, monitor);
            break;
        case FullscreenMode::WINDOWED:
            WindowToWindowed(window);
            break;
        case FullscreenMode::WINDOWED_BORDERLESS:
            WindowToWindowedBorderless(window);
            break;
    }
}

auto WindowSetVisible(const WindowHandle& window, bool makeVisible) -> void {
    if (makeVisible) {
        glfwShowWindow(window.get());
    } else {
        glfwHideWindow(window.get());
    }
}

auto WindowIsVisible(const WindowHandle& window) -> bool {
    return glfwGetWindowAttrib(window.get(), GLFW_VISIBLE) == GLFW_TRUE;
}

auto WindowIsFocused(const WindowHandle& window) -> bool {
    return glfwGetWindowAttrib(window.get(), GLFW_FOCUSED) == GLFW_TRUE;
}

}