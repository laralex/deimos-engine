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

typedef void (*WindowResizeCallback)(int widthPx, int heightPx);
typedef void (*WindowPositionCallback)(int leftUpCornerX, int leftUpCornerY);

struct CreateWindowArgs {
   size_t Width;
   size_t Height;
   const char* TitleUtf8;
   enum class GraphicsBackend {
      VULKAN = static_cast<int>(GraphicsApi::VULKAN_10),
   } GraphicalBackend;
   bool TryRawMouseMotion;
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
   auto WithDimensions(size_t width, size_t height) -> WindowBuilder&;
   auto WithTitleUtf8(const char*) -> WindowBuilder&;
   auto WithGraphicsBackend(CreateWindowArgs::GraphicsBackend) -> WindowBuilder&;
   auto WithKeymap(input::KeyMap&&) -> WindowBuilder&;
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
//glfwSetKeyCallback(window, key_callback);

} // dei::platform