#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#define stringify( name ) # name

#if defined(_WIN32)
#define DEI_WINDOWS 1
#elif defined(__linux__)
#define DEI_LINUX 1
#elif defined(__APPLE__)
#define DEI_OSX 1
#else
#error "Unsupported platform"
#endif // DEI_WINDOWS || DEI_LINUX || DEI_OSX

namespace dei::platform {

enum class GraphicsApi {
   OPENGL = GLFW_OPENGL_API,
   OPENGLES = GLFW_OPENGL_ES_API,
   VULKAN,
};

constexpr const char* GraphicsApiToStr(GraphicsApi backend) {
   switch (backend) {
      case GraphicsApi::VULKAN: return stringify(GraphicsApi::VULKAN);
      case GraphicsApi::OPENGL: return stringify(GraphicsApi::OPENGL);
      case GraphicsApi::OPENGLES: return stringify(GraphicsApi::OPENGLES);
   }
   printf("Unreachable code reached: GraphicsApiToStr on value %d", static_cast<int>(backend));
   std::exit(1);
};

struct ivec2 {
   int x, y;
};

struct vec2 {
   float x, y;
};

struct dvec2 {
   double x, y;
};

struct isize2 {
   int width, height;
};

using WindowSystemHandle = std::shared_ptr<std::nullptr_t>;

struct WindowDestroyer {
   auto operator()(GLFWwindow* window) -> void;
};
using WindowHandle = std::unique_ptr<GLFWwindow, WindowDestroyer>;

// from boost (functional/hash):
// see http://www.boost.org/doc/libs/1_35_0/doc/html/hash/combine.html template
template <class T> inline void hash_combine(size_t &seed, T const &v) {
    seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct pair_hash {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2> &p) const {
        size_t seed = 0;
        hash_combine(seed, p.first);
        hash_combine(seed, p.second);
        return seed;
    }
};

};
