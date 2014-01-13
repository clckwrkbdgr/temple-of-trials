#include "console.h"
#include "sprites.h"
#include "engine/game.h"
#include "engine/actions.h"
#include "engine/monsters.h"
#include "engine/log.h"
#include "engine/format.h"
#include <ncurses.h>

std::map<int, Point> Console::directions;

enum {
	MAP_WIDTH = 60,
	MAP_HEIGHT = 1 + 23
};

Console::Console()
	: messages_seen(0), log_messages(false)
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	start_color();
	
	for(int fore = 0; fore < 8; ++fore) {
		if(fore == 0) {
			continue;
		}
		init_pair(fore, fore, 0);
	}

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

void Console::print_tile(int x, int y, int sprite, bool with_color)
{
	if(sprites.count(sprite) > 0) {
		int value = sprites[sprite].first;
		int color = with_color ? sprites[sprite].second : 0;
		mvaddch(y, x,  value | color);
	} else {
		log("Unknown sprite with code {0} at ({1}, {2})", sprite, x, y);
	}
}

void Console::print_text(int x, int y, const std::string & text)
{
	mvprintw(y, x, "%s", text.c_str());
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

void Console::set_notification(const std::string & text)
{
	notification = text;
}

struct NCursesUpdate {
	NCursesUpdate() { erase(); }
	~NCursesUpdate() { refresh(); }
};

void Console::print_notification()
{
	mvprintw(0, 0, "%s", notification.c_str());
	notification.clear();
}

void Console::print_map(const Window & window, const Level & level)
{
	for(unsigned x = 0; x < level.map.width && (x < window.width - window.x); ++x) {
		for(unsigned y = 0; y < level.map.height && (y < window.height - window.y); ++y) {
			if(level.map.cell(x, y).visible) {
				print_tile(window.x + x, window.y + y, level.get_info(Point(x, y)).compiled().sprite, true);
			} else if(level.map.cell(x, y).seen_sprite) {
				print_tile(window.x + x, window.y + y, level.map.cell(x, y).seen_sprite, false);
			}
		}
	}
}

void Console::print_messages(const Window & window, const std::vector<std::string> & messages)
{
	if(messages.size() > messages_seen) {
		if(window.height == 0) {
			messages_seen = messages.size();
		} else {
			unsigned messages_left = messages.size() - messages_seen;
			unsigned messages_to_draw = std::min(messages_left, window.height);
			for(unsigned i = 0; i < messages_to_draw; ++i) {
				const std::string & message = messages[messages_seen + i];
				if(messages_to_draw < messages_left && i == messages_to_draw - 1) {
					print_text(0, window.y + i, message + " (...)");
				} else {
					print_text(0, window.y + i, message);
				}
			}
			messages_seen += messages_to_draw;
		}
	}
}

void Console::draw_game(const Game & game)
{
	NCursesUpdate upd;

	Window map_window(0, 1, 60, 23);
	print_map(map_window, game.level());

	unsigned width, height;
	getmaxyx(stdscr, height, width);
	Window message_window(0, map_window.y + map_window.height, width, height - (map_window.y + map_window.height));
	print_messages(message_window, messages);

	print_notification();

	const Monster & player = game.level().get_player();
	if(!player.valid()) {
		return;
	}
	int row = 0;
	print_stat(row++, format("Level: {0}", game.current_level_index));
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
		if(game.level().map.valid(target)) {
			if(game.level().map.cell(target).visible) {
				set_notification(format("You see {0}.", game.level().get_info(target).compiled().name));
			} else if(game.level().map.cell(target).seen_sprite) {
				set_notification(format("You recall {0}.", game.level().get_info(target).compiled().name));
			} else {
				set_notification("You cannot see there.");
			}
		}
		draw_game(game);
		if(game.level().map.valid(target)) {
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
			if(game.level().map.valid(new_target)) {
				target = new_target;
			}
		}
	}
	curs_set(0);
	if(ch == '.') {
		if(game.level().map.cell(target).seen_sprite == 0) {
			set_notification("You don't know how to get there.");
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
		set_notification("This is not a direction.");
		ch = see_messages(game);
	}
	return directions[ch];
}

int Console::see_messages(Game & game)
{
	foreach(const GameEvent & e, game.events) {
		message(e);
	}
	game.events.clear();

	draw_game(game);
	bool ask_control = game.state != Game::SUSPENDED;
	int ch = (!ask_control && messages.size() == messages_seen) ? 0 : get_control();
	while(messages.size() > messages_seen) {
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
	while(true) {
		mvprintw(0, 0, "%s", std::string(width, ' ').c_str());
		print_notification();

		int ch = getch();
		if(ch == 27) {
			nodelay(stdscr, TRUE);
			ch = getch();
			nodelay(stdscr, FALSE);
			if(ch == ERR || ch == 27) {
				slot = -1;
				break;
			}
			set_notification("This is not a slot");
			continue;
		}
		if(ch < 'a' || 'z' < ch) {
			set_notification("This is not a slot");
			continue;
		}
		slot = ch - 'a';
		if(!monster.inventory.get_item(slot).valid()) {
			set_notification("Slot is empty; nothing is here.");
			continue;
		}
		break;
	}
	return slot;
}

void Console::message(const std::string & text)
{
	if(text.empty()) {
		return;
	}
	std::string titled_text = text;
	titled_text[0] = toupper(titled_text[0]);
	messages.push_back(titled_text);
	if(log_messages) {
		log("Message: " + titled_text);
	}
}

void Console::message(const GameEvent & e)
{
	switch(e.type) {
		case GameEvent::CURES_POISONING: message("{0} cures {1}.", e.actor.name, e.target.name); break;
		case GameEvent::HEALS: message("{0} heals {1}.", e.actor.name, e.target.name); break;
		case GameEvent::HURTS: message("{0} hurts {1}!", e.actor.name, e.target.name); break;
		case GameEvent::IS_HURT_BY_POISONING: message("Poisoning hurts {0}!", e.actor.name); break;
		case GameEvent::LOSES_HEALTH: message("{0} loses {1} hp.", e.actor.name, e.amount); break;
		case GameEvent::DIED: message("{0} died.", e.actor.name); break;
		case GameEvent::HITS: message("{0} hits {1}.", e.actor.name, e.target.name); break;
		case GameEvent::HITS_FOR_HEALTH: message("{0} hits {1} for {2} hp.", e.actor.name, e.target.name, e.amount); break;
		case GameEvent::BUMPS_INTO: message("{0} bumps into {1}.", e.actor.name, e.target.name); break;
		case GameEvent::POISONS: message("{0} poisons {1}.", e.actor.name, e.target.name); break;
		case GameEvent::SWINGS_AT_NOTHING: message("{0} swing at nothing.", e.actor.name); break;
		case GameEvent::OPENS: message("{0} opens {1}.", e.actor.name, e.target.name); break;
		case GameEvent::CLOSES: message("{0} closes {1}.", e.actor.name, e.target.name); break;
		case GameEvent::DRINKS: message("{0} drinks from {1}.", e.actor.name, e.target.name); break;
		case GameEvent::GOES_DOWN: message("{0} goes down.", e.actor.name); break;
		case GameEvent::GOES_UP: message("{0} goes up.", e.actor.name); break;
		case GameEvent::UNLOCKS: message("{0} unloks {1}.", e.actor.name, e.target.name); break;
		case GameEvent::TRAP_IS_OUT_OF_ITEMS: message("{0} is out of bolts.", e.actor.name); break;
		case GameEvent::TRIGGERS: message("{0} triggers {1}.", e.actor.name, e.target.name); break;
		case GameEvent::EATS: message("{0} eats {1}.", e.actor.name, e.target.name); break;
		case GameEvent::EMPTIES: message("{0} is emptied.", e.target.name); break;
		case GameEvent::REFILLS: message("{0} is refilled.", e.target.name); break;
		case GameEvent::TAKES_OFF: message("{0} takes off {1}.", e.actor.name, e.target.name); break;
		case GameEvent::THROWS: message("{0} throws {1}.", e.actor.name, e.target.name); break;
		case GameEvent::UNWIELDS: message("{0} unwields {1}.", e.actor.name, e.target.name); break;
		case GameEvent::WEARS: message("{0} wears {1}.", e.actor.name, e.target.name); break;
		case GameEvent::WIELDS: message("{0} wields {1}.", e.actor.name, e.target.name); break;
		case GameEvent::DROPS_AT: message("{0} drops {1} at {2}.", e.actor.name, e.target.name, e.help.name); break;
		case GameEvent::FALLS_INTO: message("{0} falls into {1}.", e.actor.name, e.target.name); break;
		case GameEvent::PICKS_UP_FROM: message("{0} picks up {1} from {2}.", e.actor.name, e.target.name, e.help.name); break;
		case GameEvent::TAKES_FROM: message("{0} takes {1} from {2}.", e.actor.name, e.target.name, e.help.name); break;
		case GameEvent::PICKED_UP_A_QUEST_ITEM: message("Now get this {0} to the Temple Gate!", e.target.name); break;
		case GameEvent::SHOULD_GET_QUEST_ITEM: message("{0} should find explosives first!", e.actor.name); break;
		case GameEvent::WINS_GAME_WITH: message("{0} successfully brought {1} to the Temple Gate!", e.actor.name, e.target.name); break;

		case GameEvent::ALREADY_CLOSED: message(format("{0} is already closed.", e.actor.name)); break;
		case GameEvent::ALREADY_FULL: message(format("{0} is already full.", e.actor.name)); break;
		case GameEvent::ALREADY_OPENED: message(format("{0} is already opened.", e.actor.name)); break;
		case GameEvent::CANNOT_DRINK: message(format("{0} cannot drink {1}", e.actor.name, e.target.name)); break;
		case GameEvent::CANNOT_EAT: message(format("{0} is not edible.", e.target.name)); break;
		case GameEvent::CANNOT_GO_DOWN: message(format("{0} cannot go down there.", e.actor.name)); break;
		case GameEvent::CANNOT_GO_UP: message(format("{0} cannot go up there.", e.actor.name)); break;
		case GameEvent::CANNOT_WEAR: message(format("{0} cannot wear {1}.", e.actor.name, e.target.name)); break;
		case GameEvent::LOCKED: message(format("{0} is locked.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_CLOSE: message("There is nothing to close there."); break;
		case GameEvent::NOTHING_TO_DRINK: message("There is nothing to drink there."); break;
		case GameEvent::NOTHING_TO_DROP: message(format("{0} has nothing to drop.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_EAT: message(format("{0} has nothing to eat.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_GRAB: message("There is nothing to grab there."); break;
		case GameEvent::NOTHING_TO_OPEN: message("There is nothing to open there."); break;
		case GameEvent::NOTHING_TO_TAKE_OFF: message(format("{0} have nothing to take off.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_UNWIELD: message(format("{0} have nothing to unwield.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_WEAR: message(format("{0} has nothing to drop.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_WIELD: message(format("{0} has nothing to drop.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_PUT: message(format("{0} have nothing to put down.", e.actor.name)); break;
		case GameEvent::NOTHING_TO_THROW: message(format("{0} have nothing to throw.", e.actor.name)); break;
		case GameEvent::NO_SPACE_LEFT: message(format("{0} carries too much items.", e.actor.name)); break;
		case GameEvent::NO_SUCH_ITEM: message("No such item."); break;
		case GameEvent::HAS_NO_ITEMS: message(format("{0} is totally empty.", e.actor.name)); break;
		default: log("Unknown event type #{0} with actor <{1}> and target <{2}>", e.type, e.actor.id, e.target.id);

	}
}


TempleUI::TempleUI()
	: Console()
{
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

TempleUI::~TempleUI()
{
}
