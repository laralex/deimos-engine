#include "dei_platform/Prelude.hpp"
#include "dei_platform/Util.hpp"

#include <sstream>
#include <cstring>

namespace dei::platform {

auto SetSubstringInplace(std::string& destination, const char* source, size_t offset, size_t size, char padValue) -> void {
    if (offset >= destination.size()) {
        return;
    }
    auto begin = destination.data() + offset;
    size = std::min(size, destination.size() - offset);
    size_t sourceSize = 0;
    for (size_t i = 0; i < size; ++i) {
        if (source[i] == '\0') {
            sourceSize = i;
            break;
        }
        begin[i] = source[i];
    }
    for (auto i = sourceSize; i < size; ++i) {
        begin[i] = padValue;
    }
}

auto MakeLibraryFilepath(const char* directoryPath, const char* basename) -> std::string {
    return StringJoin(
    directoryPath, "/"
#if defined(DEI_WINDOWS)
    , basename, ".dll"
#elif defined(DEI_LINUX)
    , "lib", basename, ".so"
#elif defined(DEI_OSX)
    , "lib", basename, ".dylib"
#endif
    );
}

}