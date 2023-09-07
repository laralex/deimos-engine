#pragma once

#include "dei/Prelude.hpp"
#include "glm/mat4x4.hpp"

namespace dei {

extern "C"
auto MakeCamera(f32 Translate, vec2 const& Rotate) -> mat4;

}