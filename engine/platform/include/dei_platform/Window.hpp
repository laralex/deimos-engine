#pragma once

#include "Prelude.hpp"
#include "TypesVec.hpp"
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
auto SetVerticalSync(const WindowSystemHandle&, b8 enableVerticalSync) -> void;

typedef void (*WindowResizeCallback)(int widthPx, int heightPx);
typedef void (*WindowPositionCallback)(int leftUpCornerX, int leftUpCornerY);
typedef void (*WindowClosingCallback)();
typedef void (*WindowFocusedCallback)(b8 isFocused);

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
   u32 VersionMajor = 0, VersionMinor = 0, VersionRevision = 0;
   ContextCreationApi ContextCreationApi = ContextCreationApi::NATIVE;
   vec2i Size = { 800, 600 };
   b8 UseMonitorSize = false;
   u32 WidthMin = 0, HeightMin = 0;
   u32 WidthMax = 1 << 31, HeightMax = 1 << 31;
   b8 UseAspectRatio = false;
   b8 UseSizeAsAspectRatio = false;
   u32 AspectNumerator = 1;
   u32 AspectDenominator = 1;
   const char* TitleUtf8 = "";
   b8 TryRawMouseMotion = false;
   b8 IsVisible = true;
   b8 IsFocused = true;
   b8 IsResizable = true;
   b8 IsDecorated = true;
   b8 IsAutoMinimized = false;
   b8 IsTopmost = false;
   b8 IsCursorCentered = false;
   b8 IsScaleToMonitor = true;
   b8 IsSrgbCapable = false;
   b8 IsStereoscopicRendering = false;
   b8 IsDoubleBuffered = true;
   b8 IsNoErrorMode = false;
   b8 IsOpenGlForwardCompatible = false;
   b8 IsOpenGlDebugMode = false;
   OpenGlProfile OpenGlProfile = OpenGlProfile::ANY;
   u32 BitDepthRed = 8, BitDepthGreen = 8, BitDepthBlue = 8, BitDepthAlpha = 8;
   u32 BitDepthDepth = 24, BitDepthStencil = 8;
   u32 MultisamplingNumSamples = 0;
   f32 Opacity01 = 1.0f;
   b8 IsTransparentFramebuffer = false;
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
   auto WithSize(u32 width, u32 height) -> WindowBuilder&;
   auto WithSizeMin(u32 width, u32 height) -> WindowBuilder&;
   auto WithSizeMax(u32 width, u32 height) -> WindowBuilder&;
   auto WithAspectRatioForceCurrent() -> WindowBuilder&;
   auto WithAspectRatio(u32 aspectNumerator, u32 aspectDenominator) -> WindowBuilder&;
   auto WithTitleUtf8(const char*) -> WindowBuilder&;
   auto WithVulkan(u32 versionMajor, u32 versionMinor) -> WindowBuilder&;
   auto WithOpenGL(u32 versionMajor, u32 versionMinor, ContextCreationApi) -> WindowBuilder&;
   auto WithOpenGLES(u32 versionMajor, u32 versionMinor, ContextCreationApi) -> WindowBuilder&;
   auto WithOpenGLSettings(OpenGlProfile, b8 isForwardCompatible, b8 isDebugMode) -> WindowBuilder&;
   auto WithKeymap(input::KeyMap&&) -> WindowBuilder&;
   auto WithResizable(b8 isResizable) -> WindowBuilder&;
   auto WithVisible(b8 isVisible) -> WindowBuilder&;
   auto WithDecorated(b8 isDecorated) -> WindowBuilder&;
   auto WithFocused(b8 isWindowFocused) -> WindowBuilder&;
   auto WithAutoMinimized(b8 isAutoMinimized) -> WindowBuilder&;
   auto WithTopmost(b8 isTopmost) -> WindowBuilder&;
   auto WithScaleToMonitor(b8 isScaleToMonitor) -> WindowBuilder&;
   auto WithFullscreen(const MonitorHandle&, b8 useMonitorSize=false, b8 isCursorCentered=true) -> WindowBuilder&;
   auto WithColorBitDepth(u32 red, u32 green, u32 blue, u32 alpha) -> WindowBuilder&;
   auto WithStencilBitDepth(u32 stencilBitDepth) -> WindowBuilder&;
   auto WithDepthBitDepth(u32 depthBitDepth) -> WindowBuilder&;
   auto WithSrgb(b8 isSrgbCapable) -> WindowBuilder&;
   auto WithStereoscopicRendering(b8 useStereoscopic) -> WindowBuilder&;
   auto WithMultisamplingSamples(u32 numSamples) -> WindowBuilder&;
   auto WithDoublebuffered(b8 isDoublebuffered) -> WindowBuilder&;
   auto WithNoErrorMode() -> WindowBuilder&;
   auto WithWindowed() -> WindowBuilder&;
   auto WithWindowedBorderless() -> WindowBuilder&;
   auto WithOpacity(f32 opacity01) -> WindowBuilder&;
   auto WithFocusCallback(WindowFocusedCallback) -> WindowBuilder&;
   auto WithTransparentFramebuffer(b8 isTransparent) -> WindowBuilder&;
   auto WithRawMouseMotion(b8 isRawMouseMotionUsed) -> WindowBuilder&;
   auto WithPositionCallback(WindowPositionCallback) -> WindowBuilder&;
   auto WithResizeCallback(WindowResizeCallback) -> WindowBuilder&;
   auto WithClosingCallback(WindowClosingCallback) -> WindowBuilder&;
   auto WithInputTextCallback(input::InputTextCallback) -> WindowBuilder&;
   auto WithMousePositionCallback(input::MousePositionCallback) -> WindowBuilder&;
   auto WithMouseButtonCallback(input::MouseButtonCallback) -> WindowBuilder&;
   auto WithMouseScrollCallback(input::MouseScrollCallback) -> WindowBuilder&;
   auto WithMouseEntersWindowCallback(input::MouseEntersWindowCallback) -> WindowBuilder&;
   auto IsValid() const -> b8;
   friend auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle>;
