#ifndef RENDER_H
#define RENDER_H

#include "raylib.h"
#include "util.h"
#include <stdint.h>

void calc3DPerspective(
	Vector2 playerPos,
	Vector2 playerDir,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	const int CameraDistance,
	uint8_t *map,
	Image *tiles,
	Color *framePixels
);



#endif