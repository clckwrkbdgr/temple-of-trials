#pragma once
#include "engine/game.h"

class LinearDungeon : public Game {
public:
	LinearDungeon(Controller * player_controller);
	virtual ~LinearDungeon() {}
	virtual void generate(Level & level, int level_index);
};
