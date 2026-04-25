#include "pauseMenu.h"
#include "util.h"
#include "raymath.h"


void pauseMenu(
	unsigned short *gameState,
	unsigned short *gameStateTwo,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs hotkeys,
	Data *info,
	Font *DOOM
) {
	//Initialising Textures
	const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Stary.png");
	const Texture2D pointer = LoadTexture("Assets/Menu/pointer.png");

	// Base Sizes
	float baseButtonSize = 36.0f * 2;
	float baseTitleSize = 2.0f * baseButtonSize;
	float baseSpacing = baseButtonSize / 18.0f;

	FontData doomFont = {
		.FONT = DOOM,
		.size = baseButtonSize,
		.spacing = baseSpacing
	};

	FontData title = {
		.FONT = DOOM,
		.size = baseTitleSize,
		.spacing = baseSpacing
	};

	// Text
	const char titleText[] = "Paused";
	const char resumeText[] = "Resume";
	const char restartText[] = "Restart";
	const char backText[] = "Back";


	// Base positions
	Vector2 baseBackDimentions = MeasureTextEx(*DOOM, backText, baseButtonSize, baseSpacing);

	Vector2 mid = {SCREENWIDTH / 2, SCREENHEIGHT / 2};

	Vector2 baseTitlePos = centreText((Vector2){mid.x, mid.y / 4}, title, titleText);

	Vector2 baseResumePos = centreText((Vector2){mid.x, mid.y}, doomFont, resumeText);
	Vector2 baseRestartPos = centreText((Vector2){mid.x, SCREENHEIGHT * 0.75f}, doomFont, restartText);

	Vector2 baseBackPos = {10, SCREENHEIGHT - 10 - baseBackDimentions.y}; // centers it on only the y cordinate


	Vector2 backDimentions = baseBackDimentions;
	Vector2 titlePos = baseTitlePos;
	Vector2 resumePos = baseResumePos;
	Vector2 restartPos = baseRestartPos;
	Vector2 backPos = baseBackPos;

	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;

	// Pointer
	Vector2 pointerPos = {resumePos.x - 10 - pointer.width, resumePos.y}; // starts next to start button

	int curWidth = SCREENWIDTH;
	int curHeight = SCREENHEIGHT;

	unsigned short hoveredButton = 0;
	/*
		resume = 0 
		restart = 1
		back = 2
	*/


	Vector2 basePosArray[] = {
		baseTitlePos,
		baseResumePos,
		baseRestartPos,
		baseBackPos
	};

	Vector2 *posArray[] = {
		&titlePos,
		&resumePos,
		&restartPos,
		&backPos
	};

	int posArrayLen = sizeof(basePosArray) / sizeof(basePosArray[0]);

	bool inputReady = false;

	while(*gameStateTwo == 1 && *gameState == 2){
		if (WindowShouldClose()) *gameState = -1;

		if(checkWindowSize(&curWidth, &curHeight)){

			float scaleX = (float) curWidth / (float) SCREENWIDTH;
			float scaleY = (float) curHeight / (float) SCREENHEIGHT;

			buttonSize = baseButtonSize * scaleY;
			titleSize = baseTitleSize * scaleY;
			spacing = baseSpacing * scaleY;

			doomFont.size = buttonSize;
			doomFont.spacing = spacing;

			for (int i = 0; i <= posArrayLen -1; i++) {
				*posArray[i] = ScaleVector2(basePosArray[i], scaleX, scaleY);
			}
		}

		switch (hoveredButton) {
			case 0:
				pointerPos = (Vector2){resumePos.x - pointer.width - 10, resumePos.y};
				break;

			case 1:
				pointerPos = (Vector2){restartPos.x - pointer.width - 10, restartPos.y};
				break;

			case 2:
				pointerPos = (Vector2){backPos.x + backDimentions.x + 10, backPos.y};
				break;

			default:

				break;
		}

		if (getDown(hotkeys) && hoveredButton != 2) hoveredButton++;
		if (getUp(hotkeys) && hoveredButton != 0) hoveredButton--;

		 if (!inputReady) {
			inputReady = true;
		} else {
			if (getEnter(hotkeys)) {
				switch (hoveredButton) {
					case 0: // resume game
						*gameStateTwo = 0;
						break;

					case 1: // restart
						*gameState = 9;
						*gameStateTwo = 0;
						break;

					case 2: // back
						*gameState = 0;
						break;

					default:
						break;
				}
			}
		}

		BeginDrawing();
			ClearBackground(WHITE);
			
			// Background
			DrawTexturePro(
				bg, 
				(Rectangle){0, 0, bg.width, bg.height}, 
				(Rectangle){0, 0, curWidth, curHeight}, 
				(Vector2){0, 0}, 0, WHITE
			);
		
			//pointer
			DrawTexture(pointer, pointerPos.x, pointerPos.y, WHITE);

			// Draw title and shadow
			DrawTextEx(*DOOM, titleText, titlePos, titleSize, spacing, RED);

			//buttons
			DrawTextEx(*DOOM, resumeText, resumePos, buttonSize, spacing, RED);
			DrawTextEx(*DOOM, restartText, restartPos, buttonSize, spacing, RED);
			DrawTextEx(*DOOM, backText, backPos, buttonSize, spacing, RED);
		EndDrawing();
	}

	// Cleanup
	UnloadTexture(bg);
	UnloadTexture(pointer);
}