#pragma once
#include "engine/game.h"

class LinearGenerator : public LevelGenerator {
public:
	virtual ~LinearGenerator() {}
	virtual void generate(Level & level, int level_index);
	virtual void create_types(Game & game);
private:
	const CellType *wall, *floor, *goo, *torch;
	const MonsterType *player, *wander_ant, *still_ant, *scorpion;
	const ObjectType *door, *locked_door, *pot, *well, *gate, *stairs_up, *stairs_down, *trap;
	const ItemType *explosive, *money, *scorpion_tail, *spear, *sharpened_pole, *jacket, *antidote, *apple, *key;
};
