#include "objects.h"

Player::Player(const Point & player_pos)
	: pos(player_pos)
{
}

Sprite Player::sprite() const
{
	return '@';
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
