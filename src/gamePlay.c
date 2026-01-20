#include "raylib.h"
#include "raymath.h"
#include "gamePlay.h"
#include "util.h"
#include <stdio.h>

typedef struct Car{
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 direction;
	float drivingPower;
	float brakingMagnitude;
	float turningMagnitude;
	float resistanceCoefficient;
	float mass;
	float maxHealth;
	float health;
} Car;




void DrawPlayerMiniMap(Vector2 dir, Vector2 playerPos, float mapScale) {
	DrawCircleV(playerPos, mapScale * 1.0f, RED);
	DrawLineEx(playerPos,
		Vector2Add(playerPos, Vector2Scale(dir, mapScale * 2.0f)),
		mapScale * 0.4f, RED);
}

Vector2 getDrivingForce(Car *player, Inputs hotkeys) {
	if(getUpHeld(hotkeys)) {
		float forwardVelocity = Vector2DotProduct(player->direction, player->velocity);
		forwardVelocity = fmaxf(forwardVelocity, MIN_SPEED);
		return Vector2Scale(player->direction, player->drivingPower / forwardVelocity);
	}
	return (Vector2){0, 0};
}

float getTurningAngle(Inputs hotkeys, float turning) {
	// clockwise is positive
	if(getLeftHeld(hotkeys)) return -turning;
	if(getRightHeld(hotkeys)) return turning;
	return 0.0f;
}

Vector2 getSidewaysResistance(Car *player){
	//Perpendicular direction
	Vector2 sidewaysDirection = (Vector2) {player->direction.y, -player->direction.x};
	// Velocity component perpendicular to the direction of the player
	Vector2 sidewaysVelocity = Vector2Scale(sidewaysDirection, Vector2DotProduct(sidewaysDirection, player->velocity));
	return Vector2Scale(sidewaysVelocity, -player->resistanceCoefficient);
}

Vector2 getBrakeForce(Car *player){
	if (Vector2LengthSqr(player->velocity) > MIN_SPEED * MIN_SPEED) {
		return Vector2Scale(Vector2Normalize(player->velocity), -player->brakingMagnitude);
	}
	return (Vector2){0, 0};
}

void movePlayer(Car *player, Inputs hotkeys) {

	player->direction = Vector2Rotate(player->direction, getTurningAngle(hotkeys, player->turningMagnitude));
	player->direction = Vector2Normalize(player->direction);


	bool isBraking = getDownHeld(hotkeys);

	Vector2 drivingForce = getDrivingForce(player, hotkeys);

	Vector2 resistanceForce = Vector2Scale(player->velocity, -player->resistanceCoefficient);
	Vector2 sidewaysResistance = getSidewaysResistance(player);
	resistanceForce = Vector2Add(resistanceForce, sidewaysResistance);

	Vector2 brakeForce = {0, 0};
	brakeForce = getBrakeForce(player);

	if (isBraking) {
		if(Vector2LengthSqr(drivingForce) == 0) {
			brakeForce = getBrakeForce(player);
			player->acceleration = Vector2Scale(Vector2Add(brakeForce, resistanceForce), 1.0f / player->mass);
		}
		else {
			player->health -= 0.1f;
			player->acceleration = Vector2Scale(resistanceForce, 1.0f / player->mass);
		}
	}
	else {
		player->acceleration = Vector2Scale(Vector2Add(drivingForce, resistanceForce), 1.0f / player->mass);
	}
	
	player->velocity = Vector2Add(player->velocity, player->acceleration);

	if (Vector2LengthSqr(player->velocity) < MIN_SPEED * MIN_SPEED) {
		player->velocity = (Vector2){0, 0};
	}

	player->position = Vector2Add(player->position, player->velocity);
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
	float baseMiniMapScreenScale = 5.0f;
	Vector2 BaseMiniMapPos = {5, 5};

	float miniMapScreenScale = baseMiniMapScreenScale;
	Vector2 miniMapPos = BaseMiniMapPos;

	// Player Rendering
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

	while (*gameState == 2)
	{
		if (WindowShouldClose()) *gameState = -1;

		if(checkWindowSize(&currentScreenWidth, &currentScreenHeight)){
			float scaleX = (float) currentScreenWidth / (float) SCREENWIDTH;
			float scaleY = (float) currentScreenHeight / (float) SCREENHEIGHT;

			miniMapScreenScale = baseMiniMapScreenScale * scaleY;

			for (int i = 0; i < flatScreenElementsLen; i++){
				*flatScreenElementsDynamicPos[i] = Vector2Scale(flatScreenElementsBasePos[i], scaleY);
			}
		}

		movePlayer(&player, hotkeys);

		calcMiniMapPositions(miniMapPos, miniMapScreenScale, &miniMapPlayerPos, player.position);
		
		BeginDrawing();

			ClearBackground(BLACK);

			// Drawing the map
			// Raw map texture
			DrawTextureEx(miniMapImage, miniMapPos, 0, miniMapScreenScale, WHITE);

			// Drawing the miniMap player[]
			// Make it update on a lower framerate after 3-D image generation
			DrawPlayerMiniMap(player.direction, miniMapPlayerPos, miniMapScreenScale);

			



		EndDrawing();
	}
}

