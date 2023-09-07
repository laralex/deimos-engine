#include "dei_platform/Util.hpp"
#include "dei_platform/Window.hpp"

#define CR_HOST CR_UNSAFE // required in the host only and before including cr.h
#include <cr.h>

#include <cassert>
#include <string>

struct EngineState {
    std::uint32_t DrawCounter{0};
};

// args:
// 1: install directory path (absolute)
// 2: engine library basename (e.g. dei)
// 3: frequency of hot reload (in draw calls)
auto main(int argc, char *argv[]) -> int {
    // parse args
    assert(argc >= 3);
    auto hotReloadFrequency = (argc >= 4 ? std::stoi(argv[3]) : 100000);

    // make window
    auto windowSystem = dei::platform::CreateWindowSystem();
    auto window = dei::platform::CreateWindow({800, 600, "Vulkan window"});
    if (window == nullptr) {
        exit(1);
    }

    // set up hot reloading
    auto engineHotReloader = cr_plugin{};
    auto engineLibPath = dei::platform::MakeLibraryFilepath(argv[1], argv[2]);
    assert(cr_plugin_open(engineHotReloader, engineLibPath.c_str())); // the full path to library
    auto engineState = EngineState{};
    engineHotReloader.userdata = static_cast<void*>(&engineState);
    printf("Hot-loadable library: %s\n", engineLibPath.c_str());

    // app loop
    auto windowClosing{false}, engineClosing{false}, hotReloadCrashing{false};
    do {
        dei::platform::PollWindowEvents(window);

        {
            auto doReloadCheck = (engineState.DrawCounter % hotReloadFrequency) == 0;
            auto engineAnswer = cr_plugin_update(engineHotReloader, doReloadCheck);
            switch (engineAnswer) {
                case 0: break;
                case -1: printf("dei::cr::ERROR_UPDATE\n"); hotReloadCrashing = true; break;
                case -2: printf("dei::cr::ERROR_LOAD_UNLOAD=-2\n"); hotReloadCrashing = true; break;
                default: printf("dei::cr::answer=%d\n", engineAnswer); engineClosing = true; break;
            }
        }

        windowClosing = dei::platform::IsWindowClosing(window);
    } while(!(windowClosing || engineClosing || hotReloadCrashing));

    printf("windowClose=%d engineClose=%d hotReloadCrash=%d\n", windowClosing, engineClosing, hotReloadCrashing);

    // tear down hot reloading
    cr_plugin_close(engineHotReloader);

    return 0;
}