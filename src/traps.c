#include <raylib.h>
#include <raymath.h>
#include "traps.h"
#include "projectile.h"
#include <stdio.h>
FireTrap traps[MAX_TRAPS];
uint32_t numTraps = 0;
static const float myGridSize = 85.0f;

static Vector2 directionToVector(Direction dir)
{
    switch (dir)
    {
    case DIRECTION_DOWN:
        return (Vector2){.x = 0, .y = 1.f};
        break;
    case DIRECTION_UP:
        return (Vector2){.x = 0, .y = -1.f};
        break;
    case DIRECTION_LEFT:
        return (Vector2){.x = -1.f, .y = 0.f};
        break;
    case DIRECTION_RIGHT:
        return (Vector2){.x = 1.f, .y = 0.f};
        break;
    default:
        break;
    }
}
void drawTraps()
{
    for (uint32_t trapIndex = 0; trapIndex < numTraps; trapIndex++)
    {
        FireTrap trap = traps[trapIndex];
        DrawRectangle(trap.position.x, trap.position.y, myGridSize, myGridSize, (Color){.r = 220, .b = 20, .g = 40, .a = 255});

        Vector2 center = {trap.position.x + myGridSize / 2.0f, trap.position.y + myGridSize / 2.0f};
        Vector2 dirOffset = Vector2Scale(directionToVector(trap.direction), myGridSize / 2.0f);
        Vector2 nubinCenter = Vector2Add(center, dirOffset);

        float nubinSize = myGridSize / 4.0f;
        DrawRectangle(nubinCenter.x - (nubinSize / 2.0f), nubinCenter.y - (nubinSize / 2.0f), nubinSize, nubinSize, (Color){.r = 220, .b = 20, .g = 20, .a = 255});

        DrawRectangle(trap.position.x, trap.position.y, myGridSize, myGridSize, (Color){.r = 200, .b = 30, .g = 40, .a = 255});
    }
}

void addTrap(FireTrap trap)
{
    traps[numTraps] = trap;
    numTraps++;
}

void updateTraps()
{
    for (uint32_t trapIndex = 0; trapIndex < numTraps; trapIndex++)
    {
        FireTrap *trap = &traps[trapIndex];
        trap->currentTimer -= GetFrameTime();
        if (trap->state == FIRETRAP_IDLE)
        {
            if (trap->currentTimer <= 0)
            {
                trap->state = FIRETRAP_FIRING;
                trap->currentTimer = traps->maxTime;
            }
        }
        else if (trap->state == FIRETRAP_FIRING)
        {
            if (trap->currentTimer <= 0)
            {
                trap->state = FIRETRAP_IDLE;
                trap->currentTimer = trap->maxTime;
                continue;
            }
            if (trap->fireTimer <= 0)
                trap->fireTimer = trap->fireTimerMax;
            else
            {
                trap->fireTimer -= GetFrameTime();
                continue;
            }

            Vector2 center = {trap->position.x + myGridSize / 2.0f, trap->position.y + myGridSize / 2.0f};
            Vector2 dirOffset = Vector2Scale(directionToVector(trap->direction), myGridSize / 1.8f);
            Vector2 spawnPos = Vector2Add(center, dirOffset);

            Vector2 baseDir = directionToVector(trap->direction);
            float angle = atan2f(baseDir.y, baseDir.x) + (GetRandomValue(-11, 11) * DEG2RAD);
            float angle2 = atan2f(baseDir.y, baseDir.x) + (GetRandomValue(-11, 12) * DEG2RAD);
            Vector2 projDir = {cosf(angle), sinf(angle)};
            Vector2 projDir2 = {cosf(angle2), sinf(angle2)};

            float speed = 250.0f;

            spawnProjectile((Projectile){
                .color = ColorLerp(ORANGE, RED, GetRandomValue(0, 4) / 10.f),
                .direction = projDir,
                .position = spawnPos,
                .size = (float)GetRandomValue(4, 7),
                .moveSpeed = speed,
                .damage = trap->damage,
                .owner = EnemyProj,
                .lifetime = (2.0f * myGridSize) / speed});

            spawnProjectile((Projectile){
                .color = ColorLerp(ORANGE, RED, GetRandomValue(0, 6) / 10.f),
                .direction = projDir2,
                .position = spawnPos,
                .size = (float)GetRandomValue(4, 7),
                .moveSpeed = speed,
                .damage = trap->damage,
                .owner = EnemyProj,
                .lifetime = (2.0f * myGridSize) / speed});
        }
    }
}