#pragma once
#include "engine/ai.h"
class Action;
class Monster;
class Game;
class Console;

class PlayerControl : public Controller {
public:
	PlayerControl(Console & console);
	virtual Action * act(Monster & player, Game & game);
private:
	Console & interface;
};

