#include "raylib.h"
#include "raymath.h"
#include "gamePlay.h"
#include "util.h"
#include "physics.h"
#include "render.h"
#include <stdio.h>
#include <stdint.h>





void DrawPlayerMiniMap(Vector2 dir, Vector2 playerPos, float mapScale) {
	DrawCircleV(playerPos, mapScale * 1.0f, RED);
	DrawLineEx(playerPos,
		Vector2Add(playerPos, Vector2Scale(dir, mapScale * 2.0f)),
		mapScale * 0.4f, RED);
}

void calcMiniMapPositions(Vector2 miniMapPos, float miniMapScreenScale, Vector2 *miniMapPlayerPos, Vector2 playerPos) {
	*miniMapPlayerPos = Vector2Add(miniMapPos, Vector2Scale(playerPos, miniMapScreenScale));
}



void gameLoop(unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs hotkeys, Data *info, const int FRAMERATE) {

	int currentScreenHeight = SCREENHEIGHT;
	int currentScreenWidth = SCREENWIDTH;

	const unsigned short MAPSIZE = 128;

	Car player = {
		.brakingMagnitude = 0.1f,
		.direction = {0, -1},
		.drivingPower = 0.01f,
		.health = 100,
		.maxHealth = 100,
		.mass = 500,
		.position = {12, 60},
		.resistanceCoefficient = 0.8f,
		.turningMagnitude = PI/(8 * FRAMERATE),
		.acceleration = {0, 0},
		.velocity = {0, 0}
	};


	// Mini Map Rendering
	Texture2D miniMapImage = LoadTexture("Assets/Track/FullTrack.png");
	// Set baseMiniMapScreenScale lower after 3-D rendering
	// Multi to the miniMap screenSize
	float baseMiniMapScreenScale = 2.2f;
	Vector2 BaseMiniMapPos = {5, 5};

	float miniMapScreenScale = baseMiniMapScreenScale;
	Vector2 miniMapPos = BaseMiniMapPos;

	// Mini Map Player Rendering
	Vector2 baseMiniMapPlayerPos = Vector2Add(BaseMiniMapPos, Vector2Scale(player.position, baseMiniMapScreenScale));
	Vector2 miniMapPlayerPos = baseMiniMapPlayerPos;

	Vector2 flatScreenElementsBasePos[] = {
		miniMapPos,
		miniMapPlayerPos
	};

	Vector2 *flatScreenElementsDynamicPos[] = {
		&miniMapPos,
		&miniMapPlayerPos
	};
	
	int flatScreenElementsLen = sizeof(flatScreenElementsBasePos)/sizeof(flatScreenElementsBasePos[0]);


	// 3D rendering
	const int cameraDistance = (int)FovToDistance(info->fov, (float) SCREENWIDTH);

	FILE *track = fopen("Assets/Track/encodedTrack.ck", "rb");

	uint8_t map[MAPSIZE * MAPSIZE];
	fread(map, 1, MAPSIZE * MAPSIZE, track);

	Image tiles[TILE_COUNT];

	tiles[0] = LoadImage("Assets/Track/tiles/Water.png");
	tiles[1] = LoadImage("Assets/Track/tiles/Grass.png");
	tiles[2] = LoadImage("Assets/Track/tiles/Track.png");
	tiles[3] = LoadImage("Assets/Track/tiles/Red.png");
	tiles[4] = LoadImage("Assets/Track/tiles/White.png");
	tiles[5] = LoadImage("Assets/Track/tiles/Black.png"); // Placeholder for the finish line

	// Frame buffer 
	Image frame = GenImageColor(SCREENWIDTH, SCREENHEIGHT, BLACK);
	Texture2D frameTex = LoadTextureFromImage(frame);
	Color *framePixels = (Color *)frame.data;

	// Flat gameplay Rendering

	// Sky 
	Texture2D sky = LoadTexture("Assets/Game/Sky.png");
	float skyScale = SCREENWIDTH/320;

	// Water


	while (*gameState == 2)
	{
		if (WindowShouldClose()) *gameState = -1;

		if(checkWindowSize(&currentScreenWidth, &currentScreenHeight)){
			float scaleX = (float) currentScreenWidth / (float) SCREENWIDTH;
			float scaleY = (float) currentScreenHeight / (float) SCREENHEIGHT;

			miniMapScreenScale = baseMiniMapScreenScale * scaleY;
			skyScale *= scaleX;

			for (int i = 0; i < flatScreenElementsLen; i++){
				*flatScreenElementsDynamicPos[i] = Vector2Scale(flatScreenElementsBasePos[i], scaleY);
			}
		}

		movePlayer(&player, hotkeys);

		calcMiniMapPositions(miniMapPos, miniMapScreenScale, &miniMapPlayerPos, player.position);
		
		
		BeginDrawing();

			ClearBackground(BLACK);

			// sky
			DrawTextureEx(sky, (Vector2){0,0}, 0, skyScale, WHITE);

			// Drawing the map
			// Raw map texture
			DrawTextureEx(miniMapImage, miniMapPos, 0, miniMapScreenScale, WHITE);

			// Drawing the miniMap player[]
			// Make it update on a lower framerate after 3-D image generation
			DrawPlayerMiniMap(player.direction, miniMapPlayerPos, miniMapScreenScale);

			draw3DPerspective(player.position, player.direction, currentScreenWidth, currentScreenHeight, MAPSIZE, cameraDistance, map, tiles, framePixels);
			UpdateTexture(frameTex, frame.data);
			DrawTexture(frameTex, 0, 0, WHITE);



//			DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, GREEN);




		EndDrawing();
	}
}