private:
   CreateWindowArgs _args;
};

auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle>;
auto CreateWindow(const WindowSystemHandle& windowSystem, CreateWindowArgs&& builder) -> std::optional<WindowHandle>;
auto WindowSetTitleUtf8(const WindowHandle&, const char* titleUtf8) -> void;
auto WindowIsClosing(const WindowHandle&) -> b8;
auto WindowGetSize(const WindowHandle&) -> vec2i;
auto WindowSetSize(const WindowHandle&, vec2i size) -> void;
auto WindowSetKeyMap(const WindowHandle&, input::KeyMap&&) -> void;
auto WindowSwapBuffers(const WindowHandle&) -> void;
auto WindowAppendInputUtf8(const WindowHandle&, const char* textUtf8) -> void;
auto WindowClearInput(const WindowHandle&) -> void;
auto WindowUndoInput(const WindowHandle&) -> void;
auto WindowGetMousePosition(const WindowHandle&) -> vec2ff;
auto WindowGetMousePosition(const WindowHandle&, vec2ff& destination) -> void;
auto WindowSetCursorMode(const WindowHandle&, input::CursorMode mode) -> void;
auto WindowGetCursorMode(const WindowHandle&) -> input::CursorMode;
auto WindowBindToThread(const WindowHandle&) -> void;
auto WindowUnbindFromThread(const WindowHandle&) -> void;
auto WindowToFullscreen(const WindowHandle&, const MonitorHandle&) -> void;
auto WindowToFullscreen(const WindowHandle&, const MonitorHandle&, vec2u size) -> void;
auto WindowToFullscreen(const WindowHandle&, const MonitorHandle&, vec2u size, u32 refreshRate) -> void;
auto WindowToWindowed(const WindowHandle&) -> void;
auto WindowToWindowedBorderless(const WindowHandle&) -> void;
auto WindowSetFullscreenMode(const WindowHandle&, FullscreenMode, const MonitorHandle&) -> void;
auto WindowSetVisible(const WindowHandle&, b8 makeVisible) -> void;
auto WindowIsVisible(const WindowHandle&) -> b8;
auto WindowIsFocused(const WindowHandle&) -> b8;
auto WindowIsHovered(const WindowHandle&) -> b8;
auto WindowRequestAttention(const WindowHandle&) -> void;
auto WindowSetOpacity(const WindowHandle&, f32 opacity01) -> void;
auto WindowGetOpacity(const WindowHandle& window) -> f32;
auto WindowSetIsResizable(const WindowHandle&, b8 makeResizable) -> void;
auto WindowIsResizable(const WindowHandle&) -> b8;
auto WindowSetIsTopmost(const WindowHandle&, b8 makeFloating) -> void;
auto WindowIsTopmost(const WindowHandle&) -> b8;
auto WindowSetIsDecorated(const WindowHandle&, b8 makeDecorated) -> void;
auto WindowIsDecorated(const WindowHandle&) -> b8;
auto WindowSetIsAutoMinimized(const WindowHandle&, b8 makeAutoMinimized) -> void;
auto WindowIsAutoMinimized(const WindowHandle&) -> b8;
auto WindowSetIsFocusedAfterVisible(const WindowHandle&, b8 makeFocusedAfterVisible) -> void;
auto WindowIsFocusedAfterVisible(const WindowHandle&) -> b8;
auto WindowSetSizeMode(const WindowHandle&, WindowSizeMode mode) -> void;
auto WindowGetSizeMode(const WindowHandle&) -> WindowSizeMode;

auto WindowInitializeVulkanBackend(const WindowHandle&, VkInstance) -> std::optional<VkSurfaceKHR>;
auto WindowVulkanGetRequiredExtensionsCount(const WindowHandle&) -> u32;
auto WindowVulkanGetRequiredExtensions(const WindowHandle&) -> const char **;

auto WindowContextGetApi(const WindowHandle&) -> GraphicsApi;
auto WindowContextGetCreationApi(const WindowHandle&) -> ContextCreationApi;
auto WindowContextGetVersion(const WindowHandle&, i32&, i32&, i32&) -> void;
auto WindowContextIsDebugMode(const WindowHandle&) -> b8;
auto WindowContextIsForwardCompatible(const WindowHandle&) -> b8;
auto WindowContextIsNoErrorMode(const WindowHandle&) -> b8;

// TODO: also there are GLFW_OPENGL_PROFILE, GLFW_CONTEXT_RELEASE_BEHAVIOR, 
// GLFW_CONTEXT_ROBUSTNESS attributes

} // dei::platform