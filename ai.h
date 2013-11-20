#pragma once
class Monster;
class Game;
class Control;

namespace AI {
	enum { PLAYER, ANGRY_AND_WANDER, ANGRY_AND_STILL, CALM_AND_WANDER, CALM_AND_STILL };
}

Control player_control(Monster & player, Game & game);
Control angry_and_wander(Monster & monster, Game & game);
Control angry_and_still(Monster & monster, Game & game);
Control calm_and_wander(Monster & monster, Game & game);
Control calm_and_still(Monster & monster, Game & game);

