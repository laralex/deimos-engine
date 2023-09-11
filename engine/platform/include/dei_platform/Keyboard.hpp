#pragma once

#include "Prelude.hpp"

#include <unordered_map>
#include <functional>
#include <bitset>

namespace {

constexpr size_t MODIFIER_CTRL_BIT     = GLFW_MOD_CONTROL;
constexpr size_t MODIFIER_SHIFT_BIT    = GLFW_MOD_SHIFT;
constexpr size_t MODIFIER_ALT_BIT      = GLFW_MOD_ALT;
constexpr size_t MODIFIER_SUPER_BIT    = GLFW_MOD_SUPER;
constexpr size_t MODIFIER_CAPSLOCK_BIT = GLFW_MOD_CAPS_LOCK;
constexpr size_t MODIFIER_NUMLOCK_BIT  = GLFW_MOD_NUM_LOCK;

} // namespace ::

namespace dei::platform::input {

enum class KeyState {
   RELEASE = GLFW_RELEASE,
   PRESS = GLFW_PRESS,
   REPEAT = GLFW_REPEAT,
};

enum class KeyCode {
   ANYTHING = 0,
   KEY_A = GLFW_KEY_A,
   KEY_B = GLFW_KEY_B,
   KEY_C = GLFW_KEY_C,
   KEY_D = GLFW_KEY_D,
   KEY_E = GLFW_KEY_E,
   KEY_F = GLFW_KEY_F,
   KEY_G = GLFW_KEY_G,
   KEY_H = GLFW_KEY_H,
   KEY_I = GLFW_KEY_I,
   KEY_J = GLFW_KEY_J,
   KEY_K = GLFW_KEY_K,
   KEY_L = GLFW_KEY_L,
   KEY_M = GLFW_KEY_M,
   KEY_N = GLFW_KEY_N,
   KEY_O = GLFW_KEY_O,
   KEY_P = GLFW_KEY_P,
   KEY_Q = GLFW_KEY_Q,
   KEY_R = GLFW_KEY_R,
   KEY_S = GLFW_KEY_S,
   KEY_T = GLFW_KEY_T,
   KEY_U = GLFW_KEY_U,
   KEY_V = GLFW_KEY_V,
   KEY_W = GLFW_KEY_W,
   KEY_X = GLFW_KEY_X,
   KEY_Y = GLFW_KEY_Y,
   KEY_Z = GLFW_KEY_Z,
   KEY_1 = GLFW_KEY_1,
   KEY_2 = GLFW_KEY_2,
   KEY_3 = GLFW_KEY_3,
   KEY_4 = GLFW_KEY_4,
   KEY_5 = GLFW_KEY_5,
   KEY_6 = GLFW_KEY_6,
   KEY_7 = GLFW_KEY_7,
   KEY_8 = GLFW_KEY_8,
   KEY_9 = GLFW_KEY_9,
   KEY_0 = GLFW_KEY_0,
   CTRL_LEFT = GLFW_KEY_LEFT_CONTROL,
   SHIFT_LEFT = GLFW_KEY_LEFT_SHIFT,
   ALT_LEFT = GLFW_KEY_LEFT_ALT,
   CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
   TAB = GLFW_KEY_TAB,
   ESCAPE = GLFW_KEY_ESCAPE,
   ENTER = GLFW_KEY_ENTER,
   BACKSPACE = GLFW_KEY_BACKSPACE,
};


using ModifierKeysState = std::bitset<8>;
constexpr ModifierKeysState MODIFIERS_NONE = {0};
constexpr ModifierKeysState MODIFIERS_CTRL = {::MODIFIER_CTRL_BIT};
constexpr ModifierKeysState MODIFIERS_SHIFT = {::MODIFIER_SHIFT_BIT};
constexpr ModifierKeysState MODIFIERS_ALT = {::MODIFIER_ALT_BIT};
constexpr ModifierKeysState MODIFIERS_SUPER = {::MODIFIER_SUPER_BIT};
constexpr ModifierKeysState MODIFIERS_CTRL_SHIFT = {::MODIFIER_CTRL_BIT | ::MODIFIER_SHIFT_BIT};

using KeyMap = std::unordered_map<
   std::pair<KeyCode, ModifierKeysState>,
   std::function<void(KeyCode, KeyState, const char*)>,
   pair_hash
>;

typedef void (*InputTextCallback)(const std::string& currentTextUtf8, uint32_t latestCodepoint);

}