#include "objects.h"

Monster::Monster()
	: sprite(' '), sight(0), ai(0), hp(1)
{
}

Monster::operator bool() const
{
    return bool(pos);
}

Monster::Builder & Monster::Builder::pos(const Point & value) { monster.pos = value; return *this; }
Monster::Builder & Monster::Builder::sprite(const Sprite & value) { monster.sprite = value; return *this; }
Monster::Builder & Monster::Builder::sight(int value) { monster.sight = value; return *this; }
Monster::Builder & Monster::Builder::ai(int value) { monster.ai = value; return *this; }
Monster::Builder & Monster::Builder::hp(int value) { monster.hp = value; return *this; }
Monster::Builder & Monster::Builder::name(const std::string & value) { monster.name = value; return *this; }


Door::Door()
	: opened_sprite(' '), closed_sprite(' '), opened(false)
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

Door::Builder & Door::Builder::pos(const Point & value) { door.pos = value; return *this; }
Door::Builder & Door::Builder::opened_sprite(const Sprite & value) { door.opened_sprite = value; return *this; }
Door::Builder & Door::Builder::closed_sprite(const Sprite & value) { door.closed_sprite = value; return *this; }
Door::Builder & Door::Builder::opened(bool value) { door.opened = value; return *this; }
