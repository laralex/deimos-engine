#pragma once

#include <cstdint>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/fwd.hpp>

using b8 = bool;
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

namespace dei {

using vec2f = glm::vec2;
using vec3f = glm::vec3;
using vec4f = glm::vec4;

using vec2ff = glm::dvec2;
using vec3ff = glm::dvec3;
using vec4ff = glm::dvec4;

using vec2u = glm::uvec2;
using vec3u = glm::uvec3;
using vec4u = glm::uvec4;

using vec2i = glm::ivec2;
using vec3i = glm::ivec3;
using vec4i = glm::ivec4;

using mat2f = glm::mat2;
using mat3f = glm::mat3;
using mat4f = glm::mat4;

}