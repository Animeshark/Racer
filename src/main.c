#include "raylib.h"
#include "settingsMenu.h"
#include "startMenu.h"
#include "gamePlay.h"
#include "carMenu.h"

int main(void){

	const int SCREENWIDTH = 1600;
	const int SCREENHEIGHT = 900;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "Racer");
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
		.musicVolume = 0.5f,
		.fov = 90.0f,
		.healthbarColour = RED
	};



	unsigned short gameState = 0;

	while (gameState <= 9)
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
				gameLoop(&gameState, SCREENWIDTH, SCREENHEIGHT, hotkeys, &info);
				break;
/*
			case 3:
				carMenu(&gameState, SCREENWIDTH, SCREENHEIGHT, hotkeys, &info, &player);
				break;
*/				

			case 9:
				gameState = 2;
				break;

			default:
				break;
		}
	}

	CloseWindow();
	return 0;
}
