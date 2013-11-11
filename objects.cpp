#include "objects.h"

Monster::Monster(const Point & monster_pos, const Sprite & monster_sprite)
	: pos(monster_pos), sprite(monster_sprite)
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
	return Monster(monster_pos, '@');
}

Monster Monster::ant(const Point & monster_pos)
{
	return Monster(monster_pos, 'A');
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
