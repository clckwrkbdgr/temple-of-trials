#pragma once
#include "monsters.h"
#include "cell.h"
#include "objects.h"
#include "util.h"
#include <fstream>
#include <vector>
#include <map>

bool file_exists(const std::string & filename);
std::string read_string(std::istream & in, char quote = '"');
std::string escaped(const std::string & s, char quote = '"');

#define SAVEFILE_STORE_EXT(Type, variable) \
	template<class Savefile> void store_ext(Savefile & savefile, Type & variable) { store_##variable(savefile, variable); } \
	template<class Savefile> void store_ext(Savefile & savefile, const Type & variable) { store_##variable(savefile, variable); } \
	template<class Savefile, class Type> \
	void store_##variable(Savefile & savefile, Type & variable)

class Reader {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Reader(std::istream & in_stream);

	Reader & newline();
	Reader & version(int major_version, int minor_version);
	int version() const { return actual_minor_version; }
	Reader & check(const std::string & section);
	
	template<class T>
	Reader & store(T & value)
	{
		store_ext(*this, value);
		return *this;
	}

	Reader & add_type_registry(const TypeRegistry<Cell> & type_registry);
	Reader & add_type_registry(const TypeRegistry<Monster> & type_registry);
	Reader & add_type_registry(const TypeRegistry<Object> & type_registry);
	Reader & add_type_registry(const TypeRegistry<Item> & type_registry);
	Reader & store(int & value);
	Reader & store(unsigned int & value);
	Reader & store(char & value);
	Reader & store(bool & value);
	Reader & store(std::string & value);
	Reader & store(Point & value);
	template<class T>
	Reader & store_type(TypePtr<T> & type)
	{
		std::string type_id;
		store(type_id);
		type = TypePtr<T>(get_registry(T())->get(type_id));
		check(type_id + " type");
		return *this;
	}

	template<class T>
	Reader & size_of(Map<T> & map)
	{
		unsigned width = 0, height = 0;
		store(width).store(height);
		map = Map<T>(width, height);
		return *this;
	}
	template<class T>
	Reader & size_of(std::vector<T> & v)
	{
		unsigned size = 0;
		store(size);
		v.resize(size);
		return *this;
	}
	template<class T>
	void store(std::vector<T> & v, const std::string & name)
	{
		size_of(v).newline().check(name + " count");
		for(decltype(v.begin()) item = v.begin(); item != v.end(); ++item) {
			store(*item).newline().check(name);
		}
	}
	template<class K, class V>
	void store(std::map<K, V> & map, const std::string & name)
	{
		int count;
		store(count).newline().check(name + " count");
		while(count --> 0) {
			K key;
			store(key);
			store(map[key]).newline().check(name);
		}
	}
private:
	int actual_minor_version;
	std::istream & in;

	const TypeRegistry<Cell> * cell_types;
	const TypeRegistry<Monster> * monster_types;
	const TypeRegistry<Object> * object_types;
	const TypeRegistry<Item> * item_types;
	const TypeRegistry<Cell> * get_registry(const Cell::Type &) { return cell_types; }
	const TypeRegistry<Monster> * get_registry(const Monster::Type &) { return monster_types; }
	const TypeRegistry<Object> * get_registry(const Object::Type &) { return object_types; }
	const TypeRegistry<Item> * get_registry(const Item::Type &) { return item_types; }
};

class Writer {
public:
	struct Exception {
		std::string message;
		Exception(const std::string text) : message(text) {}
	};
	Writer(std::ostream & out_stream);

	Writer & newline();
	Writer & version(int major_version, int minor_version);
	int version() const { return actual_minor_version; }
	Writer & check(const std::string & section);
	
	template<class T>
	Writer & store(const T & value)
	{
		store_ext(*this, value);
		return *this;
	}

	Writer & store(int value);
	Writer & store(unsigned int value);
	Writer & store(char value);
	Writer & store(bool value);
	Writer & store(const std::string & value);
	Writer & store(const Point & value);
	Writer & add_type_registry(const TypeRegistry<Cell> &) { return * this; }
	Writer & add_type_registry(const TypeRegistry<Monster> &) { return * this; }
	Writer & add_type_registry(const TypeRegistry<Object> &) { return * this; }
	Writer & add_type_registry(const TypeRegistry<Item> &) { return * this; }
	template<class T>
	Writer & store_type(const TypePtr<T> & type)
	{
		store(type->id);
		check(type->id + " type");
		return *this;
	}

	template<class T>
	Writer & size_of(const Map<T> & map)
	{
		store(map.width).store(map.height);
		return *this;
	}
	template<class T>
	Writer & size_of(const std::vector<T> & v)
	{
		store(unsigned(v.size()));
		return *this;
	}
	template<class T>
	void store(const std::vector<T> & v, const std::string & name)
	{
		size_of(v).newline().check(name + " count");
		for(decltype(v.begin()) item = v.begin(); item != v.end(); ++item) {
			store(*item).newline().check(name);
		}
	}
	template<class K, class V>
	void store(const std::map<K, V> & map, const std::string & name)
	{
		store(unsigned(map.size())).newline().check(name + " count");
		typename std::map<K, V>::const_iterator i;
		for(i = map.begin(); i != map.end(); ++i) {
			store(i->first).store(i->second).newline().check(name);
		}
	}
private:
	int actual_minor_version;
	std::ostream & out;
};

