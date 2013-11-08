#pragma once
#include <string>

std::string now();
void log(const std::string & message);

struct Point {
	int x, y;
	Point(int _x, int _y);
	Point();
	bool operator==(const Point & other) const;
	operator bool() const;
};
Point operator+(const Point & a, const Point & b);

typedef char Sprite;
