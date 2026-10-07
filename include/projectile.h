#pragma once
#include <stdint.h>

#include "player.h"

#define MAX_PROJECTILES 99999

typedef enum {
    PlayerProj,
    EnemyProj,
    VisualProj,
} ProjectileType;

typedef struct {
    Vector2 position, direction;
    Color color;
    float moveSpeed, size;
    uint32_t damage;
    ProjectileType owner;
    float lifetime;
} Projectile;

extern Projectile projectiles[MAX_PROJECTILES];
extern uint32_t numProjectiles;

void spawnProjectileFromPlayer(Player parent, ProjectileType owner);
void spawnProjectileFromPlayerPro(Player parent, ProjectileType owner, float size, float moveSpeed);
void drawProjectiles(Projectile *projs, uint32_t numProjs);
void updateProjectiles();
void spawnProjectile(Projectile proj_to_spawn);
void initProjectiles();
