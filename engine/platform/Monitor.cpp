#include "dei_platform/Monitor.hpp"

namespace dei::platform {

auto MonitorQueryPrimary(const WindowSystemHandle&) -> MonitorHandle {
   return glfwGetPrimaryMonitor();
}

auto MonitorQueryAll(const WindowSystemHandle&) -> std::vector<MonitorHandle> {
   int numOfMonitors;
   MonitorHandle* monitorsArray = glfwGetMonitors(&numOfMonitors);
   return std::vector<MonitorHandle>{monitorsArray, monitorsArray + numOfMonitors};
}

auto MonitorQueryInfo(const WindowSystemHandle&, const MonitorHandle monitor) -> std::optional<MonitorInfo> {
   if (monitor == nullptr) {
      return std::nullopt;
   }
   MonitorInfo info;
   info.Name = glfwGetMonitorName(monitor);
   glfwGetMonitorPhysicalSize(monitor, &info.SizeMillimeters.width, &info.SizeMillimeters.height);
   glfwGetMonitorContentScale(monitor, &info.ContentScale.x, &info.ContentScale.y);
   glfwGetMonitorPos(monitor, &info.VirtualPosition.x, &info.VirtualPosition.y);
   glfwGetMonitorWorkarea(monitor,
      &info.WorkareaPosition.x, &info.WorkareaPosition.y,
      &info.WorkareaSize.width, &info.WorkareaSize.height);
   info.GammaRamp = glfwGetGammaRamp(monitor);
   info.VideoModes = glfwGetVideoModes(monitor, &info.NumVideoModes);
   return info;
}

auto MonitorSetGamma(const WindowSystemHandle&, const MonitorHandle& monitor, float gamma) -> void {
   if (monitor == nullptr) {
      return;
   }
   glfwSetGamma(monitor, gamma);
}

}