#ifndef MENU_H
#define MENU_H

#include "util.h"

void startMenu(
	unsigned short *gameState,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs hotkeys
);

void settingsMenu(
	unsigned short *gameState,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs *hotkeys,
	Data *info
);

#endif
