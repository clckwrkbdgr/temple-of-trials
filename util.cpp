#include "util.h"
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <unistd.h>

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

Point & Point::operator+=(const Point & other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Point::operator bool() const
{
	return x != 0 || y != 0;
}

Point operator+(const Point & a, const Point & b)
{
	return Point(a.x + b.x, a.y + b.y);
}

Point operator-(const Point & a, const Point & b)
{
	return Point(a.x - b.x, a.y - b.y);
}

Point operator/(const Point & a, int factor)
{
	return Point(a.x / factor, a.y / factor);
}

int distance(const Point & a, const Point & b)
{
	int x = a.x - b.x;
	int y = a.y - b.y;
	return int(std::sqrt(x * x + y * y));
}

std::string to_string(int value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(char value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(unsigned value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(long unsigned value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(const std::string & value)
{
    return value;
}

std::string to_string(const Point & value)
{
    return format("({0}, {1})", value.x, value.y);
}

void subs_arg_str(std::string & result, int index, const std::string & value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		result.replace(pos, placeholder.size(), value);
		pos = result.find(placeholder, pos + 1);
	}
}

void delay(int msec)
{
	usleep(msec * 1000);
}

std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t)
{
	out.push_back(t);
	return out;
}
