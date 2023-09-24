#include "dei/Camera.hpp"

#include "dei_platform/TypesVec.hpp"
#include "dei_platform/TypesMat.hpp"

#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace dei {

extern "C"
auto MakeCamera(f32 translation, vec2f const& rotation) -> mat4f
{
   auto projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
   auto view = glm::translate(mat4f(1.0f), vec3f(0.0f, 0.0f, -translation));
   view = glm::rotate(view, rotation.y, vec3f(-1.0f, 0.0f, 0.0f));
   view = glm::rotate(view, rotation.x, vec3f(0.0f, 1.0f, 0.0f));
   auto model = glm::scale(mat4f(1.0f), vec3f(0.5f));
   return projection * view * model;
}

}