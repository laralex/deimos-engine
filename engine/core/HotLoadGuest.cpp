#include "dei/Prelude.hpp"
#include "dei/Entry.hpp"
#include "dei/Camera.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <cr.h>
#pragma clang diagnostic pop

#include <iostream>
#include <functional>

namespace {

struct HotReloadState {
    dei::EngineState EngineState;
    dei::EngineDependencies EngineDependencies;
};

static HotReloadState* state{nullptr};

inline auto OnHotLoad(cr_plugin *ctx) -> int {
    std::cout << "cr::OnHotLoad() v" << ctx->version << " e" << ctx->failure << '\n';
    int err = 0;
    if (state == nullptr) {
        state = reinterpret_cast<HotReloadState*>(ctx->userdata);
        err = err || (dei::EngineColdStartup(state->EngineState, state->EngineDependencies) == false);
        err = err || (dei::EngineHotStartup(state->EngineState) == false);
    } else {
        err = err || (dei::EngineHotStartup(state->EngineState) == false);
    }
    return err;
}

inline auto OnUpdate(cr_plugin *ctx) -> int {
    (void)ctx;
    return dei::EngineTick(state->EngineState) == false;
}

inline auto OnHotUnload(cr_plugin *ctx) -> int {
    std::cout << "cr::OnHotUnload() v" << ctx->version << " e" << ctx->failure << '\n';
    return dei::EngineReleaseResources(state->EngineState) == false;
}

inline auto OnHotTerminate(cr_plugin *ctx) -> int {
    std::cout << "cr::OnHotTerminate() v" << ctx->version << '\n';
    return dei::EngineTerminate(state->EngineState) == false;
}

} // namespace ::

CR_EXPORT
auto cr_main(cr_plugin *ctx, cr_op operation) -> int {
    assert(ctx);
    switch (operation) {
        case CR_STEP:   return OnUpdate(ctx);
        case CR_LOAD:   return OnHotLoad(ctx); // loading back from a reload
        case CR_UNLOAD: return OnHotUnload(ctx); // preparing to a new reload
        case CR_CLOSE:  return OnHotTerminate(ctx); // the plugin will close and not reload anymore
    }
}

