#pragma once

#include "dei/Prelude.hpp"

namespace dei {

extern "C"
auto MakeCamera(f32 translation, vec2f const& rotation) -> mat4f;

}