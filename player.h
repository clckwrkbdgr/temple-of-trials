#pragma once
#include "engine/ai.h"
class Action;
class Monster;
class Game;

class PlayerControl : public Controller {
public:
	virtual Action * act(Monster & player, Game & game);
};

