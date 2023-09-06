#include "dei_platform/Prelude.hpp"
#include "dei_platform/Util.hpp"

#include <sstream>

namespace dei::platform {

std::string MakeLibraryFilepath(const char* directoryPath, const char* basename) {
    std::stringstream ss;
    ss << directoryPath << '/';
#if defined(DEI_WINDOWS)
    ss <<  basename << ".dll";
#elif defined(DEI_LINUX)
    ss << "lib" << basename << ".so";
#elif defined(DEI_OSX)
    ss << "lib" << basename << ".dylib";
#endif
    return ss.str();
}

}