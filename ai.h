#pragma once
#include "engine/ai.h"
class Action;
class Monster;
class Game;

class PlayerControl : public Controller {
public:
	virtual Action * act(Monster & player, Game & game);
};

class AngryAndWander : public Controller {
public:
	virtual Action * act(Monster & monster, Game & game);
};

class AngryAndStill : public Controller {
public:
	virtual Action * act(Monster & monster, Game & game);
};

class CalmAndStill : public Controller {
public:
	virtual Action * act(Monster & monster, Game & game);
};


