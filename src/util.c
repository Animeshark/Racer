#include "util.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>


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

void ColourToString(char *text, Color colour)
{
	if (ColorIsEqual(colour, RED))
		strcpy(text, "RED");
	else if (ColorIsEqual(colour, GOLD))
		strcpy(text, "GOLD");
	else if (ColorIsEqual(colour, PINK))
		strcpy(text, "PINK");
	else if (ColorIsEqual(colour, BLUE))
		strcpy(text, "BLUE");
	else if (ColorIsEqual(colour, PURPLE))
		strcpy(text, "PURPLE");
	else
		strcpy(text, "UNKNOWN");
}

void DifficultyToString(char *text, unsigned short difficulty) {
	switch (difficulty)
	{
	case 0:
		strcpy(text, "Easy");
		break;
	case 1:
		strcpy(text, "Normal");
		break;
	case 2:
		strcpy(text, "Hard");
		break;
	case 3:
		strcpy(text, "Extreme");
		break;
	
	// space for custom
	default:
		break;
	}
}

Color DifficultyToColour(unsigned short enemyDifficulty){
	switch (enemyDifficulty)
	{
	case 0:
		return GREEN;
	case 1:
		return YELLOW;
	case 2:
		return RED;
	case 3:
		return MAROON;
	default:
		return WHITE;
	}
}