#pragma once
class Reader;
class Writer;
class Game;

void load(Reader & reader, Game & game);
void save(Writer & reader, const Game & game);

