#pragma once

#include "dei/Prelude.hpp"
#include "dei_platform/TypesMat.hpp"

namespace dei {

extern "C"
auto MakeCamera(f32 translation, vec2f const& rotation) -> mat4f;

}