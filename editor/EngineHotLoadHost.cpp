#include "dei_platform/Util.hpp"
#include "dei_platform/Window.hpp"
#include "dei_platform/Time.hpp"
#include "dei_platform/Mouse.hpp"

using namespace dei::platform::input;

#define CR_HOST CR_UNSAFE // required in the host only and before including cr.h
#include <cr.h>

#include <cassert>
#include <string>
#include <iostream>

struct EngineState {
    std::uint32_t DrawCounter{0};
};

auto OnTextInput(const std::string& currentInputUtf8, uint32_t latestCodepoint) {
    std::cout << currentInputUtf8 << '\n';
}

auto OnMouseMoved(double windowX, double windowY) {
    //std::cout << '(' << windowX << ',' << windowY << ")\n";
}

auto OnMouseButton(MouseButton button, MouseButtonState state) {
    if (state == MouseButtonState::PRESS) {
        printf("Press mouse %d %d\n", button);
    }
}

auto OnKeyboardDefault(KeyCode key, KeyState state, const char* keyName) -> void {
    if (state == KeyState::PRESS) {
        printf("Press key %s %d\n", keyName, key);
    }
}

auto OnKeyboardQ(KeyCode key, KeyState state, const char* keyName) -> void {
    if (state == KeyState::PRESS) {
        printf("QQQQQQQQQQQQQQQQQQQQQQQ\n");
    }
}

auto OnKeyboardR(KeyCode key, KeyState state, const char* keyName) -> void {
    if (state == KeyState::PRESS) {
        printf("RRRRRRRRRRRRRRRRRRRRRRR\n");
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
    constexpr double FPS_CAP = 300.0;
    constexpr double TICK_CAP_SECONDS = 1.0 / FPS_CAP;

    // make window
    auto windowSystem = dei::platform::CreateWindowSystem();
    auto windowTitle = dei::platform::StringJoin("My window: #frame=1234567890 time=1234567890");
    constexpr auto WINTITLE_FRAME_OFFSET = 18, WINTITLE_FRAME_SIZE = 10;
    constexpr auto WINTITLE_TIME_OFFSET = 34, WINTITLE_TIME_SIZE = 10;
    auto windowBuilder = dei::platform::WindowBuilder{};
    windowBuilder
        .WithGraphicsBackend(dei::platform::CreateWindowArgs::GraphicsBackend::VULKAN)
        .WithDimensions(800, 600)
        .WithTitleUtf8(windowTitle.c_str())
        .WithInputTextCallback(&OnTextInput)
        .WithMousePositionCallback(&OnMouseMoved)
        .WithMouseButtonCallback(&OnMouseButton);
    auto maybeWindow = dei::platform::CreateWindow(windowSystem, std::move(windowBuilder));
    if (maybeWindow == std::nullopt) {
        exit(1);
    }
    auto window = *std::move(maybeWindow);

    dei::platform::WindowSetKeyMap(window, {
        {{KeyCode::KEY_Q, MODIFIERS_ALT}, &OnKeyboardQ},
        {{KeyCode::KEY_R, MODIFIERS_CTRL_SHIFT}, &OnKeyboardR},
        {{KeyCode::ANYTHING, MODIFIERS_NONE}, &OnKeyboardDefault},
        // TODO: doesn't work with BACKSPACE (-1 code)
        {{KeyCode::BACKSPACE, MODIFIERS_CTRL}, [&](KeyCode key, KeyState state, const char* keyName){
            if (state == KeyState::PRESS) {
                dei::platform::WindowClearInput(window);
            }
        }},
        {{KeyCode::BACKSPACE, MODIFIERS_NONE}, [&](KeyCode key, KeyState state, const char* keyName){
            if (state == KeyState::PRESS) {
                dei::platform::WindowUndoInput(window);
            }
        }},
        {{KeyCode::INSERT, MODIFIERS_SHIFT}, [&](KeyCode key, KeyState state, const char* keyName){
            if (state == KeyState::PRESS) {
                auto* clipboard = dei::platform::GetClipboardUtf8(windowSystem);
                dei::platform::WindowAppendInputUtf8(window, clipboard);
            }
        }},
    });

    // set up hot reloading
    auto engineHotReloader = cr_plugin{};
    auto engineLibPath = dei::platform::MakeLibraryFilepath(argv[1], argv[2]);
    assert(cr_plugin_open(engineHotReloader, engineLibPath.c_str())); // the full path to library
    auto engineState = EngineState{};
    engineHotReloader.userdata = static_cast<void*>(&engineState);
    printf("Hot-loadable library: %s\n", engineLibPath.c_str());

    // app loop
    auto windowClosing{false}, engineClosing{false}, hotReloadCrashing{false};
    auto updateWindowTitleEvery = 10;
    auto beginTimeSeconds = dei::platform::GetTimeSec();
    do {
        dei::platform::PollWindowEvents(windowSystem);
        if (engineState.DrawCounter % updateWindowTitleEvery == 0) {
            auto&& drawCounterStr = std::to_string(engineState.DrawCounter);
            dei::platform::SetSubstringInplace(windowTitle,
                drawCounterStr.c_str(), WINTITLE_FRAME_OFFSET, WINTITLE_FRAME_SIZE, ' ');
            auto&& timeSecStr = std::to_string(dei::platform::GetClockCounter() / dei::platform::GetClockFrequencyHertz());
            dei::platform::SetSubstringInplace(windowTitle,
                timeSecStr.c_str(), WINTITLE_TIME_OFFSET, WINTITLE_TIME_SIZE, ' ');
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

        dei::platform::WindowSwapBuffers(window);
        windowClosing = dei::platform::WindowIsClosing(window);

        auto nowSeconds = dei::platform::GetTimeSec();
        auto deltaSeconds = nowSeconds - beginTimeSeconds;
        if (deltaSeconds < TICK_CAP_SECONDS) {
            auto remainingMillisec = static_cast<uint32_t>(1000.0*(TICK_CAP_SECONDS - deltaSeconds)) ;
            if (remainingMillisec > 0) {
                dei::platform::ThreadSleepMs(remainingMillisec);
            }
        }

        nowSeconds = dei::platform::GetTimeSec();
        deltaSeconds = nowSeconds - beginTimeSeconds;
        beginTimeSeconds = nowSeconds;
    } while(!(windowClosing || engineClosing || hotReloadCrashing));

    printf("windowClose=%d engineClose=%d hotReloadCrash=%d\n", windowClosing, engineClosing, hotReloadCrashing);

    // tear down hot reloading
    cr_plugin_close(engineHotReloader);

    return 0;
}