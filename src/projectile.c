
#include <raylib.h>
#include <raymath.h>

#include "projectile.h"
#include "player.h"
#include "enemies.h"

Projectile projectiles[MAX_PROJECTILES];
uint32_t numProjectiles = 0;

void spawnProjectile(Projectile proj_to_spawn)
{
    if (numProjectiles < MAX_PROJECTILES)
    {
        projectiles[numProjectiles] = proj_to_spawn;
        numProjectiles++;
    }
}

void drawProjectiles(Projectile *projs, uint32_t numProjs)
{
    for (uint16_t projectileIndex = 0; projectileIndex < numProjs; projectileIndex++)
    {
        Projectile currentProjectile = projs[projectileIndex];
        DrawCircle(currentProjectile.position.x, currentProjectile.position.y, currentProjectile.size, currentProjectile.color);
    }
}
void spawnProjectileFromPlayer(Player parent, ProjectileOwner owner)
{
    Vector2 directionVector = {.x = cosf(parent.rotation * DEG2RAD), .y = sinf(parent.rotation * DEG2RAD)};
    spawnProjectile((Projectile){
        .color = ColorLerp(PINK, parent.color, 0.5),
        .direction = directionVector,
        .position = {parent.position.x + directionVector.x * 40, parent.position.y + directionVector.y * 40},
        .size = 10.f,
        .moveSpeed = 250.f,
        .damage = 15,
        .owner = owner,

    });
}
bool projectileHitsEntity(Projectile proj)
{
    if (proj.owner == EnemyProj && Vector2Distance(proj.position, mainPlayer.position) < proj.size + mainPlayer.size.x / 2)
    {
        if (mainPlayer.dashTimer > 0)
            return false;
        damagePlayer(proj.damage);
        return true;
    }
    if (proj.owner == PlayerProj)
    {
        for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
        {
            if (Vector2Distance(proj.position, enemies[enemyIndex].position) < proj.size + enemies[enemyIndex].size.x / 2)
            {
                damageEnemy(enemyIndex, proj.damage);
                return true;
            }
        }
    }

    return false;
}
void updateProjectiles()
{
    for (uint16_t projectileIndex = 0; projectileIndex < numProjectiles; projectileIndex++)
    {
        Projectile *currentProjectile = &projectiles[projectileIndex];

        currentProjectile->position.x += currentProjectile->direction.x * currentProjectile->moveSpeed * GetFrameTime();
        currentProjectile->position.y += currentProjectile->direction.y * currentProjectile->moveSpeed * GetFrameTime();

        if (projectileHitsEntity(*currentProjectile) || Vector2Distance(currentProjectile->position, mainPlayer.position) > 1000.)
        {
            *currentProjectile = projectiles[numProjectiles - 1];
            numProjectiles--;
            projectileIndex--;
        }
    }
}