#pragma once
#include <chthon/format.h>
#include <string>
#include <vector>
#include <map>
namespace Chthon {
	class Game;
	class Monster;
	class Point;
	class Level;
	class GameEvent;
}

struct Console {
	struct Window {
		int x, y;
		unsigned width, height;
		Window(int window_x, int window_y, unsigned window_width, unsigned window_height)
			: x(window_x), y(window_y), width(window_width), height(window_height) {}
	};

	static std::map<int, Chthon::Point> directions;

	unsigned messages_seen;
	bool log_messages;
	std::string notification;
	std::vector<std::string> messages;
	std::map<int, std::pair<unsigned char, unsigned> > sprites;

	void init_sprites();

	Console();
	~Console();

	void draw_game(const Chthon::Game & game);
	int draw_and_get_control(Chthon::Game & game);
	Chthon::Point draw_and_get_direction(Chthon::Game & game);
	int draw_target_mode(Chthon::Game & game, const Chthon::Point & target);
	Chthon::Point target_mode(Chthon::Game & game, const Chthon::Point & start);
	int see_messages(Chthon::Game & game);
	void draw_inventory(const Chthon::Game & game, const Chthon::Monster & monster);
	unsigned get_inventory_slot(const Chthon::Game & game, const Chthon::Monster & monster);
	void set_notification(const std::string & text);

	void print_messages(const Window & window);
	void print_map(const Window & window, const Chthon::Level & level);
	void print_notification();
	void print_tile(int x, int y, int sprite, bool with_color);
	void print_text(int x, int y, const std::string & text);
	void print_stat(int row, const std::string & text);
	void clear();
	int get_control();

	void message(const Chthon::GameEvent & event);
	void message(const std::string & text);
	template<class T>
		void message(const std::string & text, const T & t)
		{ message(Chthon::format(text, t)); }
	template<class T1, class T2>
		void message(const std::string & text, const T1 & t1, const T2 & t2)
		{ message(Chthon::format(text, t1, t2)); }
	template<class T1, class T2, class T3>
		void message(const std::string & text, const T1 & t1, const T2 & t2, const T3 & t3)
		{ message(Chthon::format(text, t1, t2, t3)); }
};

class TempleUI : public Console {
public:
	TempleUI();
	~TempleUI();
};
