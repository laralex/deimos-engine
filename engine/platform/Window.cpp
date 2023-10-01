#include "dei_platform/Window.hpp"
#include "dei_platform/Unicode.hpp"

namespace {

using namespace dei;

struct WindowState {
    vec2i Size;
    vec2i MonitorSize;
    platform::WindowSystemHandle WindowSystem;
    platform::input::KeyMap KeyMap;
    std::string InputTextUtf8;
    platform::GraphicsApi GraphicsApi;
    b8 HasContextObject;
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

inline auto GetWindowState(const dei::platform::WindowHandle& window) -> WindowState* {
    return static_cast<WindowState*>(glfwGetWindowUserPointer(window.get()));
}

inline auto GetWindowState(GLFWwindow* window) -> WindowState* {
    return static_cast<WindowState*>(glfwGetWindowUserPointer(window));
}

inline auto SetGlfwVersionHint(u32 versionMajor, u32 versionMinor) -> void {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(versionMajor));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(versionMinor));
}

auto KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
    using platform::input::KeyCode;
    auto* windowState = GetWindowState(window);
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

auto TextInputCallback(GLFWwindow* window, u32 codepoint) {
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
    (void)mods;
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
#if defined(NDEBUG)
    printf("GLFW version: %s\n", glfwGetVersionString());
#endif
    return WindowSystemHandle(
        nullptr, [](std::nullptr_t){ glfwTerminate(); });
}

auto PollWindowEvents(const WindowSystemHandle&) -> void {
    glfwPollEvents();
}

auto GetKeyName(const WindowSystemHandle&, input::KeyCode key) -> const char* {
    return glfwGetKeyName(static_cast<int>(key), 0);
}

auto GetClipboardUtf8(const WindowSystemHandle&) -> const char * {
    return glfwGetClipboardString(nullptr);
}

auto SetClipboardUtf8(const WindowSystemHandle&, const char* textUtff8) -> void {
    glfwSetClipboardString(nullptr, textUtff8);
}

