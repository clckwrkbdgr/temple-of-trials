#pragma once
#include "game.h"

class LinearGenerator : public MapGenerator {
public:
	virtual ~LinearGenerator() {}
	virtual void generate(Game & game, int level);
};
