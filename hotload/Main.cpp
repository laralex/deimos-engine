#if defined(_WIN32)
#define DEI_WINDOWS
#elif defined(__linux__)
#define DEI_LINUX
#elif defined(__APPLE__)
#define DEI_OSX
#else
#error "Unsupported platform"
#endif // DEI_WINDOWS || DEI_LINUX || DEI_OSX


#define CR_HOST // required in the host only and before including cr.h
#include "cr.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <string>
#include <sstream>

#ifndef INSTALL_PATH
#define INSTALL_PATH ""
#endif

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

struct AppState {
    std::uint32_t DrawCounter{0};
};

struct EngineState {

};

// args:
// 1: install directory path (absolute)
// 2: engine library basename (e.g. dei)
// 3: application library basename (e.g. deiapp)
// 4: frequency of hot reload (in draw calls)
int main(int argc, char *argv[]) {
    // the host application should initalize a plugin with a context, a plugin
    cr_plugin appHotReloader, engineHotReloader;
    std::uint32_t hotReloadFrequency = (argc >= 5 ? std::stoi(argv[4]) : 100000);

    assert(argc >= 4);

    std::string engineLibPath = MakeLibraryFilepath(argv[1], argv[2]);
    assert(cr_plugin_open(engineHotReloader, engineLibPath.c_str())); // the full path to library
    printf("Hot-loadable library: %s\n", engineLibPath.c_str());
    EngineState engineState;
    engineHotReloader.userdata = static_cast<void*>(&engineState);

    std::string appLibPath = MakeLibraryFilepath(argv[1], argv[3]);
    assert(cr_plugin_open(appHotReloader, appLibPath.c_str())); // library base name (e.g. libpython.so pass as python)
    printf("Hot-loadable library: %s\n", appLibPath.c_str());
    AppState appState;
    appHotReloader.userdata = static_cast<void*>(&appState);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    bool windowClosing{0}, appClosing{0}, engineClosing{0}, hotReloadCrashing{0};

    do {
        glfwPollEvents();
        windowClosing = glfwWindowShouldClose(window);
        bool doReloadCheck = appState.DrawCounter % hotReloadFrequency == 0;

        {
            int appAnswer = cr_plugin_update(appHotReloader, doReloadCheck);
            switch (appAnswer) {
                case 0: break;
                case -1: printf("app::cr::ERROR_UPDATE\n"); hotReloadCrashing = true; break;
                case -2: printf("app::cr::ERROR_LOAD_UNLOAD=-2\n"); hotReloadCrashing = true; break;
                default: printf("app::cr::answer=%d\n", appAnswer); appClosing = true; break;
            }
        }
        
        {
            int engineAnswer = cr_plugin_update(engineHotReloader, doReloadCheck);
            switch (engineAnswer) {
                case 0: break;
                case -1: printf("dei::cr::ERROR_UPDATE\n"); hotReloadCrashing = true; break;
                case -2: printf("dei::cr::ERROR_LOAD_UNLOAD=-2\n"); hotReloadCrashing = true; break;
                default: printf("dei::cr::answer=%d\n", engineAnswer); engineClosing = true; break;
            }
        }
        
    } while(!(windowClosing || appClosing || engineClosing || hotReloadCrashing));

    printf("windowClose=%d appClose=%d engineClose=%d hotReloadCrash=%d", windowClosing, appClosing, engineClosing, hotReloadCrashing);

    glfwDestroyWindow(window);

    glfwTerminate();

    cr_plugin_close(appHotReloader);
    cr_plugin_close(engineHotReloader);
    return 0;
}