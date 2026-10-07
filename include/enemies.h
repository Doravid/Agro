#pragma once
#include <stdint.h>
#include "player.h"
#include "projectile.h"
#include "boss.h"
#define MAX_ENEMIES (32)
typedef struct {
    BossState state;
    float stateTimer;
    Vector2 leftArmTargetPoint;
    Vector2 rightArmTargetPoint;
} BossData;

typedef enum {
    GHOST_SHOOTING,
    GHOST_HIDING,
} GhostState;

typedef struct {
    GhostState state;
    float stateTimer;
} GhostData;

typedef enum {
    ENEMY_SHOOTER = 1,
    ENEMY_MELEE = 2,
    ENEMY_BUFFSHOT = 3,
    ENEMY_GHOST = 4,
    ENEMY_BOSS1,
    ENEMY_BOSS2,
    ENEMY_BOSS3,
    NUM_ENEMY_TYPES
} EnemyType;

typedef struct {
    Player;
    EnemyType type;
    union {
        BossData boss;
        GhostData ghost;
    } extraData;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];
extern uint32_t numEnemies;
extern bool gameOver;

bool damageEnemy(uint32_t enemyIndex, uint32_t damage);
void updateEnemies();
void drawEnemies();
void spawnRandomEnemyPos(uint32_t difficulty, Vector2 spawnPos);
