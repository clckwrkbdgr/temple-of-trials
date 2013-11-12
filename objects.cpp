#include "objects.h"

Monster::Monster(const Point & monster_pos, const Sprite & monster_sprite, int monster_ai)
	: pos(monster_pos), sprite(monster_sprite), ai(monster_ai)
{
}

Monster::Monster()
	: sprite(' '), ai(AI_STILL)
{
}

Monster::operator bool() const
{
    return bool(pos);
}

Monster Monster::player(const Point & monster_pos)
{
	Monster player(monster_pos, '@', AI_PLAYER);
	player.name = "you";
	return player;
}

Monster Monster::ant(int ai, const Point & monster_pos)
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
