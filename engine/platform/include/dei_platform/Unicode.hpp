#pragma once

#include "dei_platform/TypesFwd.hpp"

#include <iostream>
#include <locale>
#include <string>

namespace {

template<class Facet>
struct deletable_facet : Facet
{
    template<class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

}
namespace dei::platform {

// https://gist.github.com/alexanderchuranov/9f4677b9b48d9936d83c
struct ConvertUtf32Utf8
{
  auto operator()(std::u32string const& source) -> std::string
  {
    return conv.to_bytes(source);
  }

  std::wstring_convert<std::codecvt<char32_t, char, std::mbstate_t>, char32_t> conv;
};

struct AppendToUtf8
{
  auto operator()(std::string& dest, u32 codepoint) -> b8
  {
    auto source = static_cast<char32_t>(codepoint);
    auto const* sourceNext = &source;
    char buffer[4];
    char* destNext = buffer;
    std::mbstate_t state{};

    auto convOut = conv.out(
                 state, &source, &source + 1, sourceNext,
                 buffer, buffer + sizeof(buffer), destNext);
    if(convOut != std::codecvt_base::result::ok) {
        return false;
    }
    if (destNext <= buffer) {
      return false;
    }
    dest.append(buffer, static_cast<size_t>(destNext - buffer));
    return true;
  }
  deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>> conv;
};

}