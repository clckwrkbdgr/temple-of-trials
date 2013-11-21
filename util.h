#pragma once
#include <string>
#include <vector>

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


std::string now();

void log(const std::string & message);
template<class T0>
	void log(const std::string & message, const T0 & t0)
	{ log(format(message, t0)); }
template<class T0, class T1>
	void log(const std::string & message, const T0 & t0, const T1 & t1)
	{ log(format(message, t0, t1)); }
template<class T0, class T1, class T2>
	void log(const std::string & message, const T0 & t0, const T1 & t1, const T2 & t2)
	{ log(format(message, t0, t1, t2)); }


template<typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

struct Point {
	int x, y;
	Point(int _x, int _y);
	Point();
	bool operator==(const Point & other) const;
	operator bool() const;
};
Point operator+(const Point & a, const Point & b);

template<class T>
const T & find_at(const std::vector<T> & container, const Point & pos, typename std::vector<T>::const_iterator * index = 0)
{
	typename std::vector<T>::const_iterator i;
    for(i = container.begin(); i != container.end(); ++i) {
        if(i->pos == pos) {
			if(index) {
				*index = i;
			}
            return *i;
        }
    }
    static T empty;
    return empty;
}

template<class T>
T & find_at(std::vector<T> & container, const Point & pos, typename std::vector<T>::iterator * index = 0)
{
	typename std::vector<T>::iterator i;
    for(i = container.begin(); i != container.end(); ++i) {
        if(i->pos == pos) {
			if(index) {
				*index = i;
			}
            return *i;
        }
    }
    static T empty;
	empty = T();
    return empty;
}

int distance(const Point & a, const Point & b);


typedef char Sprite;
