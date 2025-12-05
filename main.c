#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "raymath.h"

typedef struct Inputs {
	int up[2];
	int left[2];
	int right[2];
	int down[2];
	int enter[2];
} Inputs;

typedef struct Data {

	// this is for my settings menu to be able to access it all in one parameter

	Color healthbarColour;
	float musicVolume;
	float menuVolume;
	float gameVolume;
	unsigned short enemyDifficulty;
	/*
	Key:
	0 = easy 
	1 = normal
	2 = hard
	3 = extreme
	4 = custom
	*/
} Data;


typedef struct FontData {
	const Font *FONT;
	float size;
	float spacing;
} FontData;


// easier to do this than write the if statement every time
// very easy to just sub these into the if statement if I thinks its better

bool getUp(Inputs hotkeys) {
	 return IsKeyPressed(hotkeys.up[0]) || IsKeyPressed(hotkeys.up[1]);
}

bool getDown(Inputs hotkeys) {
	return IsKeyPressed(hotkeys.down[0]) || IsKeyPressed(hotkeys.down[1]);
}

bool getLeft(Inputs hotkeys) {
	return IsKeyPressed(hotkeys.left[0]) || IsKeyPressed(hotkeys.left[1]);
}

bool getRight(Inputs hotkeys) {
	return IsKeyPressed(hotkeys.right[0]) || IsKeyPressed(hotkeys.right[1]);
}

bool getEnter(Inputs hotkeys) {
	return IsKeyPressed(hotkeys.enter[0]) || IsKeyPressed(hotkeys.enter[1]);
}


bool checkWindowSize(int *screenWidth, int *screenHeight) {
	// Persistent state across calls
	static bool isBorderless = false;
	static unsigned short windowedWidth = 0;
	static unsigned short windowedHeigth = 0;
	static Vector2 windowedPos = {0};

	int currentWidth = GetScreenWidth();
	int currentHeight = GetScreenHeight();

	// Toggle borderless fullscreen on F11
	if (IsKeyPressed(KEY_F11)) {
		if (!isBorderless) {
			// Save current windowed settings
			windowedPos = GetWindowPosition();
			windowedWidth = currentWidth;
			windowedHeigth = currentHeight;

			int monitor = GetCurrentMonitor();
			SetWindowState(FLAG_WINDOW_UNDECORATED);
			SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
			SetWindowPosition(GetMonitorPosition(monitor).x, GetMonitorPosition(monitor).y);

			isBorderless = true;
		} 
		else {
			// Exit borderless fullscreen
			ClearWindowState(FLAG_WINDOW_UNDECORATED);
			SetWindowSize(windowedWidth, windowedHeigth);

			// Returns the window to its origonal pos
			SetWindowPosition((int)windowedPos.x, (int)windowedPos.y);

			isBorderless = false;
		}
	}

	// Detect and update screen size changes


	if (currentWidth != *screenWidth || currentHeight != *screenHeight) {
		*screenWidth = currentWidth;
		*screenHeight = currentHeight;
		return true;
	}
	return false;
}


Vector2 centreText(Vector2 aimedPos, FontData font, const char *text) {
	// Returns the position of the top left corner to align the center of the text with the given position
	Vector2 dimentions = MeasureTextEx(*font.FONT, text, font.size, font.spacing);
	return Vector2Subtract(aimedPos, Vector2Scale(dimentions, 0.5f));
}

Vector2 ScaleVector2(Vector2 v, float scaleX, float scaleY) {
	return (Vector2){v.x * scaleX, v.y * scaleY};
}

Vector2 ValuePos(Vector2 lablePos, FontData font, const char *lableText) {
	// Calculates the position of a value that goes infront of a label in the screen e.g. volume value
	Vector2 lableDimentions = MeasureTextEx(*font.FONT, lableText, font.size, font.spacing);
	return (Vector2){lablePos.x + lableDimentions.x + 10, lablePos.y};
}

Vector2 PointerValuePos(Vector2 lablePos, FontData font, const char *lableText, int pointerWidth) {
	// Calculates the same as valuePos but with space for a pointer in it
	Vector2 lableDimentions = MeasureTextEx(*font.FONT, lableText, font.size, font.spacing);
	return (Vector2){lablePos.x + lableDimentions.x + pointerWidth + 10, lablePos.y};
}


void VolumeToString(char *text, float volume) {
	int percent = (int)(volume * 100.0f + 0.5f);
	sprintf(text, "%d", percent);
}

void KeyToString(char *text, int key) {
    switch (key) {
        case KEY_DOWN:
            strcpy(text, "Down");
            break;
        case KEY_UP:
            strcpy(text, "Up");
            break;
        case KEY_LEFT:
            strcpy(text, "Left");
            break;
        case KEY_RIGHT:
            strcpy(text, "Right");
            break;
        case KEY_SPACE:
            strcpy(text, "Space");
            break;
        case KEY_ENTER:
            strcpy(text, "Enter");
            break;
        default:
            // Printable ASCII fallback
            if (key >= 32 && key <= 126) {
                text[0] = (char)key;
                text[1] = '\0';
            } else {
                strcpy(text, "?");
            }
            break;
    }
}







