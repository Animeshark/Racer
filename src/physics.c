#include "physics.h"
#include "raylib.h"
#include "raymath.h"
#include "util.h"
#include <stdio.h>

static int roadDetection(Car *racer, uint8_t *map) {
	int carx = racer->position.x;
	int cary = racer->position.y;

	return map[MAP_SIZE * cary + carx];
}


static Vector2 getRoadResistance(Car *racer, uint8_t *map) {
	uint8_t tileCode = roadDetection(racer, map);

	switch (tileCode)
	{
	case 1:
		//Grass
		return Vector2Scale(racer->velocity, -racer->resistanceCoefficient * 10);
	case 2:
		//Road
		return Vector2Scale(racer->velocity, -racer->resistanceCoefficient);
	case 3:
		// hairPin
		return (Vector2){0, 0};
	default:
		return (Vector2){0, 0};
	}
}


static Vector2 getDrivingForce(Car *racer) {
	float forwardVelocity = Vector2DotProduct(racer->direction, racer->velocity);
	forwardVelocity = fmaxf(forwardVelocity, MIN_SPEED);
	return Vector2Scale(racer->direction, racer->drivingPower / forwardVelocity);
}

static float getTurningAngle(Inputs hotkeys, float turning) {
	// clockwise is positive
	if(getLeftHeld(hotkeys)) return -turning;
	if(getRightHeld(hotkeys)) return turning;
	return 0.0f;
}

static Vector2 getSidewaysResistance(Car *racer) {
	Vector2 sidewaysDirection = (Vector2){racer->direction.y, -racer->direction.x};
	float sidewaysVelocity = Vector2DotProduct(sidewaysDirection, racer->velocity);

	// Max friction force this frame
	float maxFriction = racer->frictionCoefficient * racer->mass * G;

	// Clamp to only cancel out actual sideways velocity (times mass), don't overshoot
	float correctionForce = -sidewaysVelocity * racer->mass;
	correctionForce = Clamp(correctionForce, -maxFriction, maxFriction);

	return Vector2Scale(sidewaysDirection, correctionForce);
}

static Vector2 getBrakeForce(Car *racer){
	if (Vector2LengthSqr(racer->velocity) > MIN_SPEED * MIN_SPEED) {
		return Vector2Scale(Vector2Normalize(racer->velocity), -racer->brakingMagnitude);
	}
	return racer->velocity;
}

void movePlayer(Car *player, Inputs hotkeys, uint8_t *map) {
	// turn player
	player->direction = Vector2Normalize(
		Vector2Rotate(player->direction, getTurningAngle(hotkeys, player->turningMagnitude))
	);

	bool isBraking = getDownHeld(hotkeys);
	bool isDriving = getUpHeld(hotkeys);

	Vector2 driving = isDriving ? getDrivingForce(player) : (Vector2){0, 0};
	Vector2 sideways = getSidewaysResistance(player);
	Vector2 brake = isBraking ? getBrakeForce(player) : (Vector2){0, 0};
	Vector2 resistance = getRoadResistance(player, map);
	

	Vector2 totalForce = Vector2Add(driving, resistance);
	totalForce = Vector2Add(totalForce, sideways);

	if (isBraking) {
		if (isDriving && player->hurtCooldown < 0) {
			player->health -= 0.5f;
			player->hurtCooldown = 0.3f * FRAMERATE;
		}
		totalForce = Vector2Add(totalForce, brake);
	}

	player->acceleration = Vector2Scale(totalForce, 1.0f / player->mass);
	player->velocity = Vector2Add(player->velocity, player->acceleration);

	if (!isDriving && Vector2LengthSqr(player->velocity) < MIN_SPEED * MIN_SPEED) {
		player->velocity = (Vector2){0, 0};
	}

	player->position = Vector2Add(player->position, player->velocity);
	player->hurtCooldown--;
}

