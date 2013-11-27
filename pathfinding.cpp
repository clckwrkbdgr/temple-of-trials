#include "pathfinding.h"
#include "game.h"
#include "util.h"
#include <vector>

struct PathStep {
	int x, y, counter;
	bool fixed;
	PathStep(int _x, int _y, int _counter) : x(_x), y(_y), counter(_counter), fixed(false) {}
};

std::list<Control> find_path(const Point & player_pos, const Point & target, PassabilityDetector * detector)
{
	std::list<Control> best_path;
	if(!detector || !detector->is_passable(target.x, target.y)) {
		return best_path;
	}

	std::vector<Point> shifts;
	for(Point shift(-1, 0); shift.x <= 1; ++shift.x) {
		for(shift.y = -1; shift.y <= 1; ++shift.y) {
			if(shift) {
				shifts.push_back(shift);
			}
		}
	}

	std::list<std::vector<Point> > waves;
	waves.push_front(std::vector<Point>(1, target));

	for(int i = 0; i < 2000; ++i) {
		bool found = false;
		std::vector<Point> neighs;
		foreach(const Point & point, waves.front()) {
			foreach(const Point & shift, shifts) {
				Point n = point + shift;
				if(n == player_pos) {
					found = true;
					break;
				}
				if(!detector->is_passable(n.x, n.y)) {
					continue;
				}
				bool already_present = false;
				foreach(const std::vector<Point> & wave, waves) {
					if(std::find(wave.begin(), wave.end(), n) != wave.end()) {
						already_present = true;
						break;
					}
				}
				if(!already_present) {
					if(std::find(neighs.begin(), neighs.end(), n) == neighs.end()) {
						neighs.push_back(n);
					}
				}
			}
			if(found) {
				break;
			}
		}
		if(found) {
			break;
		}
		waves.push_front(neighs);
	}

	Point prev = player_pos;
	foreach(const std::vector<Point> wave, waves) {
		foreach(const Point & point, wave) {
			Point shift = Point(point.x - prev.x, point.y - prev.y);
			bool is_close = std::abs(shift.x) <= 1 && std::abs(shift.y) <= 1;
			if(shift && is_close) {
				prev = point;
				best_path.push_back(Control(Control::MOVE, shift));
				break;
			}
		}
	}
	return best_path;
}

