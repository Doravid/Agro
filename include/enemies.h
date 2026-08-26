#pragma once

#include "player.h"
#include "projectile.h"

extern Player enemies[MAX_PROJECTILES];
extern uint32_t numEnemies;

void damageEnemy(uint32_t enemyIndex, uint32_t damage);
void updateEnemies();
void drawEnemies();
void spawnEnemy(float rotationSpeed, float attackSpeed, float maxHealth, float moveSpeed, Color color, Vector2 position);