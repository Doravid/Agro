#pragma once
#include <stdint.h>
#include "player.h"

#define MAX_PROJECTILES 512

typedef enum
{
    PlayerProj,
    EnemyProj,
} ProjectileOwner;
typedef struct
{
    Vector2 position, direction;
    Color color;
    float moveSpeed, size;
    uint32_t damage;
    ProjectileOwner owner;
} Projectile;
extern Projectile projectiles[MAX_PROJECTILES];
extern uint32_t numProjectiles;

void spawnProjectileFromPlayer(Player parent, ProjectileOwner owner);
void drawProjectiles(Projectile *projs, uint32_t numProjs);
void updateProjectiles();