auto SetVerticalSync(const WindowSystemHandle&, b8 enableVerticalSync) -> void {
    glfwSwapInterval(static_cast<int>(enableVerticalSync));
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

auto WindowBuilder::WithSize(u32 width, u32 height) -> WindowBuilder& {
    _args.Size = { static_cast<int>(width), static_cast<int>(height) };
    return *this;
}

auto WindowBuilder::WithSizeMin(u32 width, u32 height) -> WindowBuilder& {
    _args.WidthMin = width;
    _args.HeightMin = height;
    return *this;
}

auto WindowBuilder::WithSizeMax(u32 width, u32 height) -> WindowBuilder& {
    _args.WidthMax = width;
    _args.HeightMax = height;
    return *this;
}

auto WindowBuilder::WithAspectRatioForceCurrent() -> WindowBuilder& {
    _args.UseAspectRatio = true;
    _args.UseSizeAsAspectRatio = true;
    return *this;
}

auto WindowBuilder::WithAspectRatio(u32 aspectNumerator, u32 aspectDenominator) -> WindowBuilder& {
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

auto WindowBuilder::WithVisible(b8 isVisible) -> WindowBuilder& {
    _args.IsVisible = isVisible;
    return *this;
}

auto WindowBuilder::WithResizable(b8 isResizable) -> WindowBuilder& {
    _args.IsResizable = isResizable;
    return *this;
}

auto WindowBuilder::WithDecorated(b8 isDecorated) -> WindowBuilder& {
    _args.IsDecorated = isDecorated;
    return *this;
}

auto WindowBuilder::WithAutoMinimized(b8 isAutoMinimized) -> WindowBuilder& {
    _args.IsAutoMinimized = isAutoMinimized;
    return *this;
}
auto WindowBuilder::WithTopmost(b8 isTopmost) -> WindowBuilder& {
    _args.IsTopmost = isTopmost;
    return *this;
}

auto WindowBuilder::WithScaleToMonitor(b8 isScaleToMonitor) -> WindowBuilder& {
    _args.IsScaleToMonitor = isScaleToMonitor;
    return *this;
}

auto WindowBuilder::WithFocused(b8 isWindowFocused) -> WindowBuilder& {
    _args.IsFocused = isWindowFocused;
    return *this;
}

auto WindowBuilder::WithFocusCallback(WindowFocusedCallback callback) -> WindowBuilder& {
    _args.WindowFocusedCallback = callback;
    return *this;
}

auto WindowBuilder::WithVulkan(u32 versionMajor, u32 versionMinor) -> WindowBuilder& {
    _args.GraphicsApi = GraphicsApi::VULKAN;
    _args.VersionMajor = versionMajor;
    _args.VersionMinor = versionMinor;
    _args.VersionRevision = 0;
    return *this;
}

auto WindowBuilder::WithOpenGL(u32 versionMajor, u32 versionMinor, ContextCreationApi api) -> WindowBuilder& {
    _args.GraphicsApi = GraphicsApi::OPENGL;
    _args.VersionMajor = versionMajor;
    _args.VersionMinor = versionMinor;
    _args.VersionRevision = 0;
    _args.ContextCreationApi = api;
    return *this;
}

auto WindowBuilder::WithOpenGLES(u32 versionMajor, u32 versionMinor, ContextCreationApi api) -> WindowBuilder& {
    _args.GraphicsApi = GraphicsApi::OPENGLES;
    _args.VersionMajor = versionMajor;
    _args.VersionMinor = versionMinor;
    _args.VersionRevision = 0;
    _args.ContextCreationApi = api;
    return *this;
}

auto WindowBuilder::WithOpenGLSettings(OpenGlProfile openglProfile, b8 isForwardCompatible, b8 isDebugMode) -> WindowBuilder& {
    _args.OpenGlProfile = openglProfile;
    _args.IsOpenGlForwardCompatible = isForwardCompatible;
    _args.IsOpenGlDebugMode = isDebugMode;
    return *this;
}

auto WindowBuilder::WithKeymap(platform::input::KeyMap&& keymap) -> WindowBuilder& {
    _args.KeyMap = std::move(keymap);
    return *this;
}

auto WindowBuilder::WithRawMouseMotion(b8 isRawMouseMotionUsed) -> WindowBuilder& {
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

auto WindowBuilder::WithFullscreen(const MonitorHandle& monitor, b8 useMonitorSize, b8 centerCursor) -> WindowBuilder& {
    if (monitor == nullptr) {
        return WithWindowed();
    }
    _args.Monitor = monitor;
    _args.FullscreenMode = FullscreenMode::FULLSCREEN;
    _args.IsCursorCentered = centerCursor;
    _args.UseMonitorSize = useMonitorSize;
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

auto WindowBuilder::WithOpacity(float opacity01) -> WindowBuilder& {
    _args.Opacity01 = std::clamp(opacity01, 0.0f, 1.0f);
    _args.IsTransparentFramebuffer = false;
    return *this;
}

auto WindowBuilder::WithTransparentFramebuffer(b8 isTransparent) -> WindowBuilder& {
    _args.IsTransparentFramebuffer = isTransparent;
    return *this;
}

auto WindowBuilder::WithColorBitDepth(u32 redBitDepth, u32 greenBitDepth, u32 blueBitDepth, u32 alphaBitDepth) -> WindowBuilder& {
    _args.BitDepthRed = redBitDepth;
    _args.BitDepthGreen = greenBitDepth;
    _args.BitDepthBlue = blueBitDepth;
    _args.BitDepthAlpha = alphaBitDepth;
    return *this;
}

auto WindowBuilder::WithStencilBitDepth(u32 stencilBitDepth) -> WindowBuilder& {
    _args.BitDepthStencil = stencilBitDepth;
    return *this;
}

auto WindowBuilder::WithDepthBitDepth(u32 depthBitDepth) -> WindowBuilder& {
    _args.BitDepthDepth = depthBitDepth;
    return *this;
}

auto WindowBuilder::WithSrgb(b8 isSrgbCapable) -> WindowBuilder& {
    _args.IsSrgbCapable = isSrgbCapable;
    return *this;
}

auto WindowBuilder::WithStereoscopicRendering(b8 useStereoscopic) -> WindowBuilder& {
    _args.IsStereoscopicRendering = useStereoscopic;
    return *this;
}

auto WindowBuilder::WithMultisamplingSamples(u32 numSamples) -> WindowBuilder& {
    _args.MultisamplingNumSamples = numSamples;
    return *this;
}

auto WindowBuilder::WithDoublebuffered(b8 isDoublebuffered) -> WindowBuilder& {
    _args.IsDoubleBuffered = isDoublebuffered;
    return *this;
}

auto WindowBuilder::WithNoErrorMode() -> WindowBuilder& {
    _args.IsNoErrorMode = true;
    return *this;
}

auto WindowBuilder::IsValid() const -> b8 {
    return _args.Size.x > 0 && _args.Size.y > 0
           && _args.VersionMajor != 0;
}

auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle> {
    return CreateWindow(windowSystem, std::move(builder._args));
}

auto CreateWindow(const WindowSystemHandle& windowSystem, CreateWindowArgs&& args) -> std::optional<WindowHandle> {
    if (args.Size.x == 0 || args.Size.y == 0) {
        return std::nullopt;
    }
    auto* windowState = new WindowState{};
    windowState->HasContextObject = false;
    auto primaryVideoMode = glfwGetVideoMode(args.Monitor != nullptr ? args.Monitor : glfwGetPrimaryMonitor());
    windowState->MonitorSize = vec2i { primaryVideoMode->width, primaryVideoMode->height };
    windowState->Size = args.Size;
    windowState->GraphicsApi = args.GraphicsApi;
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
    switch (args.GraphicsApi) {
        case GraphicsApi::VULKAN:
            if (glfwVulkanSupported() == false) {
                printf("Can't use Vulkan on this device\n");
                std::exit(1);
            }
            if (args.VersionMajor != 1 || args.VersionMinor > 3) {
                printf("Unsupported Vulkan version %d.%d", args.VersionMajor, args.VersionMinor);
                std::exit(1);
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            SetGlfwVersionHint(args.VersionMajor, args.VersionMinor);
            break;
        case GraphicsApi::OPENGL:
            if (args.VersionMajor < 3
                || (args.VersionMajor == 3 && args.VersionMinor > 3)
                || (args.VersionMajor == 4 && args.VersionMinor > 6)) {
                printf("Unsupported OpenGL version %d.%d", args.VersionMajor, args.VersionMinor);
                std::exit(1);
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            SetGlfwVersionHint(args.VersionMajor, args.VersionMinor);
            windowState->HasContextObject = true;
            break;
        case GraphicsApi::OPENGLES:
            // FIXME: can't launch on laptop
            if (args.VersionMajor < 3
                || (args.VersionMajor == 3 && args.VersionMinor > 3)) {
                printf("Unsupported OpenGLES version %d.%d", args.VersionMajor, args.VersionMinor);
                std::exit(1);
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            SetGlfwVersionHint(args.VersionMajor, args.VersionMinor);
            windowState->HasContextObject = true;
            break;
    }
    glfwWindowHint(GLFW_VISIBLE, args.IsVisible);
    // TODO: maybe more explicit selection of refresh rate
    if (args.Monitor != nullptr && args.UseMonitorSize) {
        glfwWindowHint(GLFW_REFRESH_RATE, primaryVideoMode->refreshRate);
        args.Size.x = static_cast<u32>(primaryVideoMode->width);
        args.Size.y = static_cast<u32>(primaryVideoMode->height);
    }
    // FIXME: investigate why it still steals focus when passing FALSE
    glfwWindowHint(GLFW_FOCUSED, args.IsFocused);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, args.IsFocused);
    glfwWindowHint(GLFW_RESIZABLE, args.IsResizable);
    glfwWindowHint(GLFW_DECORATED, args.IsDecorated);
    glfwWindowHint(GLFW_AUTO_ICONIFY, args.IsAutoMinimized);
    glfwWindowHint(GLFW_FLOATING, args.IsTopmost);
    glfwWindowHint(GLFW_CENTER_CURSOR, args.IsCursorCentered);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, args.IsScaleToMonitor);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, args.IsTransparentFramebuffer);
    glfwWindowHint(GLFW_RED_BITS, static_cast<int>(args.BitDepthRed));
    glfwWindowHint(GLFW_GREEN_BITS, static_cast<int>(args.BitDepthGreen));
    glfwWindowHint(GLFW_BLUE_BITS, static_cast<int>(args.BitDepthBlue));
    glfwWindowHint(GLFW_ALPHA_BITS, static_cast<int>(args.BitDepthAlpha));
    glfwWindowHint(GLFW_DEPTH_BITS, static_cast<int>(args.BitDepthDepth));
    glfwWindowHint(GLFW_STENCIL_BITS, static_cast<int>(args.BitDepthStencil));
    glfwWindowHint(GLFW_STEREO, args.IsStereoscopicRendering);
    glfwWindowHint(GLFW_SAMPLES, static_cast<int>(args.MultisamplingNumSamples));
    glfwWindowHint(GLFW_SRGB_CAPABLE, args.IsSrgbCapable);
    glfwWindowHint(GLFW_DOUBLEBUFFER, args.IsDoubleBuffered);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, static_cast<int>(args.ContextCreationApi));
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, args.IsOpenGlForwardCompatible);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, args.IsOpenGlDebugMode);
    glfwWindowHint(GLFW_OPENGL_PROFILE, static_cast<int>(args.OpenGlProfile));
    glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);
    glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, args.IsNoErrorMode);
    // TODO: other hints (not implemented):
    // macOS: GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_COCOA_FRAME_NAME, GLFW_COCOA_FRAME_NAME
    // x11: GLFW_X11_CLASS_NAME, GLFW_X11_INSTANCE_NAME

    auto* window = glfwCreateWindow(
        static_cast<int>(args.Size.x),
        static_cast<int>(args.Size.y), 
        args.TitleUtf8, args.Monitor, nullptr);
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
        args.WidthMin > 0 ? static_cast<int>(args.WidthMin) : GLFW_DONT_CARE,
        args.HeightMin > 0 ? static_cast<int>(args.HeightMin) : GLFW_DONT_CARE,
        args.WidthMax < (1 << 30) ? static_cast<int>(args.WidthMax) : GLFW_DONT_CARE,
        args.HeightMax < (1 << 30) ? static_cast<int>(args.HeightMax) : GLFW_DONT_CARE);
    if (args.UseAspectRatio) {
        if (args.UseSizeAsAspectRatio) {
            args.AspectNumerator = args.Size.x;
            args.AspectDenominator = args.Size.y;
        }
        glfwSetWindowAspectRatio(window,
            static_cast<int>(args.AspectNumerator),
            static_cast<int>(args.AspectDenominator));
    }
    if (args.TryRawMouseMotion && glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    auto windowHandle = WindowHandle{window};
    WindowBindToThread(windowHandle);
    WindowSetOpacity(windowHandle, args.Opacity01);
    return std::move(windowHandle);
}

