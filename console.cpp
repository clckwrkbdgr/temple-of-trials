#include "console.h"
#include "sprites.h"
#include "engine/game.h"
#include "engine/monsters.h"
#include "engine/format.h"
#include <ncurses.h>

std::map<int, Point> Console::directions;

enum {
	MAP_WIDTH = 60,
	MAP_HEIGHT = 1 + 23
};

Console::Console()
	: messages_seen(0)
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	start_color();
	
	init_sprites();

	if(directions.empty()) {
		directions['h'] = Point(-1,  0);
		directions['j'] = Point( 0, +1);
		directions['k'] = Point( 0, -1);
		directions['l'] = Point(+1,  0);
		directions['y'] = Point(-1, -1);
		directions['u'] = Point(+1, -1);
		directions['b'] = Point(-1, +1);
		directions['n'] = Point(+1, +1);
	}
}

Console::~Console()
{
	cbreak();
	echo();
	curs_set(1);
	endwin();
}

void Console::init_sprites()
{
	for(int fore = 0; fore < 8; ++fore) {
		if(fore == 0) {
			continue;
		}
		init_pair(fore, fore, 0);
	}
	sprites[Sprites::EMPTY]         = std::make_pair(' ', COLOR_PAIR(0));
	sprites[Sprites::FLOOR]         = std::make_pair('.', COLOR_PAIR(COLOR_YELLOW));
	sprites[Sprites::WALL]          = std::make_pair('#', COLOR_PAIR(COLOR_YELLOW));
	sprites[Sprites::TORCH]         = std::make_pair('&', COLOR_PAIR(COLOR_RED) | A_BOLD);
	sprites[Sprites::GOO]           = std::make_pair('~', COLOR_PAIR(COLOR_GREEN) | A_BOLD);
	sprites[Sprites::EXPLOSIVE]     = std::make_pair('*', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::MONEY]         = std::make_pair('$', COLOR_PAIR(COLOR_YELLOW));
	sprites[Sprites::SCORPION_TAIL] = std::make_pair('!', COLOR_PAIR(COLOR_RED));
	sprites[Sprites::SPEAR]         = std::make_pair('(', COLOR_PAIR(COLOR_BLUE) | A_BOLD);
	sprites[Sprites::JACKET]        = std::make_pair('[', COLOR_PAIR(COLOR_BLUE) | A_BOLD);
	sprites[Sprites::ANTIDOTE]      = std::make_pair('%', COLOR_PAIR(COLOR_MAGENTA));
	sprites[Sprites::APPLE]         = std::make_pair('%', COLOR_PAIR(COLOR_GREEN));
	sprites[Sprites::PLAYER]        = std::make_pair('@', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::ANT]           = std::make_pair('A', COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
	sprites[Sprites::SCORPION]      = std::make_pair('S', COLOR_PAIR(COLOR_RED) | A_BOLD);
	sprites[Sprites::DOOR_OPENED]   = std::make_pair('-', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::DOOR_CLOSED]   = std::make_pair('+', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::POT]           = std::make_pair('V', COLOR_PAIR(COLOR_YELLOW));
	sprites[Sprites::WELL]          = std::make_pair('{', COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
	sprites[Sprites::GATE]          = std::make_pair('<', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::STAIRS_UP]     = std::make_pair('<', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::STAIRS_DOWN]   = std::make_pair('>', COLOR_PAIR(COLOR_WHITE) | A_BOLD);
	sprites[Sprites::TRAP]          = std::make_pair('^', COLOR_PAIR(COLOR_YELLOW));
	sprites[Sprites::SHARPENED_POLE] = std::make_pair('(', COLOR_PAIR(COLOR_YELLOW) | A_BOLD);
	sprites[Sprites::KEY]           = std::make_pair('*', COLOR_PAIR(COLOR_WHITE));
	sprites[Sprites::FLASK]         = std::make_pair('}', COLOR_PAIR(COLOR_WHITE));
}

void Console::print_tile(int x, int y, int sprite)
{
	if(sprites.count(sprite) > 0) {
		mvaddch(y + 1, x, sprites[sprite].first | sprites[sprite].second);
	} else {
		log("Unknown sprite with code {0} at {1}", sprite, Point(x, y));
	}
}

void Console::print_fow(int x, int y, int sprite)
{
	if(sprites.count(sprite) > 0) {
		mvaddch(y + 1, x, sprites[sprite].first);
	} else {
		log("Unknown fow sprite with code {0} at {1}", sprite, Point(x, y));
	}
}

void Console::print_message(const std::string & text)
{
	mvprintw(MAP_HEIGHT, 0, "%s", text.c_str());
}

void Console::print_stat(int row, const std::string & text)
{
	mvprintw(row, MAP_WIDTH, "%s", text.c_str());
}

void Console::clear()
{
	::erase();
}

int Console::get_control()
{
	return getch();
}

Console & Console::instance()
{
	static Console console;
	return console;
}

void Console::notification(const std::string & text)
{
	notification_text = text;
}

struct NCursesUpdate {
	NCursesUpdate() { erase(); }
	~NCursesUpdate() { refresh(); }
};

void Console::draw_game(const Game & game)
{
	NCursesUpdate upd;

	const Map & map = game.level.map;

	for(unsigned x = 0; x < map.width; ++x) {
		for(unsigned y = 0; y < map.height; ++y) {
			if(map.cell(x, y).visible) {
				print_tile(x, y, game.get_info(Point(x, y)).compiled().sprite);
			} else if(map.cell(x, y).seen_sprite) {
				print_fow(x, y, map.cell(x, y).seen_sprite);
			}
		}
	}

	if(game.messages.messages.size() > messages_seen) {
		int width, height;
		getmaxyx(stdscr, height, width);
		(void)width;
		int message_pan = height - MAP_HEIGHT;
		if(message_pan <= 0) {
			messages_seen = game.messages.messages.size();
		} else {
			int messages_left = game.messages.messages.size() - messages_seen;
			int message_count = std::min(messages_left, message_pan);
			for(int i = 0; i < message_count; ++i) {
				const std::string & message = game.messages.messages[messages_seen + i];
				if(message_count < messages_left && i == message_count - 1) {
					mvprintw(MAP_HEIGHT + i, 0, "%s", (message + " (...)").c_str());
				} else {
					mvprintw(MAP_HEIGHT + i, 0, "%s", message.c_str());
				}
			}
			messages_seen += message_count;
		}
	}

	mvprintw(0, 0, "%s", notification_text.c_str());
	notification_text.clear();

	const Monster & player = game.get_player();
	if(!player.valid()) {
		return;
	}
	int row = 0;
	print_stat(row++, format("Level: {0}", game.current_level));
	print_stat(row++, format("Turns: {0}", game.turns));
	print_stat(row++, format("HP   : {0}/{1}", player.hp, player.type->max_hp));
	print_stat(row++, format("Items: {0}", player.inventory.size()));
	print_stat(row++, format("Wield: {0}", player.inventory.wielded_item().valid() ? player.inventory.wielded_item().type->name : "none"));
	print_stat(row++, format("Wear : {0}", player.inventory.worn_item().valid() ? player.inventory.worn_item().type->name : "none"));
	print_stat(row++, format("Dmg  : {0}", player.damage()));
	row++;
	if(player.poisoning > 0) {
		print_stat(row++, "Poisoned");
	}
}

Point Console::target_mode(Game & game, const Point & start)
{
	Point target = start;
	int ch = 0;
	curs_set(1);
	while(ch != 'x' && ch != 27 && ch != '.') {
		if(game.level.map.valid(target)) {
			if(game.level.map.cell(target).visible) {
				notification(format("You see {0}.", game.get_info(target).compiled().name));
			} else if(game.level.map.cell(target).seen_sprite) {
				notification(format("You recall {0}.", game.get_info(target).compiled().name));
			} else {
				notification("You cannot see there.");
			}
		}
		draw_game(game);
		if(game.level.map.valid(target)) {
			int ch = mvinch(target.y + 1, target.x);
			mvaddch(target.y + 1, target.x, ch ^ A_BLINK);
			move(target.y + 1, target.x);
		}
		ch = get_control();
		if(ch == 27) {
			nodelay(stdscr, TRUE);
			ch = getch();
			nodelay(stdscr, FALSE);
			if(ch == ERR || ch == 27) {
				ch = 27;
				break;
			}
		}

		if(directions.count(ch) != 0) {
			Point new_target = target + directions[ch];
			if(game.level.map.valid(new_target)) {
				target = new_target;
			}
		}
	}
	curs_set(0);
	if(ch == '.') {
		if(game.level.map.cell(target).seen_sprite == 0) {
			notification("You don't know how to get there.");
		} else {
			return target;
		}
	}
	return Point();
}

int Console::draw_and_get_control(Game & game)
{
	int ch = see_messages(game);
	return ch;
}

Point Console::draw_and_get_direction(Game & game)
{
	int ch = see_messages(game);
	while(directions.count(ch) == 0) {
		notification("This is not a direction.");
		ch = see_messages(game);
	}
	return directions[ch];
}

int Console::see_messages(Game & game)
{
	draw_game(game);
	bool ask_control = game.state != Game::SUSPENDED;
	int ch = (!ask_control && game.messages.messages.size() == messages_seen) ? 0 : get_control();
	while(game.messages.messages.size() > messages_seen) {
		if(ch == ' ') {
			draw_game(game);
		}
		ch = get_control();
	}
	return ch;
}

void Console::draw_inventory(const Game & game, const Monster & monster)
{
	clear();
	int width, height;
	getmaxyx(stdscr, height, width);
	(void)height;
	int pos = 0, index = 0;
	foreach(const Item & item, monster.inventory.items) {
		if(item.valid()) {
			int x = (pos < 13) ? 0 : width / 2;
			int y = 1 + ((pos < 13) ? pos : pos - 13);
			std::string text = format("{0} - {1}", char(index + 'a'), item.type->name);
			if(monster.inventory.wields(index)) {
				text += " (wielded)";
			}
			if(monster.inventory.wears(index)) {
				text += " (worn)";
			}
			mvprintw(y, x, text.c_str());
			++pos;
		}
		++index;
		if(index > Inventory::SLOT_COUNT) {
			break;
		}
	}
}

int Console::get_inventory_slot(const Game & game, const Monster & monster)
{
	draw_inventory(game, monster);

	int width, height;
	getmaxyx(stdscr, height, width);
	(void)height;
	int slot = -1;
	std::string error;
	while(true) {
		mvprintw(0, 0, "%s", std::string(width, ' ').c_str());
		mvprintw(0, 0, "%s", error.c_str());

		int ch = getch();
		if(ch == 27) {
			nodelay(stdscr, TRUE);
			ch = getch();
			nodelay(stdscr, FALSE);
			if(ch == ERR || ch == 27) {
				slot = -1;
				break;
			}
			error = "This is not a slot";
			continue;
		}
		if(ch < 'a' || 'z' < ch) {
			error = "This is not a slot";
			continue;
		}
		slot = ch - 'a';
		if(!monster.inventory.get_item(slot).valid()) {
			error = "Slot is empty; nothing is here.";
			continue;
		}
		break;
	}
	return slot;
}
