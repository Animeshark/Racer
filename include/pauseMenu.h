#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "util.h"

void pauseMenu(
	unsigned short *gameState,
	unsigned short *gameStateTwo,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs hotkeys,
	Data *info,
	Font *DOOM
);


#endif