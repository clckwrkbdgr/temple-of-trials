#pragma once
#include <string>

std::string now();
void log(const std::string & message);

struct Point {
	int x, y;
	Point(int _x, int _y);
	Point();
	bool operator==(const Point & other) const;
};

typedef char Sprite;
