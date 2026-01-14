#include "raylib.h"
#include "menu.h"
#include "gamePlay.h"

int main(void)
{
	const int SCREENWIDTH = 1600;
	const int SCREENHEIGHT = 900;
	const int FRAMERATE = 60;

	InitWindow(SCREENWIDTH, SCREENHEIGHT, "Racer");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(FRAMERATE);
	SetExitKey(KEY_NULL);

	Inputs hotkeys = {
		.up = {KEY_W, KEY_UP},
		.down = {KEY_S, KEY_DOWN},
		.left = {KEY_A, KEY_LEFT},
		.right = {KEY_D, KEY_RIGHT},
		.enter = {KEY_ENTER, KEY_SPACE}
	};

	Data info = {
		.enemyDifficulty = 1,
		.gameVolume = 0.5f,
		.menuVolume = 0.5f,
		.musicVolume = 0.5f,
		.healthbarColour = RED
	};

	unsigned short gameState = 0;

	while (gameState <= 3)
	{
		switch (gameState)
		{
			case 0:
				startMenu(&gameState, SCREENWIDTH, SCREENHEIGHT, hotkeys);
				break;

			case 1:
				settingsMenu(&gameState, SCREENWIDTH, SCREENHEIGHT, &hotkeys, &info);
				break;

			case 2:
				gameLoop(&gameState, SCREENWIDTH, SCREENHEIGHT, &hotkeys, &info, FRAMERATE);
				break;

			default:
				break;
		}
	}

	CloseWindow();
	return 0;
}