auto WindowIsClosing(const WindowHandle& window) -> b8 {
    return glfwWindowShouldClose(window.get());
}

auto WindowGetSize(const WindowHandle& window) -> vec2i {
    auto size = vec2i{};
    glfwGetWindowSize(window.get(), &size.x, &size.y);
    return size;
}

auto WindowSetSize(const WindowHandle& window, vec2i size) -> void {
    glfwSetWindowSize(window.get(), size.x, size.y);
}

auto WindowSetTitleUtf8(const WindowHandle& window, const char* titleUtf8) -> void {
    glfwSetWindowTitle(window.get(), titleUtf8);
}

auto WindowSetKeyMap(const WindowHandle& window, platform::input::KeyMap&& keymap) -> void {
    GetWindowState(window)->KeyMap = std::move(keymap);
}

auto WindowSwapBuffers(const WindowHandle& window) -> void {
    if (GetWindowState(window)->HasContextObject == false) {
        return;
    }
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

auto WindowGetMousePosition(const WindowHandle& window) -> vec2ff {
    vec2ff pos;
    glfwGetCursorPos(window.get(), &pos.x, &pos.y);
    return pos;
}

auto WindowGetMousePosition(const WindowHandle& window, vec2ff& destination) -> void {
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
    if (GetWindowState(window)->GraphicsApi != GraphicsApi::VULKAN) {
        return std::nullopt;
    }
    VkSurfaceKHR surface;
    VkResult status = glfwCreateWindowSurface(vkInstance, window.get(), nullptr, &surface);
    if (status != VK_SUCCESS) {
        return std::nullopt;
    }
    return surface;
}

auto WindowVulkanGetRequiredExtensionsCount(const WindowHandle&) -> u32 {
    u32 glfwExtensionCount;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    (void)glfwExtensions;
    return glfwExtensionCount;
}

auto WindowVulkanGetRequiredExtensions(const WindowHandle&) -> const char ** {
    u32 glfwExtensionCount;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return glfwExtensions;
}

auto WindowBindToThread(const WindowHandle& window) -> void {
    if (GetWindowState(window)->GraphicsApi != GraphicsApi::OPENGL) {
        return;
    }
    glfwMakeContextCurrent(window.get());
}

auto WindowUnbindFromThread(const WindowHandle&) -> void {
    glfwMakeContextCurrent(nullptr);
}

auto WindowToFullscreen(const WindowHandle& window, const MonitorHandle& monitor) -> void {
    auto videoMode = glfwGetVideoMode(monitor);
    WindowToFullscreen(window, monitor,
        {videoMode->width, videoMode->height}, 
        static_cast<u32>(videoMode->refreshRate));
}

// TODO: make more obvious selection of refresh rate
auto WindowToFullscreen(const WindowHandle& window, const MonitorHandle& monitor, vec2u size) -> void {
    WindowToFullscreen(window, monitor,
        size, static_cast<u32>(GLFW_DONT_CARE));
}
auto WindowToFullscreen(const WindowHandle& window, const MonitorHandle& monitor, vec2u size, u32 refreshRate) -> void {
    if (monitor == nullptr) {
        return;
    }
    glfwSetWindowMonitor(window.get(), monitor, 0, 0,
        static_cast<int>(size.x),
        static_cast<int>(size.y),
        static_cast<int>(refreshRate));
}

auto WindowToWindowed(const WindowHandle& window) -> void {
    auto windowSize = GetWindowState(window)->Size;
    vec2i windowPos;
    // FIXME: after first switch to fullscreen, windowPos will be zeros,
    // i.e. it won't remember old window position
    glfwGetWindowPos(window.get(), &windowPos.x, &windowPos.y);
    glfwSetWindowMonitor(window.get(), nullptr, windowPos.x, windowPos.y,
        windowSize.x, windowSize.y, 0);
}

auto WindowToWindowedBorderless(const WindowHandle& window) -> void {
    auto desktopResolution = GetWindowState(window)->MonitorSize;
    glfwSetWindowMonitor(window.get(), nullptr, 0, 0,
        desktopResolution.x, desktopResolution.y, 0);
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

auto WindowSetVisible(const WindowHandle& window, b8 makeVisible) -> void {
    if (makeVisible) {
        glfwShowWindow(window.get());
    } else {
        glfwHideWindow(window.get());
    }
}

auto WindowIsVisible(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_VISIBLE);
}

auto WindowIsFocused(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_FOCUSED);
}

