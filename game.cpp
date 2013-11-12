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

void Game::generate()
{
	log("Generating new game...");
	map = Map(60, 23, Cell::floor());
	player = Monster::player(Point());
	for(int i = 0; i < 10; ++i) {
		map.cell(rand() % map.get_width(), rand() % map.get_height()) = Cell::wall();
	}
	for(int i = 0; i < 5; ++i) {
		doors.push_back(Door(Point(rand() % map.get_width(), rand() % map.get_height())));
	}
	for(int i = 0; i < 5; ++i) {
		int ai = (rand() % 2) ? Monster::AI_STILL : Monster::AI_WANDER;
		monsters.push_back(Monster::ant(ai, Point(rand() % map.get_width(), rand() % map.get_height())));
	}
	log("Done.");
}

Door & Game::door_at(const Point & pos)
{
    for(unsigned i = 0; i < doors.size(); ++i) {
        if(doors[i].pos == pos) {
            return doors[i];
        }
    }
    static Door defaultDoor;
    defaultDoor = Door();
    return defaultDoor;
}

Monster & Game::monster_at(const Point & pos)
{
    for(unsigned i = 0; i < monsters.size(); ++i) {
        if(monsters[i].pos == pos) {
            return monsters[i];
        }
    }
    static Monster defaultMonster;
    defaultMonster = Monster();
    return defaultMonster;
}

void Game::message(const std::string & text)
{
	messages.push_back(text);
	log("Message: " + text);
}

void Game::process(int ch)
{
	if(messages.size() > 1) {
		if(ch == ' ') {
			messages.pop_front();
		}
		if(ch == 'q') {
			mode = EXIT_MODE;
		}
		return;
	}
	if(messages.size() == 1) {
		messages.pop_front();
	}

	switch(mode) {
		case OPEN_MODE: process_open_mode(ch); break;
		case CLOSE_MODE: process_close_mode(ch); break;
		case NORMAL_MODE: process_normal_mode(ch); break;
		case EXIT_MODE: break;
		default: log("Unknown game mode!"); break;
	}
	if(turn_is_ended) {
		for(unsigned i = 0; i < monsters.size(); ++i) {
			if(monsters[i].ai == Monster::AI_WANDER) {
				Point shift(rand() % 3 - 1, rand() % 3 - 1);
				if(!shift) {
					continue;
				}
				Point new_pos = monsters[i].pos + shift;
				if(!map.is_passable(new_pos)) {
					message("Monster bump into the wall.");
					continue;
				}
				Door & door = door_at(new_pos);
				if(door && !door.opened) {
					message("Door is closed.");
					continue;
				}
				Monster & monster = monster_at(new_pos);
				if(monster) {
					message("Monster bump into the monster.");
					continue;
				}
				if(player.pos == new_pos) {
					message("Monster bump into you.");
					continue;
				}
				monsters[i].pos = new_pos;
			}
		}
		++turns;
		turn_is_ended = false;
	}
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
    Point new_pos = player.pos + directions[ch];
	turn_is_ended = true;
    if(!map.is_passable(new_pos)) {
        message("You bump into the wall.");
        return;
    }
    Door & door = door_at(new_pos);
    if(door && !door.opened) {
        message("Door is closed.");
        return;
    }
    Monster & monster = monster_at(new_pos);
    if(monster) {
        message("You bump into the monster.");
        return;
    }
    player.pos = new_pos;
}

void Game::process_open_mode(int ch)
{
	mode = Game::NORMAL_MODE;
	if(directions.count(ch) == 0) {
		message("This is not a direction.");
        return;
    }
	turn_is_ended = true;
    Point new_pos = player.pos + directions[ch];
    Door & door = door_at(new_pos);
    if(!door) {
        message("There is nothing to open there.");
        return;
    }
    if(door.opened) {
        message("Door is already opened.");
        return;
    }
    door.opened = true;
    message("You opened the door.");
}

void Game::process_close_mode(int ch)
{
	mode = Game::NORMAL_MODE;
	if(directions.count(ch) == 0) {
		message("This is not a direction.");
        return;
    }
	turn_is_ended = true;
    Point new_pos = player.pos + directions[ch];
    Door & door = door_at(new_pos);
    if(!door) {
        message("There is nothing to close there.");
        return;
    }
    if(!door.opened) {
        message("Door is already closed.");
        return;
    }
    door.opened = false;
    message("You closed the door.");
}

