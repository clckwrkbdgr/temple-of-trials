#pragma once
class Game;
class Monster;
class Point;

void smart_move(Game & game, Monster & someone, const Point & shift);
void move(Game & game, Monster & someone, const Point & shift);
void open(Game & game, Monster & someone, const Point & shift);
void close(Game & game, Monster & someone, const Point & shift);
void swing(Game & game, Monster & someone, const Point & shift);
void fire(Game & game, Monster & someone, const Point & shift);
void drink(Game & game, Monster & someone, const Point & shift);
void grab(Game & game, Monster & someone);
void drop(Game & game, Monster & someone, int slot);
void wield(Game & game, Monster & someone, int slot);
void unwield(Game & game, Monster & someone);
void wear(Game & game, Monster & someone, int slot);
void take_off(Game & game, Monster & someone);
void eat(Game & game, Monster & someone, int slot);
void go_up(Game & game, Monster & someone);
void go_down(Game & game, Monster & someone);
