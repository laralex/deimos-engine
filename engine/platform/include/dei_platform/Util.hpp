#pragma once

#include <dei_platform/TypesFwd.hpp>

#include <string>
#include <sstream>

namespace dei::platform {

template <typename T>
auto PutIntoStringStream(std::stringstream& ss, T t) -> void
{
    ss << t;
}

template<typename T, typename... Args>
auto PutIntoStringStream(std::stringstream& ss, T t, Args... args) -> void
{
    ss << t;
    PutIntoStringStream(ss, args...) ;
}

template<typename... Args>
auto StringJoin(Args... args) -> std::string {
    std::stringstream ss;
    PutIntoStringStream(ss, args...);
    return ss.str();
}

auto SetSubstringInplace(std::string& destination, const char* source, size_t offset, size_t size, char padValue) -> void;
auto MakeLibraryFilepath(const char* directoryPath, const char* basename) -> std::string;

}