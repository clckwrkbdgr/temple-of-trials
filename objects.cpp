#include "objects.h"

AI::AI(int ai_faction, int ai_movement)
	: faction(ai_faction), movement(ai_movement)
{
}


Monster::Monster(const Point & monster_pos, const Sprite & monster_sprite, const AI & monster_ai)
	: pos(monster_pos), sprite(monster_sprite), ai(monster_ai)
{
}

Monster::Monster()
	: sprite(' ')
{
}

Monster::operator bool() const
{
    return bool(pos);
}

Monster Monster::player(const Point & monster_pos)
{
	Monster player(monster_pos, '@', AI(AI::PLAYER));
	player.name = "you";
	return player;
}

Monster Monster::ant(const AI & ai, const Point & monster_pos)
{
	Monster ant(monster_pos, 'A', ai);
	ant.name = "ant";
	return ant;
}


Door::Door(const Point & door_pos, bool is_opened)
	: pos(door_pos), opened(false)
{
}

Sprite Door::sprite() const
{
	return opened ? '-' : '+';
}

Door::operator bool() const
{
    return bool(pos);
}
