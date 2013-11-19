#include "objects.h"

Monster::Monster(const Point & monster_pos, const Sprite & monster_sprite, int monster_sight, int monster_ai)
	: pos(monster_pos), sprite(monster_sprite), sight(monster_sight), ai(monster_ai)
{
}

Monster::Monster()
	: sprite(' '), sight(0)
{
}

Monster::operator bool() const
{
    return bool(pos);
}


Door::Door()
	: opened_sprite(' '), closed_sprite(' '), opened(false)
{
}

Door::Door(const Point & door_pos, const Sprite & opened_door_sprite, const Sprite & closed_door_sprite, bool is_opened)
	: pos(door_pos), opened_sprite(opened_door_sprite), closed_sprite(closed_door_sprite), opened(false)
{
}

Sprite Door::sprite() const
{
	return opened ? opened_sprite : closed_sprite;
}

Door::operator bool() const
{
    return bool(pos);
}
