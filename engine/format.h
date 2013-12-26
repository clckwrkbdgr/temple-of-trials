#pragma once
#include <string>
#include <map>
#include <vector>
#include <list>

std::string to_string(int value);
std::string to_string(unsigned value);
std::string to_string(long unsigned value);
std::string to_string(char value);
std::string to_string(const std::string & value);
std::string to_string(const char * value);
template<class T>
std::string to_string(const T * value)
{
	return to_string((unsigned)value);
}
template<class K, class V>
std::string to_string(const std::pair<K, V> & value)
{
	return "(" + to_string(value.first) + ", " + to_string(value.second) + ")";
}

template<class Iterator>
std::string to_string(Iterator begin, Iterator current, Iterator end)
{
	Iterator next = current;
	++next;
	if(begin == current) {
		return (current != end) ? to_string(*current) + to_string(begin, next, end) : "";
	} else {
		return (current != end) ? "|" + to_string(*current) + to_string(begin, next, end) : "";
	}
}
template<class K, class V>
std::string to_string(const std::map<K, V> & m)
{
	return to_string(m.begin(), m.begin(), m.end());
}
template<class T>
std::string to_string(const std::vector<T> & v)
{
	return to_string(v.begin(), v.begin(), v.end());
}
template<class T>
std::string to_string(const std::list<T> & v)
{
	return to_string(v.begin(), v.begin(), v.end());
}

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

void direct_log(std::ostream * out_stream);
void start_log(const std::string & filename);
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

#define TRACE(x) do { log(format(#x "={0}", (x))); } while(0);
