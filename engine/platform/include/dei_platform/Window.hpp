#pragma once

#include <stddef.h>

namespace dei::platform {

bool CreateWindow(size_t width, size_t height, const char* title);
void DestroyWindow();
void PollWindowEvents();
bool IsWindowClosing();

}