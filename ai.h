#pragma once
#include "engine/game.h"

namespace AI {
	enum { DUMMY, PLAYER, ANGRY_AND_WANDER, ANGRY_AND_STILL, CALM_AND_STILL };
}

Controller get_controller(int ai);

