#pragma once
#include <string>
#include <vector>
#include <map>
class Game;
class Monster;
class Point;
class Level;

struct Console {
	struct Window {
		unsigned x, y, width, height;
		Window(unsigned window_x, unsigned window_y, unsigned window_width, unsigned window_height)
			: x(window_x), y(window_y), width(window_width), height(window_height) {}
	};

	static std::map<int, Point> directions;

	Console();
	~Console();

	void draw_game(const Game & game);
	int draw_and_get_control(Game & game);
	Point draw_and_get_direction(Game & game);
	int draw_target_mode(Game & game, const Point & target);
	Point target_mode(Game & game, const Point & start);
	int see_messages(Game & game);
	void draw_inventory(const Game & game, const Monster & monster);
	int get_inventory_slot(const Game & game, const Monster & monster);
	void set_notification(const std::string & text);

	void print_messages(const Window & window, const std::vector<std::string> & messages);
	void print_map(const Window & window, const Level & level);
	void print_notification();
	void print_tile(int x, int y, int sprite, bool with_color);
	void print_text(int x, int y, const std::string & text);
	void print_stat(int row, const std::string & text);
	void clear();
	int get_control();
protected:
	unsigned messages_seen;
	std::string notification;
	std::map<int, std::pair<char, int> > sprites;

	void init_sprites();
};

class TempleUI : public Console {
public:
	TempleUI();
	~TempleUI();
};
