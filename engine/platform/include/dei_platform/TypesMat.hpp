#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/fwd.hpp>
#include <glm/detail/type_mat2x2.hpp>
#include <glm/detail/type_mat3x3.hpp>
#include <glm/detail/type_mat4x4.hpp>


namespace dei {

using mat2f = glm::mat2;
using mat3f = glm::mat3;
using mat4f = glm::mat4;

}