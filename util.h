#pragma once
#include <string>

std::string now();
void log(const std::string & message);

struct Point {
	int x, y;
	Point(int _x, int _y) : x(_x), y(_y) {}
	Point() : x(0), y(0) {}
};

