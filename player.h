#pragma once
#include "engine/ai.h"
namespace Chthon {
	class Action;
	class Monster;
	class Game;
}
class TempleUI;

class PlayerControl : public Chthon::Controller {
public:
	PlayerControl(TempleUI & console);
	virtual Chthon::Action * act(Chthon::Monster & player, Chthon::Game & game);
private:
	TempleUI & interface;
};

