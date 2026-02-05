#ifndef PHYSICS_H
#define PHYSICS_H

#include "raylib.h"
#include "util.h"

#define MIN_SPEED 0.003f


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



void movePlayer(Car *player, Inputs hotkeys);


#endif