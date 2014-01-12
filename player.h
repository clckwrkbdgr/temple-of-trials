#pragma once
#include "engine/ai.h"
class Action;
class Monster;
class Game;
class TempleUI;

class PlayerControl : public Controller {
public:
	PlayerControl(TempleUI & console);
	virtual Action * act(Monster & player, Game & game);
private:
	TempleUI & interface;
};

