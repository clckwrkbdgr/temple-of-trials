#pragma once
#include "engine/game.h"

class LinearDungeon : public Dungeon {
public:
	LinearDungeon();
	virtual ~LinearDungeon() {}
	virtual void generate(Level & level, int level_index);
};
