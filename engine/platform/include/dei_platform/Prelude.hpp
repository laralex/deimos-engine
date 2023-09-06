#pragma once

#if defined(_WIN32)
#define DEI_WINDOWS
#elif defined(__linux__)
#define DEI_LINUX
#elif defined(__APPLE__)
#define DEI_OSX
#else
#error "Unsupported platform"
#endif // DEI_WINDOWS || DEI_LINUX || DEI_OSX