#include "dei/Prelude.hpp"

#include <cr.h>

#include <iostream>

struct HotReloadState {

};

static HotReloadState* state;

int OnLoad(cr_plugin *ctx) {
    std::cout << "dei::OnLoad() v" << ctx->version << " e" << ctx->failure << '\n';
    state = reinterpret_cast<HotReloadState*>(ctx->userdata);
    return 0;
}

int OnUpdate(cr_plugin *ctx) {
    return 0;
}

int OnUnload(cr_plugin *ctx) {
    std::cout << "dei::OnUnload() v" << ctx->version << " e" << ctx->failure << '\n';
    return 0;
}

int OnClose(cr_plugin *ctx) {
    std::cout << "dei::OnClose() v" << ctx->version << '\n';
    return 0;
}

CR_EXPORT int cr_main(cr_plugin *ctx, cr_op operation) {
    assert(ctx);
    switch (operation) {
        case CR_STEP:   return OnUpdate(ctx);
        case CR_LOAD:   return OnLoad(ctx); // loading back from a reload
        case CR_UNLOAD: return OnUnload(ctx); // preparing to a new reload
        case CR_CLOSE:  return OnClose(ctx); // the plugin will close and not reload anymore
    }
}

