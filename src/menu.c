#include "menu.h"
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
	Vector2 baseExitPos = {10, SCREENHEIGHT - 10 - baseExitDimentions.y}; // centers it on only the y cordinate

	
	Vector2 exitDimentions = baseExitDimentions;
	Vector2 titlePos = baseTitlePos;
	Vector2 titleShadowPos = baseTitleShadowPos;

	Vector2 startPos = baseStartPos;
	Vector2 settingsPos = baseSettingsPos;
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
		settings = 1
		exit = 2
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
			exitPos = ScaleVector2(baseExitPos, scaleX, scaleY);
			exitDimentions = ScaleVector2(baseExitDimentions, scaleX, scaleY);
		}
		
		switch (hoveredButton) {
			case 0:
				pointerPos = (Vector2){startPos.x - pointer.width - 10, startPos.y};
				break;
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
		DrawTextEx(DOOM, exitText, exitPos, buttonSize, spacing, RED);

	EndDrawing();
	}

	// Cleanup
	UnloadTexture(bg);
	UnloadTexture(pointer);
	UnloadFont(DOOM);
}


void settingsMenu(unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs *hotkeys, Data *info, const int FRAMERATE){
	
	//Initialising Textures
	const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Stary.png");
	const Texture2D pointer = LoadTexture("Assets/Menu/pointer.png");

	const Font DOOM = LoadFontEx("Assets/font/Amazdoomleft-epw3.ttf", 128, 0, 0);
	SetTextureFilter(DOOM.texture, TEXTURE_FILTER_POINT);

	// Base Sizes
	float baseButtonSize = 36.0f * 2;
	float baseTitleSize = 2.0f * baseButtonSize;
	float baseSpacing = baseButtonSize / 18.0f;
	
	FontData doomFont = {
		.FONT = &DOOM,
		.size = baseButtonSize,
		.spacing = baseSpacing
	};



	// Text 
	const char titleText[] = "Settings";
	
	const char hotkeyText[] = "Hotkeys";
	const char healthText[] = "Healthbar Colour:";
	const char musicText[] = "Music Volume:";
	const char gameText[] = "Game Volume:";
	const char fovText[] = "FOV:";
	const char enemyText[] = "Enemy Difficulty:";
	const char backText[] = "Back";

	// Hotkey text
	const char upText[] = "Up:";
	const char leftText[] = "Left:";
	const char rightText[] = "Right:";
	const char downText[] = "Down:";
	const char enterText[] = "Enter:";

	// Hotkey values
	char upValueText1[8];
	char upValueText2[8];
	char leftValueText1[8];
	char leftValueText2[8];
	char rightValueText1[8];
	char rightValueText2[8];
	char downValueText1[8];
	char downValueText2[8];
	char enterValueText1[8];
	char enterValueText2[8];

	KeyToString(upValueText1, hotkeys->up[0]);
	KeyToString(upValueText2, hotkeys->up[1]);
	KeyToString(leftValueText1, hotkeys->left[0]);
	KeyToString(leftValueText2, hotkeys->left[1]);
	KeyToString(rightValueText1, hotkeys->right[0]);
	KeyToString(rightValueText2, hotkeys->right[1]);
	KeyToString(downValueText1, hotkeys->down[0]);
	KeyToString(downValueText2, hotkeys->down[1]);
	KeyToString(enterValueText1, hotkeys->enter[0]);
	KeyToString(enterValueText2, hotkeys->enter[1]);

	// Health value
	char healthValueText[8];

	ColourToString(healthValueText, info->healthbarColour);

	Color colourArray[] = {RED, GOLD, BLUE, PINK, PURPLE};
	unsigned short colourArrayLen = sizeof(colourArray)/sizeof(RED);
	unsigned short colourIndex;
	// Linear search for the index of current colour
	for(unsigned short i = 0; i < colourArrayLen; i++){
		if(ColorIsEqual(colourArray[i], info->healthbarColour)) colourIndex = i;
	}

	// Volumes
	char musicValueText[5];
	char gameValueText[5];

	VolumeToString(musicValueText, info->musicVolume);
	VolumeToString(gameValueText, info->gameVolume);
	
	// Fov
	char fovValueText[12];
	sprintf(fovValueText, "%.0f", info->fov);


	// Enemy difficulty
	char enemyValueText[9];

	DifficultyToString(enemyValueText, info->enemyDifficulty);

	Color enemyValueTextColour = DifficultyToColour(info->enemyDifficulty);


	// Base Positions
	int leftMargin = pointer.width + 20;
	int baseButtonSpacing = 20;

	// Persistant images
	Vector2 baseTitlePos = {40, 40};
	Vector2 baseHotkeyPos = {leftMargin, baseTitlePos.y + baseTitleSize + 40};
	Vector2 baseHealthPos = {leftMargin, baseHotkeyPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseMusicPos = {leftMargin, baseHealthPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseGamePos = {leftMargin, baseMusicPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseFovPos = {leftMargin, baseGamePos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseEnemyPos = {leftMargin, baseFovPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseBackPos = {leftMargin, SCREENHEIGHT - baseButtonSize - 10};


	int rightMargin = SCREENWIDTH/2 + pointer.width + 20;

	// Hotkey Positions
	Vector2 baseUpPos = {rightMargin, baseHealthPos.y};
	Vector2 baseLeftPos = {rightMargin, baseUpPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseRightPos = {rightMargin, baseLeftPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseDownPos = {rightMargin, baseRightPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseEnterPos = {rightMargin, baseDownPos.y + baseButtonSize + baseButtonSpacing};

	
	Vector2 upValuePos1 = PointerValuePos(baseUpPos, doomFont, upText, pointer.width);
	Vector2 upValuePos2 = PointerValuePos(upValuePos1, doomFont, upValueText1, pointer.width);

	Vector2 leftValuePos1 = PointerValuePos(baseLeftPos, doomFont, leftText, pointer.width);
	Vector2 leftValuePos2 = PointerValuePos(leftValuePos1, doomFont, leftValueText1, pointer.width);

	Vector2 rightValuePos1 = PointerValuePos(baseRightPos, doomFont, rightText, pointer.width);
	Vector2 rightValuePos2 = PointerValuePos(rightValuePos1, doomFont, rightValueText1, pointer.width);

	Vector2 downValuePos1 = PointerValuePos(baseDownPos, doomFont, downText, pointer.width);
	Vector2 downValuePos2 = PointerValuePos(downValuePos1, doomFont, downValueText1, pointer.width);

	Vector2 enterValuePos1 = PointerValuePos(baseEnterPos, doomFont, enterText, pointer.width);
	Vector2 enterValuePos2 = PointerValuePos(enterValuePos1, doomFont, enterValueText1, pointer.width);

	// Health colour position

	Vector2 healthValuePos = ValuePos(baseHealthPos, doomFont, healthText);

	// Volume positions
	
	Vector2 musicValuePos = ValuePos(baseMusicPos, doomFont, musicText);
	Vector2 gameValuePos = ValuePos(baseGamePos, doomFont, gameText);
	Vector2 fovValuePos = ValuePos(baseFovPos, doomFont, fovText);

	// Enemy difficulty position

	Vector2 enemyValuePos = ValuePos(baseEnemyPos, doomFont, enemyText);



	// Dynamic Positions

	Vector2 titlePos = baseTitlePos;
	Vector2 hotkeyPos = baseHotkeyPos;
	Vector2 healthPos = baseHealthPos;
	Vector2 musicPos = baseMusicPos;
	Vector2 gamePos = baseGamePos;
	Vector2 fovPos = baseFovPos;
	Vector2 enemyPos = baseEnemyPos;
	Vector2 backPos = baseBackPos;

	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;

	Vector2 upPos = baseUpPos;
	Vector2 leftPos = baseLeftPos;
	Vector2 rightPos = baseRightPos;
	Vector2 downPos = baseDownPos;
	Vector2 enterPos = baseEnterPos;

	


	// Pointer
	Vector2 *initialPointerPositions[] = {
	&hotkeyPos,
	&healthPos,
	&musicPos,
	&gamePos,
	&fovPos,
	&enemyPos,
	&backPos
	};

	
	// Hotkey pointer

	Vector2 *hotkeyPointerPositions[][2] = {
		{&upValuePos1, &upValuePos2},
		{&leftValuePos1, &leftValuePos2},
		{&rightValuePos1, &rightValuePos2},
		{&downValuePos1, &downValuePos2},
		{&enterValuePos1, &enterValuePos2}
	};


	// Position arrays

	Vector2 basePosArray[] = {
		baseTitlePos,
		baseHotkeyPos,
		baseHealthPos,
		baseMusicPos,
		baseGamePos,
		baseFovPos,
		baseEnemyPos,
		baseBackPos,
		baseUpPos,
		baseLeftPos,
		baseRightPos,
		baseDownPos,
		baseEnterPos
	};

	Vector2 *posArray[] = {
		&titlePos,
		&hotkeyPos,
		&healthPos,
		&musicPos,
		&gamePos,
		&fovPos,
		&enemyPos,
		&backPos,
		&upPos,
		&leftPos,
		&rightPos,
		&downPos,
		&enterPos
	};





	int posArrayLen = sizeof(basePosArray)/sizeof(basePosArray[0]);

	int pointerOffset = 10 + pointer.width;

	Vector2 pointerPos;

	int curWidth = SCREENWIDTH;
	int curHeight = SCREENHEIGHT;

	unsigned short selectedSetting = 0;

	unsigned short pointerPosY = 0;
	bool pointerBoard = true; // true is left board false is right board

	unsigned short columnBottom = 0;
	bool pointerHotkeyColumn = true; // true means the left hotkey value is being changed

	bool waitingForKey = false;
	
	while(*gameState == 1){
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

			// Hotkeys
			upValuePos1 = PointerValuePos(upPos, doomFont, upText, pointer.width);
			upValuePos2 = PointerValuePos(upValuePos1, doomFont, upValueText1, pointer.width);

			leftValuePos1 = PointerValuePos(leftPos, doomFont, leftText, pointer.width);
			leftValuePos2 = PointerValuePos(leftValuePos1, doomFont, leftValueText1, pointer.width);

			rightValuePos1 = PointerValuePos(rightPos, doomFont, rightText, pointer.width);
			rightValuePos2 = PointerValuePos(rightValuePos1, doomFont, rightValueText1, pointer.width);

			downValuePos1 = PointerValuePos(downPos, doomFont, downText, pointer.width);
			downValuePos2 = PointerValuePos(downValuePos1, doomFont, downValueText1, pointer.width);

			enterValuePos1 = PointerValuePos(enterPos, doomFont, enterText, pointer.width);
			enterValuePos2 = PointerValuePos(enterValuePos1, doomFont, enterValueText1, pointer.width);

			// Health colour
			healthValuePos = ValuePos(healthPos, doomFont, healthText);

			// Volumes
			musicValuePos = ValuePos(musicPos, doomFont, musicText);
			gameValuePos = ValuePos(gamePos, doomFont, gameText);
			fovValuePos = ValuePos(fovPos, doomFont, fovText);

			// Enemy difficulty
			enemyValuePos = ValuePos(enemyPos, doomFont, enemyText);
		}
		



		pointerPos = (Vector2){initialPointerPositions[pointerPosY]->x - pointerOffset, initialPointerPositions[pointerPosY]->y};

		switch (selectedSetting)
		{
		case 0:
			columnBottom = 6; // len(initialPointerPositions)

			if(getUp(*hotkeys) && pointerPosY > 0) pointerPosY--;

			if(getDown(*hotkeys) && pointerPosY < columnBottom) pointerPosY++;

			if(getEnter(*hotkeys)) selectedSetting = pointerPosY + 1;
			break;

		case 1:
			if(pointerBoard) columnBottom = 6; // len(initialPointerPositions)
			else columnBottom = 4; // len(hotkeyPointerPositions)

			if (!pointerBoard) {
				pointerPos = (Vector2) {hotkeyPointerPositions[pointerPosY][!pointerHotkeyColumn]->x - pointerOffset, hotkeyPointerPositions[pointerPosY][!pointerHotkeyColumn]->y};
			}

			if (waitingForKey) {
				int key = GetKeyPressed();
				if (key != 0 && key != hotkeys->enter[0] && key != hotkeys->enter[1]) {
					waitingForKey = false;
					switch (pointerPosY)
					{
					case 0: // UP
						hotkeys->up[!pointerHotkeyColumn] = key;
						if (pointerHotkeyColumn) KeyToString(upValueText1, key);
						else KeyToString(upValueText2, key);
						break;

					case 1: // LEFT
						hotkeys->left[!pointerHotkeyColumn] = key;
						if (pointerHotkeyColumn) KeyToString(leftValueText1, key);
						else KeyToString(leftValueText2, key);
						break;

					case 2: // RIGHT
						hotkeys->right[!pointerHotkeyColumn] = key;
						if (pointerHotkeyColumn) KeyToString(rightValueText1, key);
						else KeyToString(rightValueText2, key);
						break;

					case 3: // DOWN
						hotkeys->down[!pointerHotkeyColumn] = key;
						if (pointerHotkeyColumn) KeyToString(downValueText1, key);
						else KeyToString(downValueText2, key);
						break;

					case 4: // ENTER
						hotkeys->enter[!pointerHotkeyColumn] = key;
						if (pointerHotkeyColumn) KeyToString(enterValueText1, key);
						else KeyToString(enterValueText2, key);
						break;

					default:
						break;
					}
				}
			}
			else {
				if(getUp(*hotkeys) && pointerPosY > 0) pointerPosY--;

				if(getDown(*hotkeys) && pointerPosY < columnBottom) pointerPosY++;
						
				if(getRight(*hotkeys)) {
					if(pointerBoard) {
						pointerBoard = !pointerBoard;
						pointerPosY = 0;
					}
					else if(pointerHotkeyColumn) pointerHotkeyColumn = !pointerHotkeyColumn;
				}

				if(getLeft(*hotkeys)) {
					if(!pointerHotkeyColumn) pointerHotkeyColumn = !pointerHotkeyColumn;
					else if(!pointerBoard) {
						pointerBoard = !pointerBoard;
						pointerPosY = 0;
					}
				}

				if(getEnter(*hotkeys)) {
				if(pointerBoard) selectedSetting = pointerPosY + 1;
				else waitingForKey = true;
				}
			}
			break;

		case 2:
			if(getUp(*hotkeys) && colourIndex > 0) {
				colourIndex--;
				info->healthbarColour = colourArray[colourIndex];
				ColourToString(healthValueText, info->healthbarColour);
			}

			if(getDown(*hotkeys) && colourIndex < colourArrayLen - 1) {
				colourIndex++;
				info->healthbarColour = colourArray[colourIndex];
				ColourToString(healthValueText, info->healthbarColour);
			}
			
			if(getEnter(*hotkeys)) selectedSetting = 0;
			break;

		case 3:
			if(getUp(*hotkeys) && info->musicVolume <= 1) {
				info->musicVolume += 0.05f;
				VolumeToString(musicValueText, info->musicVolume);
			}

			if(getDown(*hotkeys) && info->musicVolume > 0) {
				info->musicVolume -= 0.05f;
				VolumeToString(musicValueText, info->musicVolume);
			}
			
			if(getEnter(*hotkeys)) selectedSetting = 0;
			break;

		case 4:
			if(getUp(*hotkeys) && info->gameVolume <= 1) {
				info->gameVolume += 0.05f;
				VolumeToString(gameValueText, info->gameVolume);
			}

			if(getDown(*hotkeys) && info->gameVolume > 0) {
				info->gameVolume -= 0.05f;
				VolumeToString(gameValueText, info->gameVolume);
			}
			
			if(getEnter(*hotkeys)) selectedSetting = 0;
			break;

		case 5:
			if(getUpHeld(*hotkeys) && info->fov < 120) {
				info->fov += 10.0f / FRAMERATE;
				sprintf(fovValueText, "%.0f", info->fov);
			}

			if(getDownHeld(*hotkeys) && info->fov > 30) {
				info->fov -= 10.0f / FRAMERATE;
				sprintf(fovValueText, "%.0f", info->fov);
			}
			
			if(getEnter(*hotkeys)) selectedSetting = 0;
			break;

		case 6:
								// 3 is the max dificulty rn may be changed to 4 
			if(getUp(*hotkeys) && info->enemyDifficulty < 3) {
				info->enemyDifficulty++;
				DifficultyToString(enemyValueText, info->enemyDifficulty);
				enemyValueTextColour = DifficultyToColour(info->enemyDifficulty);
			}

			if(getDown(*hotkeys) && info->enemyDifficulty > 0) {
				info->enemyDifficulty--;
				DifficultyToString(enemyValueText, info->enemyDifficulty);
				enemyValueTextColour = DifficultyToColour(info->enemyDifficulty);
			}
			
			if(getEnter(*hotkeys)) selectedSetting = 0;
			break;

		case 7:

			*gameState = 0;
			break;

		default:
			break;
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

			// Draw settings title
			DrawTextEx(DOOM, titleText, titlePos, titleSize, spacing, RED);

			// Draw setting options
			DrawTextEx(DOOM, hotkeyText, hotkeyPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, healthText, healthPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, musicText, musicPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, gameText, gamePos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, fovText, fovPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, enemyText, enemyPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, backText, backPos, buttonSize, spacing, RED);

			// Draw values for scrollable settings

			DrawTextEx(DOOM, healthValueText, healthValuePos, buttonSize, spacing, info->healthbarColour);

			DrawTextEx(DOOM, musicValueText, musicValuePos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, gameValueText, gameValuePos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, fovValueText, fovValuePos, buttonSize, spacing, RED);

			DrawTextEx(DOOM, enemyValueText, enemyValuePos, buttonSize, spacing, enemyValueTextColour);

			if (selectedSetting == 1){
				// hotkeys
				DrawTextEx(DOOM, upText, upPos, buttonSize, spacing, RED);
				DrawTextEx(DOOM, leftText, leftPos, buttonSize, spacing, RED);
				DrawTextEx(DOOM, rightText, rightPos, buttonSize, spacing, RED);
				DrawTextEx(DOOM, downText, downPos, buttonSize, spacing, RED);
				DrawTextEx(DOOM, enterText, enterPos, buttonSize, spacing, RED);

				// Values
				DrawTextEx(DOOM, upValueText1, upValuePos1, buttonSize, spacing, GREEN);
				DrawTextEx(DOOM, upValueText2, upValuePos2, buttonSize, spacing, GREEN);

				DrawTextEx(DOOM, leftValueText1, leftValuePos1, buttonSize, spacing, GREEN);
				DrawTextEx(DOOM, leftValueText2, leftValuePos2, buttonSize, spacing, GREEN);

				DrawTextEx(DOOM, rightValueText1, rightValuePos1, buttonSize, spacing, GREEN);
				DrawTextEx(DOOM, rightValueText2, rightValuePos2, buttonSize, spacing, GREEN);

				DrawTextEx(DOOM, downValueText1, downValuePos1, buttonSize, spacing, GREEN);
				DrawTextEx(DOOM, downValueText2, downValuePos2, buttonSize, spacing, GREEN);

				DrawTextEx(DOOM, enterValueText1, enterValuePos1, buttonSize, spacing, GREEN);
				DrawTextEx(DOOM, enterValueText2, enterValuePos2, buttonSize, spacing, GREEN);
			}

		EndDrawing();
	}
	UnloadTexture(bg);
	UnloadTexture(pointer);
	UnloadFont(DOOM);
}


