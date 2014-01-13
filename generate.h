#pragma once
#include "engine/game.h"

class LinearDungeon : public Chthon::Game {
public:
	LinearDungeon(Chthon::Controller * player_controller);
	virtual ~LinearDungeon() {}
	virtual void generate(Chthon::Level & level, int level_index);
};
