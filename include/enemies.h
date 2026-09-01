#pragma once
#include <stdint.h>
#include "player.h"
#include "projectile.h"

typedef enum
{
    BOSS_IDLE,
    BOSS_SPIN_WINDUP,
    BOSS_SPIN,
    BOSS_LASER_WINDUP,
    BOSS_LASER_FIRE
} BossState;

typedef struct
{
    BossState state;
    float stateTimer;
    Vector2 leftArmTargetPoint;
    Vector2 rightArmTargetPoint;
} BossData;

typedef enum
{
    ENEMY_SHOOTER = 1,
    ENEMY_MELEE = 2,
    ENEMY_BUFFSHOT,
    ENEMY_BOSS1,
    ENEMY_BOSS2,
    ENEMY_BOSS3,
    NUM_ENEMY_TYPES
} EnemyType;

typedef struct
{
    Player;
    EnemyType type;
    union
    {
        BossData boss;
    } extraData;
} Enemy;

extern Enemy enemies[MAX_PROJECTILES];
extern uint32_t numEnemies;
extern bool gameOver;

void spawnBoss1Pos(Vector2 spawnPos);
void damageEnemy(uint32_t enemyIndex, uint32_t damage);
void updateEnemies();
void drawEnemies();
void spawnRandomEnemyPos(uint32_t difficulty, Vector2 spawnPos);