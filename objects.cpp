#include "objects.h"

Monster::Monster()
	: sprite(' '), sight(0), ai(0), hp(1), wielded(-1)
{
}

Monster::operator bool() const
{
    return bool(pos);
}

Monster::Builder & Monster::Builder::pos(const Point & value) { result.pos = value; return *this; }
Monster::Builder & Monster::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Monster::Builder & Monster::Builder::sight(int value) { result.sight = value; return *this; }
Monster::Builder & Monster::Builder::ai(int value) { result.ai = value; return *this; }
Monster::Builder & Monster::Builder::hp(int value) { result.hp = value; return *this; }
Monster::Builder & Monster::Builder::name(const std::string & value) { result.name = value; return *this; }
Monster::Builder & Monster::Builder::item(const Item & value) { result.inventory.push_back(value); return *this; }


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

Door::Builder & Door::Builder::pos(const Point & value) { result.pos = value; return *this; }
Door::Builder & Door::Builder::opened_sprite(const Sprite & value) { result.opened_sprite = value; return *this; }
Door::Builder & Door::Builder::closed_sprite(const Sprite & value) { result.closed_sprite = value; return *this; }
Door::Builder & Door::Builder::opened(bool value) { result.opened = value; return *this; }


Item::Item()
	: sprite(' ')
{
}

Item::operator bool() const
{
	return bool(pos);
}

Item::Builder & Item::Builder::pos(const Point & value) { result.pos = value; return *this; }
Item::Builder & Item::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Item::Builder & Item::Builder::name(const std::string & value) { result.name = value; return *this; }


Container::Container()
	: sprite(' ')
{
}

Container::operator bool() const
{
	return bool(pos);
}

Container::Builder & Container::Builder::pos(const Point & value) { result.pos = value; return *this; }
Container::Builder & Container::Builder::sprite(const Sprite & value) { result.sprite = value; return *this; }
Container::Builder & Container::Builder::name(const std::string & value) { result.name = value; return *this; }
Container::Builder & Container::Builder::item(const Item & value) { result.items.push_back(value); return *this; }
