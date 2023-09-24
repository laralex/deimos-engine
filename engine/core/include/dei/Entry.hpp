#pragma once

#include "dei/Prelude.hpp"

namespace dei {

b8 EngineColdStartup(EngineState& destinationState, const EngineDependencies& dependencies);

b8 EngineHotStartup(EngineState& engineState);

b8 EngineTick(EngineState& engineState);

b8 EngineReleaseResources(EngineState& engineState);

b8 EngineTerminate(EngineState& engineState);

}