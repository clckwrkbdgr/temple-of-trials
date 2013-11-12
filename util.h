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

std::string to_string(int value);
std::string to_string(unsigned value);
std::string to_string(long unsigned value);
std::string to_string(char value);
std::string to_string(const std::string & value);

void subs_arg_str(std::string & result, int index, const std::string & value);
template<class T>
void subs_arg(std::string & result, int index, const T & value) { subs_arg_str(result, index, to_string(value)); }

template<class T0>
	std::string format(std::string s, const T0 & t0)
	{ subs_arg(s, 0, t0); return s; }
template<class T0, class T1>
	std::string format(std::string s, const T0 & t0, const T1 & t1)
	{ subs_arg(s, 0, t0); subs_arg(s, 1, t1); return s; }
template<class T0, class T1, class T2>
	std::string format(std::string s, const T0 & t0, const T1 & t1, const T2 & t2)
	{ subs_arg(s, 0, t0); subs_arg(s, 1, t1); subs_arg(s, 2, t2); return s; }

