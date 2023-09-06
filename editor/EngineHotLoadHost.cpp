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
    if (false == dei::platform::CreateWindow(800, 600, "Vulkan window")) {
        exit(1);
    }

    // the host application should initalize a plugin with a context, a plugin
    auto engineHotReloader = cr_plugin{};
    auto hotReloadFrequency = (argc >= 4 ? std::stoi(argv[3]) : 100000);

    assert(argc >= 3);

    auto engineLibPath = dei::platform::MakeLibraryFilepath(argv[1], argv[2]);
    assert(cr_plugin_open(engineHotReloader, engineLibPath.c_str())); // the full path to library
    printf("Hot-loadable library: %s\n", engineLibPath.c_str());
    auto engineState = EngineState{};
    engineHotReloader.userdata = static_cast<void*>(&engineState);

    auto windowClosing{false}, engineClosing{false}, hotReloadCrashing{false};

    do {
        dei::platform::PollWindowEvents();
        windowClosing = dei::platform::IsWindowClosing();
        auto doReloadCheck = (engineState.DrawCounter % hotReloadFrequency) == 0;

        {
            auto engineAnswer = cr_plugin_update(engineHotReloader, doReloadCheck);
            switch (engineAnswer) {
                case 0: break;
                case -1: printf("dei::cr::ERROR_UPDATE\n"); hotReloadCrashing = true; break;
                case -2: printf("dei::cr::ERROR_LOAD_UNLOAD=-2\n"); hotReloadCrashing = true; break;
                default: printf("dei::cr::answer=%d\n", engineAnswer); engineClosing = true; break;
            }
        }

    } while(!(windowClosing || engineClosing || hotReloadCrashing));

    printf("windowClose=%d engineClose=%d hotReloadCrash=%d\n", windowClosing, engineClosing, hotReloadCrashing);

    cr_plugin_close(engineHotReloader);

    dei::platform::DestroyWindow();
    return 0;
}