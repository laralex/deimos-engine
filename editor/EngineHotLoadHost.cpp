#include "dei_platform/Util.hpp"
#include "dei_platform/Window.hpp"
#include "dei_platform/Time.hpp"
#include "dei_platform/Mouse.hpp"
#include "dei_platform/Monitor.hpp"

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

auto OnWindowResized(int newWidthPx, int newHeightPx) {
    printf("New size %d %d\n", newWidthPx, newHeightPx);

}

auto OnWindowMoved(int leftUpCornerX, int leftUpCornerY) {
    printf("New pos %d %d\n", leftUpCornerX, leftUpCornerY);
}

auto OnMouseMoved(double windowX, double windowY) {
    //std::cout << '(' << windowX << ',' << windowY << ")\n";
}

auto OnMouseScrolled(double directionX, double directionY) {
    //std::cout << '[' << directionX << ',' << directionY << "]\n";
}

auto OnMouseEnteredWindow(bool entered) {
    std::cout << (entered ? "MOUSE ENTERED" : "MOUSE LEFT") << std::endl;
}

auto OnMouseButton(MouseButton button, MouseButtonState state) {
    if (state == MouseButtonState::PRESS) {
        printf("Press mouse %d\n", button);
    }
}

auto OnKeyboardDefault(KeyCode key, KeyState state, const char* keyName) -> void {
    if (state == KeyState::PRESS) {
        printf("Press key %s %d\n", keyName, key);
    }
}

auto OnKeyboardR(KeyCode key, KeyState state, const char* keyName) -> void {
    if (state == KeyState::PRESS) {
        printf("RRRRRRRRRRRRRRRRRRRRRRR\n");
    }
}

auto OnWindowClosing() {
    printf("GLFW Window closing\n");
}

auto OnWindowFocused(bool isFocused) {
    printf("Window focused: %d\n", isFocused);
}