auto WindowIsHovered(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_HOVERED);
}

auto WindowRequestAttention(const WindowHandle& window) -> void {
    glfwRequestWindowAttention(window.get());
}

auto WindowSetOpacity(const WindowHandle& window, float opacity01) -> void {
    if (glfwGetWindowAttrib(window.get(), GLFW_TRANSPARENT_FRAMEBUFFER)) {
        return;
    }
    glfwSetWindowOpacity(window.get(), opacity01);
}

auto WindowGetOpacity(const WindowHandle& window) -> float {
    return glfwGetWindowOpacity(window.get());
}

auto WindowSetIsResizable(const WindowHandle& window, b8 makeResizable) -> void {
    glfwSetWindowAttrib(window.get(), GLFW_RESIZABLE, makeResizable);
}

auto WindowIsResizable(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_RESIZABLE);
}

auto WindowSetIsTopmost(const WindowHandle& window, b8 makeFloating) -> void {
    glfwSetWindowAttrib(window.get(), GLFW_FLOATING, makeFloating);
}

auto WindowIsTopmost(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_FLOATING);
}

auto WindowSetIsDecorated(const WindowHandle& window, b8 makeDecorated) -> void {
    glfwSetWindowAttrib(window.get(), GLFW_DECORATED, makeDecorated); 
}

