#ifndef UTIL_H
#define UTIL_H

#include "raylib.h"

/* -------- Types -------- */

typedef struct Inputs {
    int up[2];
    int left[2];
    int down[2];
    int right[2];
    int enter[2];
} Inputs;

typedef struct Data {
    Color healthbarColour;
    float musicVolume;
    float menuVolume;
    float gameVolume;
    unsigned short enemyDifficulty;
} Data;

typedef struct FontData {
    const Font *FONT;
    float size;
    float spacing;
} FontData;

/* -------- Input helpers -------- */

bool getUp(Inputs hotkeys);
bool getDown(Inputs hotkeys);
bool getLeft(Inputs hotkeys);
bool getRight(Inputs hotkeys);
bool getEnter(Inputs hotkeys);

bool getUpHeld(Inputs hotkeys);
bool getDownHeld(Inputs hotkeys);
bool getLeftHeld(Inputs hotkeys);
bool getRightHeld(Inputs hotkeys);


/* -------- Window helpers -------- */

bool checkWindowSize(int *screenWidth, int *screenHeight);

/* -------- Math / layout -------- */

Vector2 centreText(Vector2 aimedPos, FontData font, const char *text);
Vector2 ScaleVector2(Vector2 v, float scaleX, float scaleY);
Vector2 ValuePos(Vector2 labelPos, FontData font, const char *labelText);
Vector2 PointerValuePos(Vector2 labelPos, FontData font, const char *labelText, int pointerWidth);

/* -------- Text helpers -------- */

void VolumeToString(char *text, float volume);
void KeyToString(char *text, int key);
void ColourToString(char *text, Color colour);
void DifficultyToString(char *text, unsigned short difficulty);
Color DifficultyToColour(unsigned short enemyDifficulty);

#endif
