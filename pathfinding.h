#pragma once
#include <list>
class Point;
class Control;

class PassabilityDetector {
public:
	virtual ~PassabilityDetector() {}
	virtual bool is_passable(int x, int y) const = 0;
};

std::list<Control> find_path(const Point & player_pos, const Point & target, PassabilityDetector * detector);