void startMenu(const int FRAMERATE, unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs hotkeys) {
	
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
				case 0: *gameState = 3; break; // Start game
				case 1: *gameState = 2; break; // Settings
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



void settingsMenu(const int FRAMERATE, unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs *hotkeys, Data *info){
	
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
	const char titeText[] = "Settings";
	
	const char hotkeyText[] = "Hotkeys";
	const char healthText[] = "Health Colour";
	const char musicText[] = "Music Volume:";
	const char gameText[] = "Game Sound Volume:";
	const char menuText[] = "Menu Volume:";
	const char enemyText[] = "Enemy Difficulty";
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

	// Volumes
	char musicValueText[5];
	char gameValueText[5];
	char menuValueText[5];

	VolumeToString(musicValueText, info->musicVolume);
	VolumeToString(gameValueText, info->gameVolume);
	VolumeToString(menuValueText, info->menuVolume);
	






	// Base Positions
	int leftMargin = pointer.width + 20;
	int baseButtonSpacing = 20;

	// Persistant images
	Vector2 baseTitlePos = {40, 40};
	Vector2 baseHotkeyPos = {leftMargin, baseTitlePos.y + baseTitleSize + 40};
	Vector2 baseHealthPos = {leftMargin, baseHotkeyPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseMusicPos = {leftMargin, baseHealthPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseGamePos = {leftMargin, baseMusicPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseMenuPos = {leftMargin, baseGamePos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseEnemyPos = {leftMargin, baseMenuPos.y + baseButtonSize + baseButtonSpacing};
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

	// Volume positions
	
	Vector2 musicValuePos = ValuePos(baseMusicPos, doomFont, musicText);
	Vector2 gameValuePos = ValuePos(baseGamePos, doomFont, gameText);
	Vector2 menuValuePos = ValuePos(baseMenuPos, doomFont, menuText);





	// Dynamic Positions

	Vector2 titlePos = baseTitlePos;
	Vector2 hotkeyPos = baseHotkeyPos;
	Vector2 healthPos = baseHealthPos;
	Vector2 musicPos = baseMusicPos;
	Vector2 gamePos = baseGamePos;
	Vector2 menuPos = baseMenuPos;
	Vector2 enemyPos = baseEnemyPos;
	Vector2 backPos = baseBackPos;

	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;
	int buttonSpacing = baseButtonSpacing;

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
	&menuPos,
	&enemyPos,
	&backPos
	};

	
	// Hotkey pointer

	Vector2 *hotkeyPointerPositions[][3] = {
		{&hotkeyPos, NULL, NULL},
		{&upPos, &upValuePos1, &upValuePos2},
		{&leftPos, &leftValuePos1, &leftValuePos2},
		{&rightPos, &rightValuePos1, &rightValuePos2},
		{&downPos, &downValuePos1, &downValuePos2},
		{&enterPos, &enterValuePos1, &enterValuePos2}
	};



	// Position arrays

	Vector2 basePosArray[] = {
		baseTitlePos,
		baseHotkeyPos,
		baseHealthPos,
		baseMusicPos,
		baseGamePos,
		baseMenuPos,
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
		&menuPos,
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

	unsigned short hoveredButton = 0;
	unsigned short selectedButton = 9;

	/*
	Key:
		hotkey = 0
		health = 1
		music = 2
		game = 3
		menu = 4
		enemy = 5
		back = 6

		None = 9
		when no button is selected it shows 9.
	*/

	unsigned short hotkeyHoveredButton = 0;
	unsigned short hotkeySelectedButton = 9;
	
	/*
	Key:
		hotkey = 0
		up = 1
		left = 2
		right = 3
		down = 4
		enter = 5

		None = 9
	*/

	unsigned short hotkeyValueHoveredButton = 0;
	unsigned short hotkeyValueSelectedButton = 9;

	/*
	Key:
		label = 0
		value1 = 1
		value2 = 2
	*/
	

	while(*gameState == 2){
		if (WindowShouldClose()) *gameState = -1;
		if(checkWindowSize(&curWidth, &curHeight)){

			float scaleX = (float) curWidth / (float) SCREENWIDTH;
			float scaleY = (float) curHeight / (float) SCREENHEIGHT;

			buttonSize = baseButtonSize * scaleY;
			titleSize = baseTitleSize * scaleY;
			spacing = baseSpacing * scaleY;
			buttonSpacing = baseButtonSpacing * scaleY;

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


			// Volumes
			musicValuePos = ValuePos(musicPos, doomFont, musicText);
			gameValuePos = ValuePos(gamePos, doomFont, gameText);
			menuValuePos = ValuePos(menuPos, doomFont, menuText);
		}

		switch (selectedButton)
		{
		case 9:
			pointerPos = (Vector2){initialPointerPositions[hoveredButton]->x - pointerOffset, initialPointerPositions[hoveredButton]->y};

			if (getDown(*hotkeys) && hoveredButton != 6) hoveredButton++;
			if (getUp(*hotkeys) && hoveredButton != 0) hoveredButton--;

			if (getEnter(*hotkeys)) selectedButton = hoveredButton;
			break;

		case 0:


			if (hotkeySelectedButton == 0) {
				hotkeySelectedButton = 9;
				selectedButton = 9;
			}

			else if (hotkeySelectedButton == 9) {
				pointerPos = (Vector2){hotkeyPointerPositions[hotkeyHoveredButton][0]->x - pointerOffset, 
									hotkeyPointerPositions[hotkeyHoveredButton][0]->y};

				if (getLeft(*hotkeys)) {
					hotkeyHoveredButton = 0;
				}

				if (getDown(*hotkeys)) {
				if (hotkeyHoveredButton < 5 && hotkeyHoveredButton != 0)
					hotkeyHoveredButton++;
				}

				if (getUp(*hotkeys)) {
					if (hotkeyHoveredButton > 1)
						hotkeyHoveredButton--;
				}

				if (getRight(*hotkeys)) {
					if (hotkeyHoveredButton == 0)
						hotkeyHoveredButton = 1;
				}

				if (getEnter(*hotkeys)) hotkeySelectedButton = hotkeyHoveredButton;
			}
	
			else {

				pointerPos = (Vector2) {hotkeyPointerPositions[hotkeySelectedButton][hotkeyValueHoveredButton]->x - pointerOffset,
									hotkeyPointerPositions[hotkeySelectedButton][hotkeyValueHoveredButton]->y};

				if(getLeft(*hotkeys)) {
					if(hotkeyValueHoveredButton > 0) {
						hotkeyValueHoveredButton--;
					}
				}

				if(getRight(*hotkeys)) {
					if(hotkeyValueHoveredButton < 2) {
						hotkeyValueHoveredButton++;
					}
				}

				if(getEnter(*hotkeys)) {
					hotkeyValueSelectedButton = hotkeyValueHoveredButton;
				}

				if(hotkeyValueSelectedButton == 0) {
					hotkeyValueSelectedButton = 9;
					hotkeySelectedButton = 9;
				}
				else {
					
				}
			}

		case 1:



			break;    

		case 6:
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
		DrawTextEx(DOOM, titeText, titlePos, titleSize, spacing, RED);

		// Draw setting options
		DrawTextEx(DOOM, hotkeyText, hotkeyPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, healthText, healthPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, musicText, musicPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, gameText, gamePos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, menuText, menuPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, enemyText, enemyPos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, backText, backPos, buttonSize, spacing, RED);

		// Draw values for scrollable settings
		DrawTextEx(DOOM, musicValueText, musicValuePos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, gameValueText, gameValuePos, buttonSize, spacing, RED);
		DrawTextEx(DOOM, menuValueText, menuValuePos, buttonSize, spacing, RED);


	switch (selectedButton)
		{
		case 0:
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

			break;

		case 1:

			break;    
		default:
			break;
		}
				


	EndDrawing();
	}
	UnloadTexture(bg);
	UnloadTexture(pointer);
	UnloadFont(DOOM);
}




int main() {
	const int SCREENWIDTH = 1600;
	const int SCREENHEIGHT = 900;
	const int FRAMERATE = 60;

	InitWindow((int) SCREENWIDTH, (int) SCREENHEIGHT, "Racer");
	SetWindowState(FLAG_WINDOW_RESIZABLE); // Draging the corner will resize the window

	SetTargetFPS(FRAMERATE); // Limit to 60 frames per second
	SetExitKey(KEY_NULL); // Stops pressing esc closing the window

	//Setting default input values
	Inputs hotkeys = {
		.up = {KEY_W, KEY_UP},
		.down = {KEY_S, KEY_DOWN},
		.left = {KEY_A, KEY_LEFT},
		.right = {KEY_D, KEY_RIGHT},
		.enter = {KEY_ENTER, KEY_SPACE}
	};

	//Setting default data values
	Data info = {
		.enemyDifficulty = 1,
		.gameVolume = 0.5f,
		.menuVolume = 0.5f,
		.musicVolume = 0.5f,
		.healthbarColour = RED
	};

	// GameState handling

	unsigned short gameState = 0;
	while (gameState >= 0 && gameState <= 3) {
		switch (gameState) {
			case 0:
				startMenu(FRAMERATE, &gameState, SCREENWIDTH, SCREENHEIGHT, hotkeys);
				break;

			case 1:
				// Car customization
				break;

			case 2:
				settingsMenu(FRAMERATE, &gameState, SCREENWIDTH, SCREENHEIGHT, &hotkeys, &info);

			case 3:
				//gameplay
				break;

			case 4:
				// loss
			
			default:
				break;
		}
	}


	CloseWindow(); // Clean up
	return 0;
}