#pragma once

#include <raylib.h>
#include <stdint.h>
#define MAX_TRAPS 32

typedef enum
{
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_DOWN,
} Direction;

typedef enum
{
    FIRETRAP_FIRING,
    FIRETRAP_IDLE,
} FiretrapState;

typedef struct
{
    Vector2 position;
    float currentTimer, maxTime;
    uint32_t damage;
    Direction direction;
    FiretrapState state;

    float fireTimer, fireTimerMax;
} FireTrap;

extern FireTrap traps[MAX_TRAPS];
extern uint32_t numTraps;

void drawTraps();
void addTrap(FireTrap trap);
void updateTraps();