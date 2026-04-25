#ifndef PHYSICS_H
#define PHYSICS_H

#include "raylib.h"
#include "util.h"
#include <stdint.h>

#define MIN_SPEED 0.003f
#define G 98.1 / (60 * 60 * 15)
// 1 pixel = 10 meters
// 9 is and arbitrary drifiting coefficient


typedef struct Car{
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	Vector2 direction;
	float drivingPower;
	float brakingMagnitude;
	float turningMagnitude;
	float resistanceCoefficient;
	float frictionCoefficient;
	float mass;
	float maxHealth;
	float health;
	float hurtCooldown;
} Car;


typedef struct Npc
{
	Car racecar;
	bool up;
	bool left;
	bool right;
	bool down;
	Vector2 target;
	// between 1 and 100
	int aggressiveness;
	int saftey;
	
	int inputCountdown;
} Npc;

void movePlayer(Car *player, Inputs hotkeys, uint8_t *map);

void moveNpc(Npc *racer, uint8_t *map);
#endif