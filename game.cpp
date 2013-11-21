#include "ai.h"
#include "game.h"
#include <map>
#include <cstdlib>

Control::Control(int control_value, const Point & control_direction)
	: control(control_value), direction(control_direction)
{
}


Game::Game()
	: map(1, 1, Cell()), done(false), turns(0)
{
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
		return new_pos;
	}
	return Point();
}

const Monster & Game::getPlayer() const
{
	for(unsigned i = 0; i < monsters.size(); ++i) {
		if(monsters[i].ai == AI::PLAYER) {
			return monsters[i];
		}
	}
	static Monster empty;
	return empty;
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

void Game::swing(Monster & someone, const Point & shift)
{
	if(!shift) {
		return;
	}
    Point new_pos = someone.pos + shift;
    Door & door = find_at(doors, new_pos);
    if(door && !door.opened) {
		message(format("{0} swing at door.", someone.name));
		open(someone, shift);
		return;
    }
    Monster & monster = find_at(monsters, new_pos);
	if(monster) {
		monster.hp -= 1;
		message(format("{0} hit {1} for 1 hp.", someone.name, monster.name));
		if(monster.is_dead()) {
			message(format("{0} kill {1}.", someone.name, monster.name));
		}
		return;
	}
	if(!map.is_passable(new_pos)) {
		message(format("{0} hit wall.", someone.name));
		return;
	}
    message(format("{0} swing at nothing.", someone.name));
}

