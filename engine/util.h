#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if GCC_VERSION < 403000
// Ugly hack for decltype presence.
#define decltype __typeof__
#endif

void delay(int msec);

template<typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

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

template<class T>
struct TypePtr {
	explicit TypePtr(const T * type_pointer = 0)
		: pointer(type_pointer) {}
	bool valid() const
	{
		return pointer;
	}
	bool equal(const TypePtr<T> & other) const
	{
		return other == pointer;
	}
	bool equal(const T * other) const
	{
		return other == pointer;
	}
	const T & operator*() const
	{
		if(pointer) {
			return *pointer;
		}
		static T empty;
		return empty;
	}
	const T * operator->() const
	{
		return &(operator*());
	}
private:
	const T * pointer;
};
template<class T>
std::string to_string(const TypePtr<T> & ptr)
{
	return ptr->id;
}
template<class T>
bool operator==(const TypePtr<T> & typeptr, const T * ptr)
{
	return typeptr.equal(ptr);
}
template<class T>
bool operator==(const TypePtr<T> & typeptr, const TypePtr<T> & other)
{
	return typeptr.equal(other);
}
template<class T> bool operator==(const T * ptr, const TypePtr<T> & typeptr) { return operator==(typeptr, ptr); }
template<class T> bool operator!=(const TypePtr<T> & typeptr, const TypePtr<T> & other) { return !operator==(typeptr, other); }
template<class T> bool operator!=(const TypePtr<T> & typeptr, const T * ptr) { return !operator==(typeptr, ptr); }
template<class T> bool operator!=(const T * ptr, const TypePtr<T> & typeptr) { return !operator==(typeptr, ptr); }

template<class Value>
struct TypeRegistry {
	typedef typename Value::Type ValueType;
	std::map<std::string, ValueType> types;

	bool has(const std::string & id) const
	{
		return (types.count(id) > 0);
	}
	const ValueType * get(const std::string & id) const
	{
		if(has(id)) {
			return &types.find(id)->second;
		}
		return 0;
	}
	const ValueType * insert(const ValueType & type)
	{
		return &(types[type.id] = type);
	}
	typename ValueType::Builder insert(const std::string & id)
	{
		types[id].id = id;
		return typename ValueType::Builder(types[id]);
	}
};
