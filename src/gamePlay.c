#include "raylib.h"
#include "raymath.h"
#include "gamePlay.h"
#include "util.h"
#include "physics.h"
#include "render.h"
#include "pauseMenu.h"
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



void gameLoop(unsigned short *gameState, const int SCREENWIDTH, const int SCREENHEIGHT, Inputs hotkeys, Data *info) {

	Car player = {
		.brakingMagnitude = 2.0f,
		.direction = {0, -1},
		.drivingPower = 0.25f,
		.health = 100,
		.maxHealth = 100,
		.mass = 400,
		.position = {12, 60},
		.resistanceCoefficient = 0.8f,
		.frictionCoefficient = 0.6f,
		.turningMagnitude = PI/(3.14f * FRAMERATE),
		.acceleration = {0, 0},
		.velocity = {0, 0}
	};

	int currentScreenHeight = SCREENHEIGHT;
	int currentScreenWidth = SCREENWIDTH;

	
	Font DOOM = LoadFontEx("Assets/font/Amazdoomleft-epw3.ttf", 128, 0, 0);

	unsigned short gameStateTwo = 0;

	// Initialising start timer
	unsigned int timerLenght = 6 * FRAMERATE;

	// Timer values
	bool needTimer = true;
	char timerText[8];
	sprintf(timerText, "%d", timerLenght / FRAMERATE);

	float timerSize = 80;
	float timerSpacing = 6;

	FontData doomFont = {
		.FONT = &DOOM,
		.size = timerSize,
		.spacing = timerSpacing
	};

	Vector2 baseTimerPos = centreText((Vector2){currentScreenWidth/2, currentScreenHeight/2}, doomFont, timerText);
	Vector2 timerPos = baseTimerPos;


	// Mini Map Rendering
	Texture2D miniMapImage = LoadTexture("Assets/Track/FullTrack.png");
	// Set baseMiniMapScreenScale lower after 3-D rendering
	// Multi to the miniMap screenSize
	float baseMiniMapScreenScale = 2.0f;
	Vector2 baseMiniMapPos = {5, 5};

	float miniMapScreenScale = baseMiniMapScreenScale;
	Vector2 miniMapPos = baseMiniMapPos;

	// Mini Map Player Rendering
	Vector2 baseMiniMapPlayerPos = Vector2Add(baseMiniMapPos, Vector2Scale(player.position, baseMiniMapScreenScale));
	Vector2 miniMapPlayerPos = baseMiniMapPlayerPos;

	Vector2 flatScreenElementsBasePos[] = {
		baseMiniMapPos,
		baseTimerPos
	};

	Vector2 *flatScreenElementsDynamicPos[] = {
		&miniMapPos,
		&timerPos
	};

	int flatScreenElementsLen = sizeof(flatScreenElementsBasePos)/sizeof(flatScreenElementsBasePos[0]);


	// 3D rendering
	const int cameraDistance = (int)FovToDistance(info->fov, (float) SCREENWIDTH);

	// 1. Check the file opened successfully
	FILE *track = fopen("Assets/Track/encodedTrack.ck", "rb");
	if (track == NULL) {
		printf("ERROR: Failed to open track file!\n");
		fflush(stdout);
		*gameState = -1;
		return;
	}

	// 2. Move map off the stack — make it static or malloc it
	static uint8_t map[MAP_SIZE * MAP_SIZE];
	fread(map, 1, MAP_SIZE * MAP_SIZE, track);
	fclose(track);  // you're also never closing this file

	Image tiles[TILE_COUNT];

	tiles[0] = LoadImage("Assets/Track/tiles/Black.png");
	tiles[1] = LoadImage("Assets/Track/tiles/Grass.png");
	tiles[2] = LoadImage("Assets/Track/tiles/Track.png");
	tiles[3] = LoadImage("Assets/Track/tiles/HairPin.png");
	tiles[4] = LoadImage("Assets/Track/tiles/Finish.png"); // Placeholder for the finish line

	// Frame buffer 
	Image frame = GenImageColor(SCREENWIDTH, SCREENHEIGHT, BLACK);
	Texture2D frameTex = LoadTextureFromImage(frame);
	Color *framePixels = (Color *)frame.data;

	// Flat gameplay Rendering

	// Sky 
	Texture2D sky = LoadTexture("Assets/Game/Sky.png");
	float baseSkyScale = SCREENHEIGHT/120;
	float skyScale = baseSkyScale;

	// Water
	Texture2D water = LoadTexture("Assets/Game/Water.png");
	// same scale as the same dimentions


	while (*gameState == 2)
	{
		if (WindowShouldClose()) *gameState = -1;

		if(checkWindowSize(&currentScreenWidth, &currentScreenHeight)){
			float scaleX = (float) currentScreenWidth / (float) SCREENWIDTH;
			float scaleY = (float) currentScreenHeight / (float) SCREENHEIGHT;


			miniMapScreenScale = baseMiniMapScreenScale * scaleY;
			skyScale = baseSkyScale * scaleX;

			for (int i = 0; i <= flatScreenElementsLen - 1; i++){
				*flatScreenElementsDynamicPos[i] = Vector2Scale(flatScreenElementsBasePos[i], scaleY);
			}
		}

		if (getEnter(hotkeys)) gameStateTwo = 1;

		if (gameStateTwo == 0){
			if (needTimer) {
				timerLenght--;
				sprintf(timerText, "%d", timerLenght / FRAMERATE);
				if (timerLenght / FRAMERATE <= 0) needTimer = false;
				
			} else {
				movePlayer(&player, hotkeys, map);
				calcMiniMapPositions(miniMapPos, miniMapScreenScale, &miniMapPlayerPos, player.position);
			}
			
			calc3DPerspective(player.position, player.direction, 
				SCREENWIDTH, SCREENHEIGHT, cameraDistance, map, tiles, framePixels);


			BeginDrawing();

				ClearBackground(BLACK);

				// sky
				DrawTextureEx(sky, (Vector2){0,0}, 0, skyScale, WHITE);
				// water
				DrawTextureEx(water, (Vector2){0,currentScreenHeight/2}, 0, skyScale, WHITE);
				// Drawing the map
				// Raw map texture
				DrawTextureEx(miniMapImage, miniMapPos, 0, miniMapScreenScale, WHITE);

				// Drawing the miniMap player
				// Make it update on a lower framerate after 3-D image generation
				DrawPlayerMiniMap(player.direction, miniMapPlayerPos, miniMapScreenScale);

				UpdateTexture(frameTex, frame.data);
				DrawTexturePro(
					frameTex,
					(Rectangle) {0, 0, SCREENWIDTH, SCREENHEIGHT},
					(Rectangle) {0, 0, currentScreenWidth, currentScreenHeight},
					(Vector2) {0,0},
					0,
					WHITE);

				if (needTimer) DrawTextEx(DOOM, timerText, timerPos, timerSize, timerSpacing, WHITE);

				DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, GREEN);
			EndDrawing();
		}
		else if(gameStateTwo == 1){
			// Pause menu
			pauseMenu(gameState, &gameStateTwo, SCREENWIDTH, SCREENHEIGHT, hotkeys, info, &DOOM);
			needTimer = true;
			timerLenght = 4 * FRAMERATE;
		};
	}
	UnloadTexture(frameTex);
	UnloadImage(frame);
	UnloadTexture(miniMapImage);
	UnloadTexture(sky);
	UnloadTexture(water);
	for(int i = 0; i < TILE_COUNT; i++) UnloadImage(tiles[i]);
 }

