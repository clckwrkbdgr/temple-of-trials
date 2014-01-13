#pragma once
namespace Chthon {
	class Reader;
	class Writer;
	class Game;
}

void load(Chthon::Reader & reader, Chthon::Game & game);
void save(Chthon::Writer & reader, const Chthon::Game & game);

