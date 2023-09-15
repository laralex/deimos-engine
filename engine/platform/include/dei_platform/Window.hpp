#pragma once

#include "Prelude.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

#include <memory>
#include <optional>

namespace dei::platform {

auto CreateWindowSystem(void (*errorCallback)(int, const char*) = nullptr) -> WindowSystemHandle;
auto PollWindowEvents(const WindowSystemHandle&) -> void;
auto GetKeyName(input::KeyCode) -> const char*;
auto GetClipboardUtf8(const WindowSystemHandle&) -> const char *;
auto SetClipboardUtf8(const WindowSystemHandle&, const char* textUtff8) -> void;
auto SetVerticalSync(const WindowSystemHandle&, bool enableVerticalSync) -> void;

typedef void (*WindowResizeCallback)(int widthPx, int heightPx);
typedef void (*WindowPositionCallback)(int leftUpCornerX, int leftUpCornerY);

enum class GraphicsBackend {
   VULKAN = static_cast<int>(GraphicsApi::VULKAN_10),
};
constexpr const char* GraphicsBackendToStr(GraphicsBackend backend) {
   switch (backend) {
      case GraphicsBackend::VULKAN: return stringify(GraphicsBackend::VULKAN);
   }
   printf("Unreachable code reached: GraphicsBackendToStr on value %d", static_cast<int>(backend));
   std::exit(1);
};

struct CreateWindowArgs {
   GraphicsBackend GraphicsBackend;
   size_t Width, Height;
   size_t WidthMin = 0, HeightMin = 0;
   size_t WidthMax = 1 << 31, HeightMax = 1 << 31;
   bool UseAspectRatio = false;
   bool UseSizeAsAspectRatio = false;
   size_t AspectNumerator = 1;
   size_t AspectDenominator = 1;
   const char* TitleUtf8;
   bool TryRawMouseMotion;
   bool IsVisible = true;
   WindowResizeCallback WindowResizeCallback;
   WindowPositionCallback WindowPositionCallback;
   input::KeyMap KeyMap;
   input::InputTextCallback InputTextCallback;
   input::MousePositionCallback MousePositionCallback;
   input::MouseButtonCallback MouseButtonCallback;
   input::MouseScrollCallback MouseScrollCallback;
   input::MouseEntersWindowCallback MouseEntersWindowCallback;
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
   auto WithGraphicsBackend(GraphicsBackend) -> WindowBuilder&;
   auto WithKeymap(input::KeyMap&&) -> WindowBuilder&;
   auto WithVisible(bool isVisible) -> WindowBuilder&;
   auto WithRawMouseMotion(bool isRawMouseMotionUsed) -> WindowBuilder&;
   auto WithPositionCallback(WindowPositionCallback) -> WindowBuilder&;
   auto WithResizeCallback(WindowResizeCallback) -> WindowBuilder&;
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

//    Window(Window&&) = default;
//    Window(const Window&) = delete;
//    auto IsClosing() const -> bool;
//    auto GetWidth() const -> size_t;
//    auto GetHeight() const -> size_t;
//    auto SetTitleUtf8(const char* titleUtf8) -> void;
//    auto SetKeyMap(KeyMap&&) -> void;
// protected:
//    static auto KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
// private:
//    explicit Window(const WindowSystemHandle&, CreateWindowArgs&&);
//    WindowHandle _window;
//    WindowSystemHandle _windowSystem;
//    CreateWindowArgs _createArgs;
// };

auto CreateWindow(const WindowSystemHandle& windowSystem, WindowBuilder&& builder) -> std::optional<WindowHandle>;
auto CreateWindow(const WindowSystemHandle& windowSystem, CreateWindowArgs&& builder) -> std::optional<WindowHandle>;
auto WindowSetTitleUtf8(const WindowHandle&, const char* titleUtf8) -> void;
auto WindowIsClosing(const WindowHandle&) -> bool;
auto WindowGetSize(const WindowHandle&) -> isize2;
auto WindowSetKeyMap(const WindowHandle&, input::KeyMap&&) -> void;
auto WindowSwapBuffers(const WindowHandle&) -> void;
auto WindowAppendInputUtf8(const WindowHandle&, const char* textUtf8) -> void;
auto WindowClearInput(const WindowHandle&) -> void;
auto WindowUndoInput(const WindowHandle&) -> void;
auto WindowGetMousePosition(const WindowHandle&) -> dvec2;
auto WindowGetMousePosition(const WindowHandle&, dvec2& destination) -> void;
auto WindowSetCursorMode(const WindowHandle&, input::CursorMode mode) -> void;
auto WindowGetCursorMode(const WindowHandle&) -> input::CursorMode;
auto WindowInitializeVulkanBackend(const WindowHandle&, VkInstance) -> std::optional<VkSurfaceKHR>;

enum class WindowSizeMode {
   NORMAL,
   MAXIMIZED,
   MINIMIZED,
};
auto WindowSetSizeMode(const WindowHandle&, WindowSizeMode mode) -> void;
auto WindowGetSizeMode(const WindowHandle&) -> WindowSizeMode;
//glfwSetKeyCallback(window, key_callback);

} // dei::platform