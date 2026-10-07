#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#include "projectile.h"
#include "player.h"
#include "enemies.h"
#include "camera.h"
#include "levelManager.h"
#include <time.h>

Projectile projectiles[MAX_PROJECTILES];
uint32_t numProjectiles = 0;

void spawnProjectile(Projectile proj_to_spawn) {
    if (numProjectiles < MAX_PROJECTILES) {
        projectiles[numProjectiles] = proj_to_spawn;
        numProjectiles++;
    }
}
static Texture2D circleTexture;

void initProjectiles() {
    Image img = GenImageColor(64, 64, BLANK);
    ImageDrawCircle(&img, 32, 32, 32, WHITE);
    circleTexture = LoadTextureFromImage(img);
    UnloadImage(img);
}

void drawProjectiles(Projectile *projs, uint32_t numProjs) {

    for (uint16_t projectileIndex = 0; projectileIndex < numProjs; projectileIndex++) {
        Projectile *currentProjectile = &projs[projectileIndex];

        DrawTexturePro(circleTexture, (Rectangle){0, 0, 64, 64},
                       (Rectangle){currentProjectile->position.x, currentProjectile->position.y,
                                   currentProjectile->size * 2, currentProjectile->size * 2},
                       (Vector2){currentProjectile->size, currentProjectile->size}, 0.0f,
                       currentProjectile->color);
    }
}
void spawnProjectileFromPlayer(Player parent, ProjectileType owner) {
    spawnProjectileFromPlayerPro(parent, owner, 10.f, 250.f);
}

void spawnProjectileFromPlayerPro(Player parent, ProjectileType owner, float size,
                                  float moveSpeed) {
    Vector2 directionVector = {.x = cosf(parent.rotation * DEG2RAD),
                               .y = sinf(parent.rotation * DEG2RAD)};
    ProjectileType newOwner = owner;

    spawnProjectile((Projectile){
        .color = ColorLerp(PINK, parent.color, 0.5),
        .direction = directionVector,
        .position = {parent.position.x + directionVector.x * 40,
                     parent.position.y + directionVector.y * 40},
        .size = size,
        .moveSpeed = moveSpeed,
        .damage = parent.attackDamage,
        .owner = newOwner,
        .lifetime = 50.f,
    });
}

bool projectileHitsEntity(Projectile proj) {
    if (proj.owner == VisualProj)
        return false;
    if (proj.owner == EnemyProj) {
        if (mainPlayer.dashTimer > 0)
            return false;

        float hitDist = proj.size + mainPlayer.size.x / 2.0f;
        if (Vector2DistanceSqr(proj.position, mainPlayer.position) < (hitDist * hitDist)) {
            damagePlayer(proj.damage);
            return true;
        }
    }

    if (proj.owner == PlayerProj) {
        for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++) {
            float hitDist = proj.size + enemies[enemyIndex].size.x / 2.0f;
            if (Vector2DistanceSqr(proj.position, enemies[enemyIndex].position) <
                (hitDist * hitDist)) {
                return damageEnemy(enemyIndex, proj.damage);
            }
        }
    }

    for (uint32_t r = 0; r < numRoomsLoaded; r++) {
        RoomData *room = &rooms[r];
        bool isRoomDone = roomDone(room);

        for (uint32_t colliderIndex = 0; colliderIndex < room->numColliders; colliderIndex++) {
            int type = room->colliders[colliderIndex].type;
            if (type == TILE_ENTRANCE || (type == TILE_EXIT && isRoomDone)) {
                continue;
            }

            if (CheckCollisionCircleRec(proj.position, proj.size,
                                        room->colliders[colliderIndex].bounds)) {
                return true;
            }
        }
    }

    return false;
}

void updateProjectiles() {

    float frameTime = GetFrameTime();
    float cullDistSqr = 2500.f * 2500.f;
    for (uint16_t projectileIndex = 0; projectileIndex < numProjectiles; projectileIndex++) {
        Projectile *currentProjectile = &projectiles[projectileIndex];

        currentProjectile->position.x +=
            currentProjectile->direction.x * currentProjectile->moveSpeed * frameTime;
        currentProjectile->position.y +=
            currentProjectile->direction.y * currentProjectile->moveSpeed * frameTime;

        currentProjectile->lifetime -= frameTime;

        if (currentProjectile->lifetime <= 0.0f || projectileHitsEntity(*currentProjectile) ||
            Vector2DistanceSqr(currentProjectile->position, mainPlayer.position) > cullDistSqr) {
            *currentProjectile = projectiles[numProjectiles - 1];
            numProjectiles--;
            projectileIndex--;
        }
    }
}