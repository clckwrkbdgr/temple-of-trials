#pragma once
#include "engine/game.h"

class LinearGenerator : public LevelGenerator {
public:
	virtual ~LinearGenerator() {}
	virtual void generate(Level & level, int level_index);
};
