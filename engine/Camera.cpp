#include "dei/Camera.hpp"

#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

extern "C" mat4 dei::camera(f32 Translate, vec2 const& Rotate)
{
   mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
   mat4 View = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -Translate));
   View = glm::rotate(View, Rotate.y, vec3(-1.0f, 0.0f, 0.0f));
   View = glm::rotate(View, Rotate.x, vec3(0.0f, 1.0f, 0.0f));
   mat4 Model = glm::scale(mat4(1.0f), vec3(0.5f));
   return Projection * View * Model;
}