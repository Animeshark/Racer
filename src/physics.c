#include "physics.h"
#include "raylib.h"
#include "raymath.h"
#include "util.h"



static Vector2 getDrivingForce(Car *player, Inputs hotkeys) {
	if(getUpHeld(hotkeys)) {
		float forwardVelocity = Vector2DotProduct(player->direction, player->velocity);
		forwardVelocity = fmaxf(forwardVelocity, MIN_SPEED);
		return Vector2Scale(player->direction, player->drivingPower / forwardVelocity);
	}
	return (Vector2){0, 0};
}

static float getTurningAngle(Inputs hotkeys, float turning) {
	// clockwise is positive
	if(getLeftHeld(hotkeys)) return -turning;
	if(getRightHeld(hotkeys)) return turning;
	return 0.0f;
}

static Vector2 getSidewaysResistance(Car *player){
	//Perpendicular direction
	Vector2 sidewaysDirection = (Vector2) {player->direction.y, -player->direction.x};
	// Velocity component perpendicular to the direction of the player
	Vector2 sidewaysVelocity = Vector2Scale(sidewaysDirection, Vector2DotProduct(sidewaysDirection, player->velocity));
	return Vector2Scale(sidewaysVelocity, -player->resistanceCoefficient);
}

static Vector2 getBrakeForce(Car *player){
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

