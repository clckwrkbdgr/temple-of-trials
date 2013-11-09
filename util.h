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

class Format {
public:
	Format(const std::string & format_string);
	const std::string & str() const { return result; }
	Format & arg(int index, const std::string & value);
	Format & arg(int index, int value);
	Format & arg(int index, unsigned value);
	Format & arg(int index, char value);
private:
	std::string result;
};

template<class T0>
	std::string format(const std::string & s, const T0 & t0)
	{ Format format(s); return format.arg(0, t0).str(); }
template<class T0, class T1>
	std::string format(const std::string & s, const T0 & t0, const T1 & t1)
	{ Format format(s); return format.arg(0, t0).arg(1, t1).str(); }
template<class T0, class T1, class T2>
	std::string format(const std::string & s, const T0 & t0, const T1 & t1, const T2 & t2)
	{ Format format(s); return format.arg(0, t0).arg(1, t1).arg(2, t2).str(); }

