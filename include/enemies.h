#pragma once
#include <stdint.h>
#include "player.h"
#include "projectile.h"

typedef enum
{
    ENEMY_SHOOTER,
    ENEMY_MELEE,
    ENEMY_CRUSHER,
    ENEMY_BOSS1,
    ENEMY_BOSS2,
    ENEMY_BOSS3,
    NUM_ENEMY_TYPES
} EnemyType;

typedef struct
{
    Player;
    EnemyType type;
} Enemy;

extern Enemy enemies[MAX_PROJECTILES];
extern uint32_t numEnemies;

void damageEnemy(uint32_t enemyIndex, uint32_t damage);
void updateEnemies();
void drawEnemies();
void spawnEnemy(float rotationSpeed, float attackSpeed, uint32_t maxHealth, float moveSpeed, Color color, Vector2 position);