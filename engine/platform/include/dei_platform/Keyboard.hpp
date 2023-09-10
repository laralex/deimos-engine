#pragma once

#include "Prelude.hpp"

#include <unordered_map>
#include <functional>
#include <bitset>

namespace {

constexpr size_t KEY_CTRL_BIT = 1UL;
constexpr size_t KEY_SHIFT_BIT = 2UL;
constexpr size_t KEY_ALT_BIT = 4UL;

} // namespace ::

namespace dei::platform::input {

enum class KeyState {
   PRESS = GLFW_PRESS,
   REPEAT = GLFW_REPEAT,
   RELEASE = GLFW_RELEASE,
};

enum class KeyCode {
   ANYTHING = 0,
   LET_A = GLFW_KEY_A,
   LET_B = GLFW_KEY_B,
   LET_C = GLFW_KEY_C,
   LET_D = GLFW_KEY_D,
   LET_E = GLFW_KEY_E,
   LET_F = GLFW_KEY_F,
   LET_G = GLFW_KEY_G,
   LET_H = GLFW_KEY_H,
   LET_I = GLFW_KEY_I,
   LET_J = GLFW_KEY_J,
   LET_K = GLFW_KEY_K,
   LET_L = GLFW_KEY_L,
   LET_M = GLFW_KEY_M,
   LET_N = GLFW_KEY_N,
   LET_O = GLFW_KEY_O,
   LET_P = GLFW_KEY_P,
   LET_Q = GLFW_KEY_Q,
   LET_R = GLFW_KEY_R,
   LET_S = GLFW_KEY_S,
   LET_T = GLFW_KEY_T,
   LET_U = GLFW_KEY_U,
   LET_V = GLFW_KEY_V,
   LET_W = GLFW_KEY_W,
   LET_X = GLFW_KEY_X,
   LET_Y = GLFW_KEY_Y,
   LET_Z = GLFW_KEY_Z,
   LET_1 = GLFW_KEY_1,
   LET_2 = GLFW_KEY_2,
   LET_3 = GLFW_KEY_3,
   LET_4 = GLFW_KEY_4,
   LET_5 = GLFW_KEY_5,
   LET_6 = GLFW_KEY_6,
   LET_7 = GLFW_KEY_7,
   LET_8 = GLFW_KEY_8,
   LET_9 = GLFW_KEY_9,
   LET_0 = GLFW_KEY_0,
   CTRL_LEFT = GLFW_KEY_LEFT_CONTROL,
   SHIFT_LEFT = GLFW_KEY_LEFT_SHIFT,
   ALT_LEFT = GLFW_KEY_LEFT_ALT,
   CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
   TAB = GLFW_KEY_TAB,
   ESCAPE = GLFW_KEY_ESCAPE,
   ENTER = GLFW_KEY_ENTER,
};


using ModifierKeysState = std::bitset<8>;
constexpr ModifierKeysState KEY_MOD_ANYTHING = {0};
constexpr ModifierKeysState KEY_MOD_CTRL = {::KEY_CTRL_BIT};
constexpr ModifierKeysState KEY_MOD_SHIFT = {::KEY_SHIFT_BIT};
constexpr ModifierKeysState KEY_MOD_ALT = {::KEY_ALT_BIT};
constexpr ModifierKeysState CTRL_SHIFT = {::KEY_CTRL_BIT | ::KEY_SHIFT_BIT};

using KeyMap = std::unordered_map<
   std::pair<KeyCode, ModifierKeysState>,
   std::function<void(KeyCode, KeyState, const char*)>,
   pair_hash
>;

}