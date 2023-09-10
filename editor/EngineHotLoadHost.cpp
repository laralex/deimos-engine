#include "dei_platform/Util.hpp"
#include "dei_platform/Window.hpp"

#define CR_HOST CR_UNSAFE // required in the host only and before including cr.h
#include <cr.h>

#include <cassert>
#include <string>

struct EngineState {
    std::uint32_t DrawCounter{0};
};

auto OnKeyboardDefault(dei::platform::KeyCode key, dei::platform::KeyState state) -> void {
    using namespace dei::platform;
    if (state == KeyState::RELEASE) {
        printf("Release key\n");
    } else if (state == KeyState::PRESS) {
        printf("Press key\n");
    }
}

auto OnKeyboardQ(dei::platform::KeyCode key, dei::platform::KeyState state) -> void {
    using namespace dei::platform;
    if (state == KeyState::PRESS) {
        printf("QQQ\n");
    }
}
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
    auto windowTitle = dei::platform::StringJoin("My window: #frame=XXXXXXXXXX");
    auto titleOffsetFrame = 18, titleSizeFrame = 10;
    auto windowBuilder = dei::platform::WindowBuilder{};
    windowBuilder
        .WithGraphicsBackend(dei::platform::CreateWindowArgs::GraphicsBackend::VULKAN)
        .WithDimensions(800, 600)
        .WithTitleUtf8(windowTitle.c_str())
        .WithKeymap({
            {dei::platform::KeyCode::LET_Q, &OnKeyboardQ},
            {dei::platform::KeyCode::ANYTHING, &OnKeyboardDefault},
        });
    auto maybeWindow = dei::platform::CreateWindow(windowSystem, std::move(windowBuilder));
    if (maybeWindow == std::nullopt) {
        exit(1);
    }
    auto window = *std::move(maybeWindow);

    // set up hot reloading
    auto engineHotReloader = cr_plugin{};
    auto engineLibPath = dei::platform::MakeLibraryFilepath(argv[1], argv[2]);
    assert(cr_plugin_open(engineHotReloader, engineLibPath.c_str())); // the full path to library
    auto engineState = EngineState{};
    engineHotReloader.userdata = static_cast<void*>(&engineState);
    printf("Hot-loadable library: %s\n", engineLibPath.c_str());

    // app loop
    auto windowClosing{false}, engineClosing{false}, hotReloadCrashing{false};
    auto updateWindowTitleEvery = 1000;
    do {
        dei::platform::PollWindowEvents(windowSystem);
        if (engineState.DrawCounter % updateWindowTitleEvery == 0) {
            auto&& drawCounterStr = std::to_string(engineState.DrawCounter);
            dei::platform::SetSubstringInplace(windowTitle,
                drawCounterStr.c_str(), titleOffsetFrame, titleSizeFrame, ' ');
            dei::platform::WindowSetTitleUtf8(window, windowTitle.c_str());
        }
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

        windowClosing = dei::platform::WindowIsClosing(window);
    } while(!(windowClosing || engineClosing || hotReloadCrashing));

    printf("windowClose=%d engineClose=%d hotReloadCrash=%d\n", windowClosing, engineClosing, hotReloadCrashing);

    // tear down hot reloading
    cr_plugin_close(engineHotReloader);

    return 0;
}