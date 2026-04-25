#include "startMenu.h"
#include "raymath.h"
#include <stdio.h>

void startMenu(unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs hotkeys) {
	
	//Font
	const Font DOOM = LoadFontEx("Assets/font/Amazdoomleft-epw3.ttf", 128, 0, 0);
	SetTextureFilter(DOOM.texture, TEXTURE_FILTER_POINT);

	//Initialising textures
	const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Stary.png");
	const Texture2D pointer = LoadTexture("Assets/Menu/pointer.png");

	//text
	const char titleText[] = "SUPER RACEY RACER";
	const char startText[] = "Start";
	const char exitText[] = "Exit";
	const char settingsText[] = "Settings";
	const char carText[] = "Garage";

	/* Sizes
	They are floats although will only hold integer values. 
	Scaling with integers leads to smooth edges.
	Floats are used to avoid realtime casting in the raylib text func*/

	float baseButtonSize = 36.0f * 2; //Multiplying 36 because I want the resolution to be a clean multi
	float baseTitleSize = 2.0f * baseButtonSize;
	float baseSpacing = baseButtonSize / 18.0f;

	FontData button = {
		.FONT = &DOOM,
		.size = baseButtonSize,
		.spacing = baseSpacing
	};

	FontData title = {
		.FONT = &DOOM,
		.size = baseTitleSize,
		.spacing = baseSpacing
	};
	
	// Exit needs dimentions because he is reletive to a side
	Vector2 baseExitDimentions = MeasureTextEx(DOOM, exitText, baseButtonSize, baseSpacing);

	// Positions
	Vector2 mid = {SCREENWIDTH/2, SCREENHEIGHT/2};

	Vector2 baseTitlePos = centreText((Vector2){mid.x, mid.y/4}, title, titleText);
	Vector2 baseTitleShadowPos = {baseTitlePos.x + 5, baseTitlePos.y + 5}; // Offseting shadow

	// Buttons
	Vector2 baseStartPos = centreText((Vector2){mid.x, mid.y}, button, startText);
	Vector2 baseSettingsPos = centreText((Vector2){mid.x, SCREENHEIGHT * 0.75f}, button, settingsText);
	Vector2 baseCarPos = centreText((Vector2){mid.x, SCREENHEIGHT * 0.625f}, button, carText);
	Vector2 baseExitPos = {10, SCREENHEIGHT - 10 - baseExitDimentions.y}; // centers it on only the y cordinate

	
	Vector2 exitDimentions = baseExitDimentions;
	Vector2 titlePos = baseTitlePos;
	Vector2 titleShadowPos = baseTitleShadowPos;

	Vector2 startPos = baseStartPos;
	Vector2 settingsPos = baseSettingsPos;
	Vector2 carPos = baseCarPos;
	Vector2 exitPos = baseExitPos;
	
	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;

	// Pointer
	Vector2 pointerPos = {startPos.x - 10 - pointer.width, startPos.y}; // starts next to start button

	int curWidth = SCREENWIDTH;
	int curHeight = SCREENHEIGHT;

	unsigned short hoveredButton = 0;
	/*
		start = 0 
		garage = 1
		settings = 2
		exit = 3
	*/

	while(*gameState == 0){

		if (WindowShouldClose()) *gameState = -1; // will set gameState to bit integer limit, used for exit

		if (checkWindowSize(&curWidth, &curHeight)) {

			//readjusting size
			float scaleX = (float) curWidth / (float) SCREENWIDTH;
			float scaleY = (float) curHeight / (float) SCREENHEIGHT;
			
			buttonSize = baseButtonSize * scaleY;
			titleSize = baseTitleSize * scaleY;
			spacing = baseSpacing * scaleY;

			titlePos = ScaleVector2(baseTitlePos, scaleX, scaleY);
			titleShadowPos = ScaleVector2(baseTitleShadowPos, scaleX, scaleY);
			startPos = ScaleVector2(baseStartPos, scaleX, scaleY);
			settingsPos = ScaleVector2(baseSettingsPos, scaleX, scaleY);
//			carPos = ScaleVector2(baseCarPos, scaleX, scaleY);
			exitPos = ScaleVector2(baseExitPos, scaleX, scaleY);
			exitDimentions = ScaleVector2(baseExitDimentions, scaleX, scaleY);
		}
		
		switch (hoveredButton) {
			case 0:
				pointerPos = (Vector2){startPos.x - pointer.width - 10, startPos.y};
				break;
/*			case 1:
				pointerPos = (Vector2){carPos.x - pointer.width - 10, carPos.y};
				break;*/
			case 1:
				pointerPos = (Vector2){settingsPos.x - pointer.width - 10, settingsPos.y};
				break;
			case 2:
				pointerPos = (Vector2){exitPos.x + exitDimentions.x + 10, exitPos.y};
				break;
			default:
				printf("%s", "Error with pointer selection in start menu");
				break;
		}

		// Inputhandling
		if (getDown(hotkeys) && hoveredButton != 2) hoveredButton++;
		if (getUp(hotkeys) && hoveredButton != 0) hoveredButton--;

		if (getEnter(hotkeys)) {
			switch (hoveredButton) {
				case 0: *gameState = 2; break; // Start game
//				case 1: *gameState = 3; break; // Car customisation
				case 1: *gameState = 1; break; // Settings
				case 2: *gameState = -1; break; // Exit
				default: break;
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
		DrawTextEx(DOOM, titleText, titleShadowPos, titleSize, spacing, BLUE);
		DrawTextEx(DOOM, titleText, titlePos, titleSize, spacing, RED);

		//buttons
		DrawTextEx(DOOM, startText, startPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, settingsText, settingsPos, buttonSize, spacing, RED);
//		DrawTextEx(DOOM, carText, carPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, exitText, exitPos, buttonSize, spacing, RED);

	EndDrawing();
	}

	// Cleanup
	UnloadTexture(bg);
	UnloadTexture(pointer);
	UnloadFont(DOOM);
}