#include "raylib.h"
#include "raymath.h"
#include "gamePlay.h"
#include "util.h"

typedef struct Car
{
	Vector2 coordinates;
	Vector2 acceleration;
} Car;


void DrawPlayerMiniMap(Vector2 dir, Vector2 playerPos, float mapScale) {
	DrawCircleV(playerPos, mapScale * 1.0f, RED);
	DrawLineEx(playerPos,
		Vector2Add(playerPos, Vector2Scale(dir, mapScale * 2.0f)),
		mapScale * 0.4f, RED);
};


void gameLoop(unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs *hotkeys, Data *info, const int FRAMERATE) {

	int currentScreenHeight = SCREENHEIGHT;
	int currentScreenWidth = SCREENWIDTH;

	const unsigned short MAPSIZE = 128;

	Car player = {
		.acceleration = {0, -1},
		.coordinates = {12, 60}
	};


	Texture2D miniMapImage = LoadTexture("Assets/Track/FullTrack.png");
	// Set it lower after 3-D rendering
	float miniMapScreenScale = 5.0f;
	Vector2 miniMapPos = {5, 5};

	Vector2 miniMapPlayerPos = Vector2Add(miniMapPos, Vector2Scale(player.coordinates, miniMapScreenScale));



	while (*gameState == 2)
	{
		if (WindowShouldClose()) *gameState = -1;

		checkWindowSize(&currentScreenWidth, &currentScreenHeight);

		BeginDrawing();

			ClearBackground(BLACK);

			// Drawing the map
			// Raw map texture
			DrawTextureEx(miniMapImage, miniMapPos, 0, miniMapScreenScale, WHITE);

			// Drawing the miniMap player
			// Make it update on less then every frame after 3-D image generation
			DrawPlayerMiniMap(Vector2Normalize(player.acceleration), miniMapPlayerPos, miniMapScreenScale);

			



		EndDrawing();
	}
	
}

