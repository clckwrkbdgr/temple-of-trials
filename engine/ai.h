#pragma once
#include <map>
class Game;
class Monster;
class Action;

class Controller {
public:
	virtual ~Controller() {}
	virtual Action * act(Monster &, Game &) = 0;
};

struct ControllerFactory {
	~ControllerFactory();
	void add_controller(int ai, Controller * controller);
	Controller * get_controller(int ai) const;
private:
	std::map<int, Controller*> controllers;
};

