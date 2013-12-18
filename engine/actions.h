#pragma once
#include "util.h"
class Game;
class Monster;

class Action {
public:
	virtual ~Action() {}
	virtual void commit(Monster & someone, Game & game) = 0;
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

