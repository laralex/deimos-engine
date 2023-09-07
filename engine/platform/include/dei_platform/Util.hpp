#pragma once

#include <string>

namespace dei::platform {

auto MakeLibraryFilepath(const char* directoryPath, const char* basename) -> std::string;

}