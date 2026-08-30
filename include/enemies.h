#pragma once
#include <stdint.h>
#include "player.h"
#include "projectile.h"

typedef enum
{
    ENEMY_SHOOTER = 1,
    ENEMY_MELEE = 2,
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
Enemy *spawnShooter(uint32_t difficulty);
void spawnMelee(uint32_t difficulty);