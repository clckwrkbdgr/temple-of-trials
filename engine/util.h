#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include <cstdarg>
class Point;

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION < 403000
// Ugly hack for decltype presence.
#define decltype __typeof__
#endif

template<class T>
struct ForeachIterator {
	T iterator;
	bool done;
	bool brk;
	ForeachIterator(T t): iterator(t), done(false), brk(false)  { }
	operator bool() const { return true; }
	bool is_done() { if(!done) { brk = true; };  return done; }
	void mark_done() { brk = false; done = true;  }
	void reset_done() { done = false;  }
};
#define foreach(expression, container) \
	if(ForeachIterator<decltype(container.begin())> it = container.begin()) \
        for(; it.iterator != container.end() && !it.brk; ++it.iterator, it.reset_done()) \
            for(expression = *it.iterator; !it.is_done(); it.mark_done())


std::string to_string(int value);
std::string to_string(unsigned value);
std::string to_string(long unsigned value);
std::string to_string(char value);
std::string to_string(const std::string & value);
std::string to_string(const Point & value);
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


void delay(int msec);

template<typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

template<class T>
std::vector<T> & operator<<(std::vector<T> & out, const T & t)
{
	out.push_back(t);
	return out;
}
std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t);

template<class T, size_t N>
size_t size_of_array(T (&)[N]) { return N; }
template<class T, size_t N>
std::vector<T> make_vector(T (&a)[N]) { return std::vector<T>(a, a + N); }
template<class T>
std::vector<T> make_vector(const std::list<T> & a) { return std::vector<T>(a.begin(), a.end()); }

template<class T>
class MakeVector {
public:
	std::vector<T> result;
	MakeVector(const T & value) : result(1, value) {}
	MakeVector & operator()(const T & value) { result.push_back(value); return *this; }
	operator std::vector<T> & () { return result; }
	operator const std::vector<T> & () const { return result; }
};

template<class IteratorA, class IteratorB>
bool equal_containers(IteratorA a_begin, IteratorA a_end, IteratorB b_begin, IteratorB b_end)
{
	for(; a_begin != a_end && b_begin != b_end; ++a_begin, ++b_begin) {
		if(*a_begin != *b_begin) {
			return false;
		}
	}
	return a_begin == a_end && b_begin == b_end;
}

