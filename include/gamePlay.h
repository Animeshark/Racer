#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "util.h"




void gameLoop(
	unsigned short *gameState,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs *hotkeys,
	Data *info,
	const int FRAMERATE
);



#endif
