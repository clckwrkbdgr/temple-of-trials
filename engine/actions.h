#pragma once
#include "point.h"
#include "info.h"
class Game;
class Monster;

struct ActionException {
	unsigned type;
	Info subject, object;
	ActionException(unsigned exception_type, const Info & action_subject, const Info & action_object = Info());
};

class Action {
public:
	enum ExceptionType {
		UNKNOWN,

		ALREADY_CLOSED,
		ALREADY_FULL,
		ALREADY_OPENED,
		CANNOT_DRINK,
		CANNOT_EAT,
		CANNOT_GO_DOWN,
		CANNOT_GO_UP,
		CANNOT_WEAR,
		LOCKED,
		NOTHING_TO_CLOSE,
		NOTHING_TO_DRINK,
		NOTHING_TO_DROP,
		NOTHING_TO_EAT,
		NOTHING_TO_GRAB,
		NOTHING_TO_OPEN,
		NOTHING_TO_TAKE_OFF,
		NOTHING_TO_UNWIELD,
		NOTHING_TO_WEAR,
		NOTHING_TO_WIELD,
		NOTHING_TO_PUT,
		NOTHING_TO_THROW,
		NO_SPACE_LEFT,
		NO_SUCH_ITEM,
		HAS_NO_ITEMS,

		COUNT
	};
	typedef ActionException Exception;

	virtual ~Action() {}
	virtual void commit(Monster & someone, Game & game) = 0;
	void assert(bool expression, unsigned exception_type, const Info & subject, const Info & object = Info());
};

class DirectedAction : public Action {
public:
	const Point shift;

	DirectedAction(const Point & action_direction) : shift(action_direction) {}
	virtual ~DirectedAction() {}
};

class SlotAction : public Action {
public:
	int slot;

	SlotAction(int action_slot) : slot(action_slot) {}
	virtual ~SlotAction() {}
};


class Wait : public Action {
public:
	virtual void commit(Monster &, Game &) {}
};

class Move : public DirectedAction {
public:
	Move(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Open : public DirectedAction {
public:
	Open(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Close : public DirectedAction {
public:
	Close(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Swing : public DirectedAction {
public:
	Swing(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Fire : public DirectedAction {
public:
	Fire(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Drink : public DirectedAction {
public:
	Drink(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};

class Grab : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Drop : public SlotAction {
public:
	Drop(int slot) : SlotAction(slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class Wield : public SlotAction {
public:
	Wield(int slot) : SlotAction(slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class Unwield : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Wear : public SlotAction {
public:
	Wear(int slot) : SlotAction(slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class TakeOff : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Eat : public SlotAction {
public:
	Eat(int slot) : SlotAction(slot) {}
	virtual void commit(Monster & someone, Game & game);
};

class GoUp : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class GoDown : public Action {
public:
	virtual void commit(Monster & someone, Game & game);
};

class Put : public DirectedAction {
public:
	Put(const Point & shift) : DirectedAction(shift) {}
	virtual void commit(Monster & someone, Game & game);
};
