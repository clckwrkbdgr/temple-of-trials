#include "game.h"
#include <map>
#include <cstdlib>

std::map<int, Point> Game::directions;

Game::Game()
	: map(1, 1, Cell::floor()), player(), mode(NORMAL_MODE), turns(0),
	turn_is_ended(false)
{
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

Point Game::find_random_free_cell() const
{
	int counter = map.get_width() * map.get_height();
	while(--counter > 0) {
		Point new_pos = Point(rand() % map.get_width(), rand() % map.get_height());
		if(!map.is_passable(new_pos)) {
			continue;
		}
		if(find_at(doors, new_pos)) {
			continue;
		}
		if(find_at(monsters, new_pos)) {
			continue;
		}
		if(player.pos == new_pos) {
			continue;
		}
		return new_pos;
	}
	return Point();
}

void Game::generate()
{
	log("Generating new game...");
	doors.clear();
	monsters.clear();
	map = Map(60, 23, Cell::floor());
	player = Monster::player(find_random_free_cell());
	for(int i = 0; i < 10; ++i) {
		Point point = find_random_free_cell();
		if(point) {
			map.cell(point.x, point.y) = Cell::wall();
		}
	}
	for(int i = 0; i < 5; ++i) {
		Point point = find_random_free_cell();
		if(point) {
			doors.push_back(Door(point));
		}
	}
	for(int i = 0; i < 5; ++i) {
		Point point = find_random_free_cell();
		if(point) {
			int ai = (rand() % 2) ? Monster::AI_STILL : Monster::AI_WANDER;
			monsters.push_back(Monster::ant(ai, point));
		}
	}
	log("Done.");
}

void Game::message(std::string text)
{
	if(text.empty()) {
		return;
	}
	text[0] = toupper(text[0]);
	messages.push_back(text);
	log("Message: " + text);
}

void Game::move(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
	Point new_pos = someone.pos + shift;
	if(!map.is_passable(new_pos)) {
		message(format("{0} bump into the wall.", someone.name));
		return;
	}
    Door & door = find_at(doors, new_pos);
	if(door && !door.opened) {
		message("Door is closed.");
		return;
	}
    Monster & monster = find_at(monsters, new_pos);
	if(monster) {
		message(format("{0} bump into {1}.", someone.name, monster.name));
		return;
	}
	if(player.pos == new_pos) {
		message(format("{0} bump into {1}.", someone.name, player.name));
		return;
	}
    someone.pos = new_pos;
}

void Game::open(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(!door) {
        message("There is nothing to open there.");
        return;
    }
    if(door.opened) {
        message("Door is already opened.");
        return;
    }
    door.opened = true;
    message(format("{0} opened the door.", someone.name));
}

void Game::close(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(!door) {
        message("There is nothing to close there.");
        return;
    }
    if(!door.opened) {
        message("Door is already closed.");
        return;
    }
    door.opened = false;
    message(format("{0} closed the door.", someone.name));
}

void Game::process_normal_mode(int ch)
{
	switch(ch) {
		case 'o': mode = Game::OPEN_MODE; return;
		case 'c': mode = Game::CLOSE_MODE; return;
		case 'q': mode = Game::EXIT_MODE; return;
		default: break;
	}
	if(directions.count(ch) == 0) {
        message(format("Unknown control '{0}'", char(ch)));
        return;
    }
	move(player, directions[ch]);
	turn_is_ended = true;
}

void Game::process_open_mode(int ch)
{
	mode = Game::NORMAL_MODE;
	if(directions.count(ch) == 0) {
		message("This is not a direction.");
        return;
    }
	open(player, directions[ch]);
	turn_is_ended = true;
}

void Game::process_close_mode(int ch)
{
	mode = Game::NORMAL_MODE;
	if(directions.count(ch) == 0) {
		message("This is not a direction.");
        return;
    }
	close(player, directions[ch]);
	turn_is_ended = true;
}

