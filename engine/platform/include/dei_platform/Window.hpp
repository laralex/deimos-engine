#pragma once

#include "Prelude.hpp"
#include "Keyboard.hpp"

#include <memory>
#include <optional>

namespace dei::platform {

auto CreateWindowSystem() -> WindowSystemHandle;
auto PollWindowEvents(const WindowSystemHandle&) -> void;
auto GetKeyName(input::KeyCode) -> const char*;

struct CreateWindowArgs {
   size_t Width;
   size_t Height;
   const char* TitleUtf8;
   enum class GraphicsBackend {
      VULKAN = static_cast<int>(GraphicsApi::VULKAN_10),
   } GraphicalBackend;
   input::KeyMap KeyMap;
   input::InputTextCallback InputTextCallback;
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
   auto WithInputTextCallback(input::InputTextCallback) -> WindowBuilder&;
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
auto WindowGetSize(const WindowHandle&) -> size2i;
auto WindowSetKeyMap(const WindowHandle&, input::KeyMap&&) -> void;
auto WindowSwapBuffers(const WindowHandle&) -> void;
auto WindowClearInput(const WindowHandle&) -> void;
auto WindowUndoInput(const WindowHandle&) -> void;
//glfwSetKeyCallback(window, key_callback);

} // dei::platform