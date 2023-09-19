#pragma once

#include "dei/Prelude.hpp"
#include "glm/mat4x4.hpp"

namespace dei {

extern "C"
auto MakeCamera(f32 translation, vec2 const& rotation) -> mat4;

}