auto WindowIsDecorated(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_DECORATED);
}

auto WindowSetIsAutoMinimized(const WindowHandle& window, b8 makeAutoMinimized) -> void {
    glfwSetWindowAttrib(window.get(), GLFW_AUTO_ICONIFY, makeAutoMinimized);
}

auto WindowIsAutoMinimized(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_AUTO_ICONIFY);
}

auto WindowSetIsFocusedAfterVisible(const WindowHandle& window, b8 makeFocusedAfterVisible) -> void {
    glfwSetWindowAttrib(window.get(), GLFW_FOCUS_ON_SHOW, makeFocusedAfterVisible);
}

auto WindowIsFocusedAfterVisible(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_FOCUS_ON_SHOW);
}

auto WindowContextGetApi(const WindowHandle& window) -> GraphicsApi {
    auto contextApi = glfwGetWindowAttrib(window.get(), GLFW_CLIENT_API);
    switch (contextApi) {
        case GLFW_OPENGL_API: return GraphicsApi::OPENGL;
        case GLFW_OPENGL_ES_API: return GraphicsApi::OPENGLES;
        case GLFW_NO_API: return GraphicsApi::VULKAN;
    }
    printf("Unreachable code reached: WindowContextGetApi on value %d", static_cast<int>(contextApi));
    std::exit(1);
}

auto WindowContextGetCreationApi(const WindowHandle& window) -> ContextCreationApi {
    return static_cast<ContextCreationApi>(
        glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_CREATION_API));
}

auto WindowContextGetVersion(const WindowHandle& window, int& major, int& minor, int& revision) -> void {
    major = glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_VERSION_MINOR);
    revision = glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_REVISION);
}

auto WindowContextIsDebugMode(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_FOCUS_ON_SHOW);
}

auto WindowContextIsForwardCompatible(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_OPENGL_FORWARD_COMPAT);
}

auto WindowContextIsNoErrorMode(const WindowHandle& window) -> b8 {
    return glfwGetWindowAttrib(window.get(), GLFW_CONTEXT_NO_ERROR);
}

}