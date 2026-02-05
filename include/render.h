#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include <stdint.h>


#define WATER_COLOUR (Color){15, 29, 119, 255}

void draw3DPerspective(
	Vector2 playerPos,
	Vector2 playerDir,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	const unsigned short MAPSIZE,
	const int CameraDistance,
	uint8_t *map,
	Image *tiles,
	Color *framePixels
);



#endif