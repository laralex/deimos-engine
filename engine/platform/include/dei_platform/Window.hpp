#pragma once

#include "Prelude.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Monitor.hpp"

#include <memory>
#include <optional>

namespace dei::platform {

auto CreateWindowSystem(void (*errorCallback)(int, const char*) = nullptr) -> WindowSystemHandle;
auto PollWindowEvents(const WindowSystemHandle&) -> void;
auto GetKeyName(const WindowSystemHandle&, input::KeyCode) -> const char*;
auto GetClipboardUtf8(const WindowSystemHandle&) -> const char *;
auto SetClipboardUtf8(const WindowSystemHandle&, const char* textUtff8) -> void;
auto SetVerticalSync(const WindowSystemHandle&, bool enableVerticalSync) -> void;

typedef void (*WindowResizeCallback)(int widthPx, int heightPx);
typedef void (*WindowPositionCallback)(int leftUpCornerX, int leftUpCornerY);
typedef void (*WindowClosingCallback)();
typedef void (*WindowFocusedCallback)(bool isFocused);

enum class ContextCreationApi {
   NONE = 0,
   NATIVE = GLFW_NATIVE_CONTEXT_API,
   OS_MESA = GLFW_OSMESA_CONTEXT_API,
   EGL = GLFW_EGL_CONTEXT_API,
};

constexpr const char* ContextCreationApiToStr(ContextCreationApi api) {
   switch (api) {
      case ContextCreationApi::NATIVE: return stringify(ContextCreationApi::NATIVE);
      case ContextCreationApi::OS_MESA: return stringify(ContextCreationApi::OS_MESA);
      case ContextCreationApi::EGL: return stringify(ContextCreationApi::EGL);
      case ContextCreationApi::NONE: return stringify(ContextCreationApi::NONE);
   }
   printf("Unreachable code reached: ContextCreationApiToStr on value %d", static_cast<int>(api));
   std::exit(1);
};

enum class FullscreenMode {
   FULLSCREEN,
   WINDOWED,
   WINDOWED_BORDERLESS
};

enum class WindowSizeMode {
   NORMAL,
   MAXIMIZED,
   MINIMIZED,
};

enum class OpenGlProfile {
   ANY = GLFW_OPENGL_ANY_PROFILE,
   CORE = GLFW_OPENGL_CORE_PROFILE,
   COMPATIBLE = GLFW_OPENGL_COMPAT_PROFILE,
};

struct CreateWindowArgs {
   GraphicsApi GraphicsApi;
   size_t VersionMajor = 0, VersionMinor = 0, VersionRevision = 0;
   ContextCreationApi ContextCreationApi = ContextCreationApi::NATIVE;
   isize2 Size = { 800, 600 };
   bool UseMonitorSize = false;
   size_t WidthMin = 0, HeightMin = 0;
   size_t WidthMax = 1 << 31, HeightMax = 1 << 31;
   bool UseAspectRatio = false;
   bool UseSizeAsAspectRatio = false;
   size_t AspectNumerator = 1;
   size_t AspectDenominator = 1;
   const char* TitleUtf8 = "";
   bool TryRawMouseMotion = false;
   bool IsVisible = true;
   bool IsFocused = true;
   bool IsResizable = true;
   bool IsDecorated = true;
   bool IsAutoMinimized = false;
   bool IsTopmost = false;
   bool IsCursorCentered = false;
   bool IsScaleToMonitor = true;
   bool IsSrgbCapable = false;
   bool IsStereoscopicRendering = false;
   bool IsDoubleBuffered = true;
   bool IsNoErrorMode = false;
   bool IsOpenGlForwardCompatible = false;
   bool IsOpenGlDebugMode = false;
   OpenGlProfile OpenGlProfile = OpenGlProfile::ANY;
   size_t BitDepthRed = 8, BitDepthGreen = 8, BitDepthBlue = 8, BitDepthAlpha = 8;
   size_t BitDepthDepth = 24, BitDepthStencil = 8;
   size_t MultisamplingNumSamples = 0;
   float Opacity01 = 1.0f;
   bool IsTransparentFramebuffer = false;
   FullscreenMode FullscreenMode = FullscreenMode::WINDOWED;
   MonitorHandle Monitor = nullptr;
   WindowResizeCallback WindowResizeCallback = nullptr;
   WindowPositionCallback WindowPositionCallback = nullptr;
   WindowClosingCallback WindowClosingCallback = nullptr;
   WindowFocusedCallback WindowFocusedCallback = nullptr;
   input::KeyMap KeyMap = {};
   input::InputTextCallback InputTextCallback = nullptr;
   input::MousePositionCallback MousePositionCallback = nullptr;
   input::MouseButtonCallback MouseButtonCallback = nullptr;
   input::MouseScrollCallback MouseScrollCallback = nullptr;
   input::MouseEntersWindowCallback MouseEntersWindowCallback = nullptr;
};

struct WindowBuilder {
   WindowBuilder() {};
   WindowBuilder(const WindowBuilder&) = delete;
   WindowBuilder(WindowBuilder&&) = default;
   auto operator=(WindowBuilder&&) -> WindowBuilder& = default;
   auto WithSize(size_t width, size_t height) -> WindowBuilder&;
   auto WithSizeMin(size_t width, size_t height) -> WindowBuilder&;
   auto WithSizeMax(size_t width, size_t height) -> WindowBuilder&;
   auto WithAspectRatioForceCurrent() -> WindowBuilder&;
   auto WithAspectRatio(size_t aspectNumerator, size_t aspectDenominator) -> WindowBuilder&;
   auto WithTitleUtf8(const char*) -> WindowBuilder&;
   auto WithVulkan(size_t versionMajor, size_t versionMinor) -> WindowBuilder&;
   auto WithOpenGL(size_t versionMajor, size_t versionMinor, ContextCreationApi) -> WindowBuilder&;
   auto WithOpenGLES(size_t versionMajor, size_t versionMinor, ContextCreationApi) -> WindowBuilder&;
   auto WithOpenGLSettings(OpenGlProfile, bool isForwardCompatible, bool isDebugMode) -> WindowBuilder&;
   auto WithKeymap(input::KeyMap&&) -> WindowBuilder&;
   auto WithResizable(bool isResizable) -> WindowBuilder&;
   auto WithVisible(bool isVisible) -> WindowBuilder&;
   auto WithDecorated(bool isDecorated) -> WindowBuilder&;
   auto WithFocused(bool isWindowFocused) -> WindowBuilder&;
   auto WithAutoMinimized(bool isAutoMinimized) -> WindowBuilder&;
   auto WithTopmost(bool isTopmost) -> WindowBuilder&;
   auto WithScaleToMonitor(bool isScaleToMonitor) -> WindowBuilder&;
   auto WithFullscreen(const MonitorHandle&, bool useMonitorSize=false, bool isCursorCentered=true) -> WindowBuilder&;
   auto WithColorBitDepth(size_t red, size_t green, size_t blue, size_t alpha) -> WindowBuilder&;
   auto WithStencilBitDepth(size_t stencilBitDepth) -> WindowBuilder&;
   auto WithDepthBitDepth(size_t depthBitDepth) -> WindowBuilder&;
   auto WithSrgb(bool isSrgbCapable) -> WindowBuilder&;
   auto WithStereoscopicRendering(bool useStereoscopic) -> WindowBuilder&;
   auto WithMultisamplingSamples(size_t numSamples) -> WindowBuilder&;
   auto WithDoublebuffered(bool isDoublebuffered) -> WindowBuilder&;
   auto WithNoErrorMode() -> WindowBuilder&;
   auto WithWindowed() -> WindowBuilder&;
   auto WithWindowedBorderless() -> WindowBuilder&;
   auto WithOpacity(float opacity01) -> WindowBuilder&;
   auto WithFocusCallback(WindowFocusedCallback) -> WindowBuilder&;
   auto WithTransparentFramebuffer(bool isTransparent) -> WindowBuilder&;
   auto WithRawMouseMotion(bool isRawMouseMotionUsed) -> WindowBuilder&;
   auto WithPositionCallback(WindowPositionCallback) -> WindowBuilder&;
   auto WithResizeCallback(WindowResizeCallback) -> WindowBuilder&;
   auto WithClosingCallback(WindowClosingCallback) -> WindowBuilder&;
   auto WithInputTextCallback(input::InputTextCallback) -> WindowBuilder&;
   auto WithMousePositionCallback(input::MousePositionCallback) -> WindowBuilder&;
   auto WithMouseButtonCallback(input::MouseButtonCallback) -> WindowBuilder&;
   auto WithMouseScrollCallback(input::MouseScrollCallback) -> WindowBuilder&;
   auto WithMouseEntersWindowCallback(input::MouseEntersWindowCallback) -> WindowBuilder&;
   auto IsValid() const -> bool;
   friend auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle>;
private:
   CreateWindowArgs _args;
};

auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle>;
auto CreateWindow(const WindowSystemHandle& windowSystem, CreateWindowArgs&& builder) -> std::optional<WindowHandle>;
auto WindowSetTitleUtf8(const WindowHandle&, const char* titleUtf8) -> void;
auto WindowIsClosing(const WindowHandle&) -> bool;
auto WindowGetSize(const WindowHandle&) -> isize2;
auto WindowSetSize(const WindowHandle&, isize2 size) -> void;
auto WindowSetKeyMap(const WindowHandle&, input::KeyMap&&) -> void;
auto WindowSwapBuffers(const WindowHandle&) -> void;
auto WindowAppendInputUtf8(const WindowHandle&, const char* textUtf8) -> void;
auto WindowClearInput(const WindowHandle&) -> void;
auto WindowUndoInput(const WindowHandle&) -> void;
auto WindowGetMousePosition(const WindowHandle&) -> dvec2;
auto WindowGetMousePosition(const WindowHandle&, dvec2& destination) -> void;
auto WindowSetCursorMode(const WindowHandle&, input::CursorMode mode) -> void;
auto WindowGetCursorMode(const WindowHandle&) -> input::CursorMode;
auto WindowBindToThread(const WindowHandle&) -> void;
auto WindowUnbindFromThread(const WindowHandle&) -> void;
auto WindowToFullscreen(const WindowHandle&, const MonitorHandle&) -> void;
auto WindowToFullscreen(const WindowHandle&, const MonitorHandle&, isize2 size) -> void;
auto WindowToFullscreen(const WindowHandle&, const MonitorHandle&, isize2 size, int refreshRate) -> void;
auto WindowToWindowed(const WindowHandle&) -> void;
auto WindowToWindowedBorderless(const WindowHandle&) -> void;
auto WindowSetFullscreenMode(const WindowHandle&, FullscreenMode, const MonitorHandle&) -> void;
auto WindowSetVisible(const WindowHandle&, bool makeVisible) -> void;
auto WindowIsVisible(const WindowHandle&) -> bool;
auto WindowIsFocused(const WindowHandle&) -> bool;
auto WindowIsHovered(const WindowHandle&) -> bool;
auto WindowRequestAttention(const WindowHandle&) -> void;
auto WindowSetOpacity(const WindowHandle&, float opacity01) -> void;
auto WindowGetOpacity(const WindowHandle& window) -> float;
auto WindowSetIsResizable(const WindowHandle&, bool makeResizable) -> void;
auto WindowIsResizable(const WindowHandle&) -> bool;
auto WindowSetIsTopmost(const WindowHandle&, bool makeFloating) -> void;
auto WindowIsTopmost(const WindowHandle&) -> bool;
auto WindowSetIsDecorated(const WindowHandle&, bool makeDecorated) -> void;
auto WindowIsDecorated(const WindowHandle&) -> bool;
auto WindowSetIsAutoMinimized(const WindowHandle&, bool makeAutoMinimized) -> void;
auto WindowIsAutoMinimized(const WindowHandle&) -> bool;
auto WindowSetIsFocusedAfterVisible(const WindowHandle&, bool makeFocusedAfterVisible) -> void;
auto WindowIsFocusedAfterVisible(const WindowHandle&) -> bool;
auto WindowSetSizeMode(const WindowHandle&, WindowSizeMode mode) -> void;
auto WindowGetSizeMode(const WindowHandle&) -> WindowSizeMode;

auto WindowInitializeVulkanBackend(const WindowHandle&, VkInstance) -> std::optional<VkSurfaceKHR>;
auto WindowVulkanGetRequiredExtensionsCount(const WindowHandle&) -> std::uint32_t;
auto WindowVulkanGetRequiredExtensions(const WindowHandle&) -> const char **;

auto WindowContextGetApi(const WindowHandle&) -> GraphicsApi;
auto WindowContextGetCreationApi(const WindowHandle&) -> ContextCreationApi;
auto WindowContextGetVersion(const WindowHandle&, int&, int&, int&) -> void;
auto WindowContextIsDebugMode(const WindowHandle&) -> bool;
auto WindowContextIsForwardCompatible(const WindowHandle&) -> bool;
auto WindowContextIsNoErrorMode(const WindowHandle&) -> bool;

// TODO: also there are GLFW_OPENGL_PROFILE, GLFW_CONTEXT_RELEASE_BEHAVIOR, 
// GLFW_CONTEXT_ROBUSTNESS attributes

} // dei::platform