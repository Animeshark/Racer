#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "util.h"

#define MIN_SPEED 0.003f


void gameLoop(
	unsigned short *gameState,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs hotkeys,
	Data *info,
	const int FRAMERATE
);



#endif
