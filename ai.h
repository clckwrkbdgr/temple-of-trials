#pragma once
class Action;
class Monster;
class Game;

namespace AI {
	enum { DUMMY, PLAYER, ANGRY_AND_WANDER, ANGRY_AND_STILL, CALM_AND_STILL };
}

Action * player_control(Monster & player, Game & game);
Action * angry_and_wander(Monster & monster, Game & game);
Action * angry_and_still(Monster & monster, Game & game);
Action * calm_and_still(Monster & monster, Game & game);

