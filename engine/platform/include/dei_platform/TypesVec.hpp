#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/fwd.hpp>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec4.hpp>

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

}