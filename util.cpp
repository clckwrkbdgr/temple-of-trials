#include "util.h"
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>

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

Point::operator bool() const
{
	return x != 0 || y != 0;
}

Point operator+(const Point & a, const Point & b)
{
	return Point(a.x + b.x, a.y + b.y);
}


Format::Format(const std::string & format_string)
	: result(format_string)
{
}

Format & Format::arg(int index, const std::string & value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		result.replace(pos, placeholder.size(), value);
		pos = result.find(placeholder, pos + 1);
	}
	return *this;
}

Format & Format::arg(int index, int value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		std::ostringstream value_out;
		value_out << value;
		result.replace(pos, placeholder.size(), value_out.str());
		pos = result.find(placeholder, pos + 1);
	}
	return *this;
}

Format & Format::arg(int index, unsigned value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		std::ostringstream value_out;
		value_out << value;
		result.replace(pos, placeholder.size(), value_out.str());
		pos = result.find(placeholder, pos + 1);
	}
	return *this;
}

Format & Format::arg(int index, char value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		result.replace(pos, placeholder.size(), std::string(1, value));
		pos = result.find(placeholder, pos + 1);
	}
	return *this;
}

