#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "util.h"
#include "physics.h"
#include "render.h"

#define TILE_COUNT 5



void gameLoop(
	unsigned short *gameState,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs hotkeys,
	Data *info,
	const int FRAMERATE
);



#endif