auto OnWindowError(int code, const char* description) {
    printf("Error GLFW: %s\n", description);
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
    auto windowSystem = dei::platform::CreateWindowSystem(&OnWindowError);
    // NOTE: mustn't use when benchmarking
    dei::platform::SetVerticalSync(windowSystem, true);
    auto primaryMonitor = dei::platform::MonitorQueryPrimary(windowSystem);
    assert(primaryMonitor != nullptr);
    auto monitorInfo = *dei::platform::MonitorQueryInfo(windowSystem, primaryMonitor);

    printf("Monitor info: %s (size millimeters %dx%d)\n", monitorInfo.Name,
        monitorInfo.WorkareaSize.width, monitorInfo.WorkareaSize.height);
    if (monitorInfo.NumVideoModes > 0) {
        auto videoMode = monitorInfo.VideoModes[monitorInfo.NumVideoModes - 1];
        printf("Monitor video mode: %dx%d %d Hz, bits R=%d G=%d B=%d\n",
            videoMode.width, videoMode.height, videoMode.refreshRate,
            videoMode.redBits, videoMode.blueBits, videoMode.greenBits);
    }

    auto startupClockCounter = dei::platform::GetClockCounter();
    auto windowTitle = dei::platform::StringJoin("My window: #frame=1234567890 time=1234567890");
    constexpr auto WINTITLE_FRAME_OFFSET = 18, WINTITLE_FRAME_SIZE = 10;
    constexpr auto WINTITLE_TIME_OFFSET = 34, WINTITLE_TIME_SIZE = 10;

    dei::platform::FullscreenMode windowFullscreenMode = dei::platform::FullscreenMode::WINDOWED;
    auto windowBuilder = dei::platform::WindowBuilder{};
    windowBuilder
        .WithGraphicsBackend(dei::platform::GraphicsBackend::VULKAN)
        .WithSize(800, 600)
        .WithSizeMin(200, 200)
        .WithTitleUtf8(windowTitle.c_str())
        .WithInputTextCallback(&OnTextInput)
        .WithPositionCallback(&OnWindowMoved)
        .WithResizeCallback(&OnWindowResized)
        .WithClosingCallback(&OnWindowClosing)
        .WithFocus(false)
        .WithFocusCallback(&OnWindowFocused)
        .WithRawMouseMotion(true)
        .WithFullscreen(windowFullscreenMode == dei::platform::FullscreenMode::FULLSCREEN ? primaryMonitor : nullptr)
        .WithMousePositionCallback(&OnMouseMoved)
        .WithMouseScrollCallback(&OnMouseScrolled)
        .WithMouseButtonCallback(&OnMouseButton)
        .WithMouseEntersWindowCallback(&OnMouseEnteredWindow);
    auto maybeWindow = dei::platform::CreateWindow(windowSystem, std::move(windowBuilder));
    if (maybeWindow == std::nullopt) {
        printf("Window creation failed");
        exit(1);
    }
    auto window = *std::move(maybeWindow);
    dei::platform::WindowRequestAttention(window);

    dei::platform::WindowSetKeyMap(window, {
        {{KeyCode::ENTER, MODIFIERS_ALT}, [&](KeyCode key, KeyState state, const char* keyName) {
            using dei::platform::FullscreenMode;
            if (state != KeyState::PRESS) return;
            auto nextFullscreenMode = static_cast<FullscreenMode>(
                (static_cast<int>(windowFullscreenMode) + 1) % 3);
            dei::platform::WindowSetFullscreenMode(window, nextFullscreenMode, primaryMonitor);
            windowFullscreenMode = nextFullscreenMode;
        }},
        {{KeyCode::KEY_P, MODIFIERS_ALT}, [&](KeyCode key, KeyState state, const char* keyName) {
            if (state != KeyState::PRESS) return;
            dei::platform::WindowSetVisible(window,
                !dei::platform::WindowIsVisible(window));
        }},
        {{KeyCode::KEY_0, MODIFIERS_ALT}, [&](KeyCode key, KeyState state, const char* keyName) {
            static bool isVerticalSyncEnabled = false;
            if (state != KeyState::PRESS) return;
            isVerticalSyncEnabled ^= 1;
            dei::platform::SetVerticalSync(windowSystem, isVerticalSyncEnabled);
        }},
        {{KeyCode::KEY_C, MODIFIERS_ALT}, [&](KeyCode key, KeyState state, const char* keyName) {
            using dei::platform::input::CursorMode;
            if (state != KeyState::PRESS) return;
            bool shouldEnableCursor = dei::platform::WindowGetCursorMode(window) == CursorMode::DISABLED;
            dei::platform::WindowSetCursorMode(window,
                shouldEnableCursor ? CursorMode::NORMAL : CursorMode::DISABLED
            );
        }},
        {{KeyCode::KEY_R, MODIFIERS_CTRL_SHIFT}, &OnKeyboardR},
        {{KeyCode::ANYTHING, MODIFIERS_NONE}, &OnKeyboardDefault},
        // TODO: doesn't work with BACKSPACE (-1 code)
        {{KeyCode::BACKSPACE, MODIFIERS_CTRL}, [&](KeyCode key, KeyState state, const char* keyName){
            if (state != KeyState::PRESS) return;
            dei::platform::WindowClearInput(window);
        }},
        {{KeyCode::BACKSPACE, MODIFIERS_NONE}, [&](KeyCode key, KeyState state, const char* keyName){
            if (state != KeyState::PRESS) return;
            dei::platform::WindowUndoInput(window);
        }},
        {{KeyCode::INSERT, MODIFIERS_SHIFT}, [&](KeyCode key, KeyState state, const char* keyName){
            if (state != KeyState::PRESS) return;
            auto* clipboard = dei::platform::GetClipboardUtf8(windowSystem);
            dei::platform::WindowAppendInputUtf8(window, clipboard);
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
    auto updateWindowTitleEvery = 100;
    auto beginTimeSeconds = dei::platform::GetTimeSec();
    do {
        dei::platform::PollWindowEvents(windowSystem);
        if (engineState.DrawCounter % updateWindowTitleEvery == 0) {
            auto&& drawCounterStr = std::to_string(engineState.DrawCounter);
            dei::platform::SetSubstringInplace(windowTitle,
                drawCounterStr.c_str(), WINTITLE_FRAME_OFFSET, WINTITLE_FRAME_SIZE, ' ');
            auto&& timeSecStr = std::to_string(
                (dei::platform::GetClockCounter() - startupClockCounter) / dei::platform::GetClockFrequencyHertz());
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