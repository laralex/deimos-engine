#pragma once

#include "Prelude.hpp"
#include <GLFW/glfw3.h>
#include <stdint.h>

#if DEI_WINDOWS
#include <windows.h>
#elif DEI_LINUX
#include <unistd.h>
#endif

namespace dei::platform {

auto WindowSwapBuffers(const WindowHandle&) -> void;
inline auto GetTimeSec() -> double {
   return glfwGetTime();
}

inline auto GetElapsedSec(double beginSec) -> double {
   return glfwGetTime() - beginSec;
}

inline auto GetClockCounterUnfused() -> uint64_t {
    // fact call
    return glfwGetTimerValue();
}

inline auto GetClockCounter() -> uint64_t {
    static uint64_t lastClockCounter;
    uint64_t currentClockCounter = glfwGetTimerValue();
    if (currentClockCounter == 0) {
        return lastClockCounter;
    }
    lastClockCounter = currentClockCounter;
    return currentClockCounter;
}


inline auto GetClockFrequencyHertzUnfused() -> uint64_t {
    // fast call
    return glfwGetTimerFrequency();
}

inline auto GetClockFrequencyHertz() -> uint64_t {
    // call and try to fallback if error
    auto freqHz = glfwGetTimerFrequency();
    if (freqHz == 0) {
        // glfw error, hope retry helps
        freqHz = glfwGetTimerFrequency();
    }
    return freqHz;
}

#if DEI_WINDOWS
#define WIN32_LEAN_AND_MEAN

struct SystemClock {
    LARGE_INTEGER time;
    LARGE_INTEGER frequency;
};

inline void GetTimeFrequency(SystemClock& destination) {
    QueryPerformanceCounter(&destination.time);
    QueryPerformanceFrequency(&destination.freq);
}

inline void GetTime(SystemClock& destination){
    QueryPerformanceCounter(&destination.time);
}

inline auto GetElapsedMillisec(const SystemClock& begin) -> std::int64_t {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return (1000 * (end.QuadPart-begin.time.QuadPart)) / begin.frequency.QuadPart;
}

inline auto GetElapsedNanosec(const SystemClock& begin) -> std::int64_t {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return (end.QuadPart-begin.time.QuadPart) / begin.frequency.QuadPart;
}

inline auto ThreadSleepMs(uint32_t ms) -> void {
    Sleep( ms );
}

#elif defined(DEI_LINUX)

auto ThreadSleepMs(uint32_t ms) -> void {
    usleep( ms * 1000 );
}

#endif // DEI_LINUX

}