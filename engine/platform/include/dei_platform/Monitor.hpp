#pragma once

#include "Prelude.hpp"
#include "TypesVec.hpp"

#include <vector>
#include <optional>

namespace dei::platform {

// opaque and managed by WindowSystem (i.e. GLFW) !!! mustn't be deleted by the user
using MonitorHandle = GLFWmonitor*;
using MonitorVideoMode = const GLFWvidmode*;
using MonitorGammaRamp = const GLFWgammaramp*;

struct MonitorInfo {
   const char* Name;
   vec2i SizeMillimeters;
   vec2f ContentScale;
   vec2i VirtualPosition;
   vec2i WorkareaPosition;
   vec2i WorkareaSize;

   MonitorGammaRamp GammaRamp;
   MonitorVideoMode VideoModes;
   int NumVideoModes;
};

auto MonitorQueryPrimary(const WindowSystemHandle&) -> MonitorHandle;
auto MonitorQueryAll(const WindowSystemHandle&) -> std::vector<MonitorHandle>;
auto MonitorQueryInfo(const WindowSystemHandle&, const MonitorHandle&) -> std::optional<MonitorInfo>;
auto MonitorSetGamma(const WindowSystemHandle&, const MonitorHandle& monitor, float gamma) -> void;

}