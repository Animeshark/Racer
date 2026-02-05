#include "render.h"
#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <string.h>




float getParallelOffset(int row, const int SCREENHEIGHT, const int CameraDistance){
	return CameraDistance * (SCREENHEIGHT - row) / (row - SCREENHEIGHT / 2);
}


Vector2 getMapPixelPos(Vector2 playerPos, Vector2 playerDir, float parallelOffset, float perpendicularOffset){
	return Vector2Add(Vector2Add(playerPos, Vector2Scale(playerDir, parallelOffset)),
			Vector2Scale((Vector2) {-playerDir.y, playerDir.x}, perpendicularOffset));
}

static inline void writeMapPixel(
	Color *frame,
	int screenWidth,
	int x,
	int y,
	Vector2 mapPixelPos,
	float mapScale,
	int MAPSIZE,
	uint8_t *map,
	Image *tiles
){
	// Convert to map space
	mapPixelPos = Vector2Scale(mapPixelPos, 1.0f / mapScale);

	int mapx = (int)mapPixelPos.x;
	int mapy = (int)mapPixelPos.y;

	if (mapx < 0 || mapy < 0 || mapx >= MAPSIZE || mapy >= MAPSIZE) {
		frame[y * screenWidth + x] = WATER_COLOUR;
		return;
	}

	uint8_t tileId = map[mapy * MAPSIZE + mapx];
	if (tileId == 0) return;

	Image *tile = &tiles[tileId];

	int texturex = (int)((mapPixelPos.x - mapx) * tile->width);
	int texturey = (int)((mapPixelPos.y - mapy) * tile->height);

	// Clamp for safety
	if (texturex < 0) texturex = 0;
	if (texturey < 0) texturey = 0;
	if (texturex >= tile->width)  texturex = tile->width - 1;
	if (texturey >= tile->height) texturey = tile->height - 1;

	frame[y * screenWidth + x] = GetImageColor(*tile, texturex, texturey);
}

void draw3DPerspective(
	Vector2 playerPos,
	Vector2 playerDir,
	const int SCREENWIDTH,
	const int SCREENHEIGHT,
	const unsigned short MAPSIZE,
	const int CameraDistance,
	uint8_t *map,
	Image *tiles,
	Color *framePixels) {
	//screen dimentions are for the current frame. Constant as they are not changing in this frame
	float parallelOffset;
	float perpendicularOffset;
	float mapScale = SCREENWIDTH * 50.0f / MAPSIZE;
	
	Vector2 scaledPlayerPos = Vector2Scale(playerPos, mapScale);

	memset(framePixels, 0, SCREENWIDTH * SCREENHEIGHT * sizeof(Color));

	// Precalculated constants for efficientcy

	float a;
	float b;

	for (int row = SCREENHEIGHT/2 + 1; row < SCREENHEIGHT; row++) {
		// Calculated ofsets are multipliers parallel and perpendicular to player direction
		parallelOffset = getParallelOffset(row, SCREENHEIGHT, CameraDistance);

		// Precalculated horizontal constants for efficientcy
		a = 1.0f + parallelOffset / CameraDistance;
		b = SCREENWIDTH / 2.0f + (parallelOffset * SCREENWIDTH) / (2.0f * CameraDistance);

		for (int column = 0; column < SCREENWIDTH; column++) {
			perpendicularOffset = a * column - b;

			Vector2 mapPos = getMapPixelPos(
				scaledPlayerPos,
				playerDir,
				parallelOffset,
				perpendicularOffset
			);

			writeMapPixel(
				framePixels,
				SCREENWIDTH,
				column,
				row,
				mapPos,
				mapScale,
				MAPSIZE,
				map,
				tiles
			);
		}
	}
}