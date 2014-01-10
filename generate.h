#pragma once
#include "engine/game.h"

class LinearDungeon : public Game {
public:
	LinearDungeon();
	virtual ~LinearDungeon() {}
	virtual void generate(Level & level, int level_index);
};
