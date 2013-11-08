#include "util.h"
#include <ctime>
#include <fstream>

std::string now()
{
	time_t t = time(0);
	std::string s = ctime(&t);
	return s.replace(s.find('\n'), 1, " ");
}

void log(const std::string & message)
{
	static std::ofstream lout("temple.log", std::ios::app);
	lout << message << std::endl;
}


Point::Point(int _x, int _y)
	: x(_x), y(_y)
{
}

Point::Point()
	: x(0), y(0)
{
}

bool Point::operator==(const Point & other) const
{
	return x == other.x && y == other.y;
}
