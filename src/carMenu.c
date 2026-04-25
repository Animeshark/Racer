#include "util.h"
#include "physics.h"

static inline Vector2 statLineOffset(Vector2 statPos, float buttonSize, float lineWidth) {
	return (Vector2) {statPos.x, statPos.y + lineWidth + buttonSize};
}



void drawStatLines(Vector2 statPos, float buttonSize, float statCurValue, float lineWidth, float lineLenght, float statMaxValue, float statMinValue){
	// Line width is used as a scale factor as it is an input that will be scaled with the screen resizing
	float length = statCurValue / (statMaxValue - statMinValue);
	length *= lineWidth * lineLenght / 20;

	Vector2 startPos = statLineOffset(statPos, buttonSize, lineWidth);
	Vector2 endPos = (Vector2) {startPos.x + length, startPos.y};

	Vector2 fullEndPos = (Vector2) {startPos.x + lineLenght, startPos.y};

	// Grey underline
	DrawLineEx(startPos, fullEndPos, lineWidth, GRAY);
	// Main line
	DrawLineEx(startPos, endPos, lineWidth, BLUE);
}

void carMenu(
	unsigned short *gameState,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	Inputs hotkeys,
	Data *info, 
	Car *player
) {
	
	//Initialising Textures
	const Texture2D bg = LoadTexture("Assets/Menu/Backgrounds/Rollers.png");
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

	float baseLineWidth = 10;
	float baseLineLenght = 200;



	// Text 
	const char titleText[] = "Garage";

	const char wheelText[] = "Wheels";
	const char bodyText[] = "Body";
	const char engineText[] = "Engine";
	const char brakesText[] = "Brakes";
	const char backText[] = "Back";

	const char statsText[] = "Stats";

	const char drivingText[] = "Driving Power";
	const char brakingText[] = "Braking Power";
	const char turningText[] = "Turning Power";
	const char healthText[] = "Max Health";
	const char massText[] = "Mass";

// ---- VALUE TEXT ----
	// Add char arrays here for any dynamic values e.g.:
	// char wheelValueText[16];
	// SomeToString(wheelValueText, player->wheels);

	// ---- BASE POSITIONS ----
	int leftMargin = pointer.width + 20;
	int baseButtonSpacing = 20;

	int rightMargin = SCREENWIDTH * 0.625 + 20;
	int baseStatSpacing = 40;

	Vector2 baseTitlePos = {40, 40};

	Vector2 baseWheelPos = {leftMargin, baseTitlePos.y + baseButtonSize + 40};
	Vector2 baseBodyPos = {leftMargin, baseWheelPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseEnginePos = {leftMargin, baseBodyPos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseBrakesPos = {leftMargin, baseEnginePos.y + baseButtonSize + baseButtonSpacing};
	Vector2 baseBackPos = {leftMargin, SCREENHEIGHT - baseButtonSize - 10};

	Vector2 baseStatsPos = {rightMargin - 20, 40};

	Vector2 baseDrivingPos = {rightMargin, baseStatsPos.y + baseButtonSize + baseStatSpacing};
	Vector2 baseBrakingPos = {rightMargin, baseDrivingPos.y + baseButtonSize + baseStatSpacing};
	Vector2 baseTurningPos = {rightMargin, baseBrakingPos.y + baseButtonSize + baseStatSpacing};
	Vector2 baseHealthPos = {rightMargin, baseTurningPos.y + baseButtonSize + baseStatSpacing};
	Vector2 baseMassPos = {rightMargin, baseHealthPos.y + baseButtonSize + baseStatSpacing};
	
	// Add value positions here e.g.:
	// Vector2 wheelValuePos = ValuePos(baseWheelPos, doomFont, wheelText);

	// ---- DYNAMIC POSITIONS (for resize support) ----
	Vector2 titlePos = baseTitlePos;

	Vector2 wheelPos = baseWheelPos;
	Vector2 bodyPos = baseBodyPos;
	Vector2 enginePos = baseEnginePos;
	Vector2 brakesPos = baseBrakesPos;
	Vector2 backPos = baseBackPos;

	Vector2 statsPos = baseStatsPos;

	Vector2 drivingPos = baseDrivingPos;
	Vector2 brakingPos = baseBrakingPos;
	Vector2 turningPos = baseTurningPos;
	Vector2 healthPos = baseHealthPos;
	Vector2 massPos = baseMassPos;

	int buttonSize = baseButtonSize;
	int titleSize = baseTitleSize;
	int spacing = baseSpacing;

	// ---- POSITION ARRAYS (for resize loop) ----
	Vector2 basePosArray[] = {
		baseTitlePos,
		baseWheelPos,
		baseBodyPos,
		baseEnginePos,
		baseBrakesPos,
		baseBackPos,
		baseStatsPos,
		baseDrivingPos,
		baseBrakingPos,
		baseTurningPos,
		baseHealthPos,
		baseMassPos
	};

	Vector2 *posArray[] = {
		&titlePos,
		&wheelPos,
		&bodyPos,
		&enginePos,
		&brakesPos,
		&backPos,
		&statsPos,
		&drivingPos,
		&brakingPos,
		&turningPos,
		&healthPos,
		&massPos
	};

	int posArrayLen = sizeof(basePosArray) / sizeof(basePosArray[0]);
	// ---- POINTER ----
	// Skip title (index 0) and stats (index 1) if they aren't selectable
	Vector2 *pointerPositions[] = {
		&wheelPos,
		&bodyPos,
		&enginePos,
		&brakesPos,
		&backPos
	};

	int pointerPositionsLen = sizeof(pointerPositions) / sizeof(pointerPositions[0]);
	int pointerOffset = 10 + pointer.width;
	Vector2 pointerPos;

	unsigned short hoveredPart = 0;
	unsigned short selectedPart = 0;
	
	int curWidth = SCREENWIDTH;
	int curHeight = SCREENHEIGHT;


	// Values
	float lineWidth = baseLineWidth;
	float lineLenght = baseLineLenght;

	float minDrivingPower = 0.01f;
	float maxDrivingPower = 0.02f;


	// ---- GAME LOOP ----
	while (*gameState == 3) {

		if (WindowShouldClose()) *gameState = -1;

		// Handle resize
		if(checkWindowSize(&curWidth, &curHeight)){

			float scaleX = (float) curWidth / (float) SCREENWIDTH;
			float scaleY = (float) curHeight / (float) SCREENHEIGHT;

			buttonSize = baseButtonSize * scaleY;
			titleSize = baseTitleSize * scaleY;
			spacing = baseSpacing * scaleY;
			lineWidth = baseLineWidth * scaleY;
			lineLenght = baseLineLenght * scaleX;

			doomFont.size = buttonSize;
			doomFont.spacing = spacing;

			for (int i = 0; i <= posArrayLen -1; i++) {
				*posArray[i] = ScaleVector2(basePosArray[i], scaleX, scaleY);
			}
		}

		// Input
		switch (selectedPart)
		{
		case 0:
			if (getDown(hotkeys) && hoveredPart != pointerPositionsLen - 1) hoveredPart++;
			if (getUp(hotkeys) && hoveredPart != 0) hoveredPart--;
			break;
		}

		// Update pointer pos
		pointerPos = *pointerPositions[hoveredPart];
		pointerPos.x -= pointerOffset;

		// Draw
		BeginDrawing();
			ClearBackground(BLACK);
			DrawTexturePro(
				bg, 
				(Rectangle){0, 0, bg.width, bg.height}, 
				(Rectangle){0, 0, curWidth, curHeight}, 
				(Vector2){0, 0}, 0, WHITE
			);

			// Title
			DrawTextEx(DOOM, titleText, titlePos, titleSize, spacing, RED);

			// Buttons
			DrawTextEx(DOOM, wheelText, wheelPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, bodyText, bodyPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, engineText, enginePos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, brakesText, brakesPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, backText, backPos, buttonSize, spacing, RED);


			// Stats Title
			DrawTextEx(DOOM, statsText, statsPos, titleSize, spacing, RED);

			// Stats 
			DrawTextEx(DOOM, drivingText, drivingPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, brakingText, brakingPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, turningText, turningPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, healthText, healthPos, buttonSize, spacing, RED);
			DrawTextEx(DOOM, massText, massPos, buttonSize, spacing, RED);

			// StatLines
			drawStatLines(drivingPos, buttonSize, player->drivingPower, lineWidth, lineLenght, maxDrivingPower, minDrivingPower);
			

			// Add value text draws here e.g.:
			// DrawTextEx(DOOM, wheelValueText, wheelValuePos, baseButtonSize, baseSpacing, WHITE);

			// Pointer
			DrawTexture(pointer, pointerPos.x, pointerPos.y, WHITE);

		EndDrawing();
	}

	// Unload
	UnloadTexture(bg);
	UnloadTexture(pointer);
	UnloadFont(DOOM);
}
