#pragma once

#include "Prelude.hpp"

namespace dei::platform::input {

enum class MouseButtonState {
   RELEASE = GLFW_RELEASE,
   PRESS = GLFW_PRESS,
};

enum class MouseButton {
   M1 = GLFW_MOUSE_BUTTON_1,
   M2 = GLFW_MOUSE_BUTTON_2,
   M3 = GLFW_MOUSE_BUTTON_3,
   M4 = GLFW_MOUSE_BUTTON_4,
   M5 = GLFW_MOUSE_BUTTON_5,
   M6 = GLFW_MOUSE_BUTTON_6,
   M7 = GLFW_MOUSE_BUTTON_7,
   M8 = GLFW_MOUSE_BUTTON_8,
   LEFT = GLFW_MOUSE_BUTTON_LEFT,
   RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
   MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
   _MAX_BUTTONS = GLFW_MOUSE_BUTTON_8 + 1,
};

enum class CursorMode {
   DISABLED = GLFW_CURSOR_DISABLED,
   NORMAL = GLFW_CURSOR_NORMAL,
   HIDDEN = GLFW_CURSOR_HIDDEN,
};

typedef void (*MousePositionCallback)(double windowX, double windowY);
typedef void (*MouseScrollCallback)(double directionX, double directionY);
typedef void (*MouseButtonCallback)(MouseButton, MouseButtonState);
typedef void (*MouseEntersWindowCallback)(b8 hasEntered);


}