#include "info.h"
#include "cell.h"
#include "items.h"
#include "monsters.h"
#include "objects.h"

static std::string invalid_id = "";
static int invalid_sprite = 0;
static std::string invalid_name = "";
static bool invalid_impassable = false;
static bool invalid_passable = true;
static bool invalid_opaque = false;
static bool invalid_transparent = true;


Info::Info()
	: id(invalid_id), sprite(invalid_sprite), name(invalid_name), passable(invalid_impassable), transparent(invalid_opaque)
{
}

Info::Info(const Item & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(invalid_passable), transparent(invalid_transparent)
{
}

Info::Info(const Object & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(base.type->passable), transparent(base.type->transparent)
{
}

Info::Info(const Monster & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(invalid_impassable), transparent(invalid_transparent)
{
}

Info::Info(const Cell & base)
	: id(base.type->id), sprite(base.type->sprite), name(base.type->name), passable(base.type->passable), transparent(base.type->transparent)
{
}

bool Info::valid() const
{
	return !id.empty();
}

