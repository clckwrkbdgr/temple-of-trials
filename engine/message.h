#pragma once
#include "format.h"
#include <vector>
#include <string>
class GameEvent;

struct Messages {
	bool log_messages;
	std::vector<std::string> messages;

	Messages();
	void message(const std::string & text);
	template<class T>
		void message(const std::string & text, const T & t)
		{ message(format(text, t)); }
	template<class T1, class T2>
		void message(const std::string & text, const T1 & t1, const T2 & t2)
		{ message(format(text, t1, t2)); }
	template<class T1, class T2, class T3>
		void message(const std::string & text, const T1 & t1, const T2 & t2, const T3 & t3)
		{ message(format(text, t1, t2, t3)); }

	void message(const GameEvent & event);
};
