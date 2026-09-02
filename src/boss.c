#include <raylib.h>
#include "boss.h"
#include "enemies.h"
#include "camera.h"
#include "levelManager.h"

#define BOSS_ARM_LENGTH 90.0f
#define BOSS_JOINT_SIZE 12.0f
#define BOSS_HAND_SIZE_X 48.0f
#define BOSS_RED_HAND_SIZE 30.0f
#define BOSS_LASER_LENGTH 1000.0f
#define BOSS_LASER_WIDTH 20.0f

static float getLaserLength(Vector2 start, float angle, float maxLength)
{
    Vector2 end = {start.x + cosf(angle) * maxLength, start.y + sinf(angle) * maxLength};
    float minLen = maxLength;

    for (uint32_t r = 0; r < numRoomsLoaded; r++)
    {
        RoomData *room = &rooms[r];
        for (uint32_t i = 0; i < room->numColliders; i++)
        {
            if (room->colliders[i].type == TILE_ENTRANCE ||
                (room->colliders[i].type == TILE_EXIT && roomDone(room)))
            {
                continue;
            }

            Rectangle rec = room->colliders[i].bounds;
            Vector2 points[4] = {
                {rec.x, rec.y},
                {rec.x + rec.width, rec.y},
                {rec.x + rec.width, rec.y + rec.height},
                {rec.x, rec.y + rec.height}};

            for (int j = 0; j < 4; j++)
            {
                Vector2 hitPoint;
                if (CheckCollisionLines(start, end, points[j], points[(j + 1) % 4], &hitPoint))
                {
                    float dist = Vector2Distance(start, hitPoint);
                    if (dist < minLen)
                    {
                        minLen = dist;
                        end = hitPoint;
                    }
                }
            }
        }
    }
    return minLen;
}

void drawBoss1(Enemy *enemy)
{
    // Draw the main body
    DrawCircle(enemy->position.x, enemy->position.y, enemy->size.x * 0.5f, RED);

    // Draw the eyes.
    const float eyeRadius = enemy->size.x * 0.1f;
    const float leftEyeOffset = enemy->position.x - enemy->size.x / 6.0f;
    const float rightEyeOffset = enemy->position.x + enemy->size.x / 6.0f;
    DrawCircle(leftEyeOffset, enemy->position.y - enemy->size.x / 5.0f, eyeRadius, BLACK);
    DrawCircle(rightEyeOffset, enemy->position.y - enemy->size.x / 5.0f, eyeRadius, BLACK);
    DrawRectangle(leftEyeOffset - eyeRadius / 2.0f, enemy->position.y - (enemy->size.x * 0.125f), enemy->size.x * 0.05f, enemy->size.x * 0.1875f, BLACK);
    DrawRectangle(rightEyeOffset, enemy->position.y - (enemy->size.x * 0.125f), enemy->size.x * 0.05f, enemy->size.x * 0.1875f, BLACK);

    Vector2 leftShoulder = Vector2Add(enemy->position, Vector2Rotate((Vector2){-enemy->size.x * 0.5f, 0.0f}, (enemy->rotation) * DEG2RAD));
    Vector2 rightShoulder = Vector2Add(enemy->position, Vector2Rotate((Vector2){enemy->size.x * 0.5f, 0.0f}, (enemy->rotation) * DEG2RAD));

    Vector2 shoulders[2] = {leftShoulder, rightShoulder};
    Vector2 targets[2] = {enemy->extraData.boss.leftArmTargetPoint, enemy->extraData.boss.rightArmTargetPoint};

    if (enemy->extraData.boss.state == BOSS_SPIN_WINDUP)
        DrawCircle(enemy->position.x, enemy->position.y, BOSS_ARM_LENGTH * 2.0f + enemy->size.x * 0.5f, Fade(RED, 0.2f));

    for (int i = 0; i < 2; i++)
    {
        float dist = Vector2Distance(shoulders[i], targets[i]);
        if (dist >= BOSS_ARM_LENGTH * 2.0f)
            dist = BOSS_ARM_LENGTH * 2.0f - 0.1f;

        float angleB = acosf((dist * dist + BOSS_ARM_LENGTH * BOSS_ARM_LENGTH - BOSS_ARM_LENGTH * BOSS_ARM_LENGTH) / (2.0f * dist * BOSS_ARM_LENGTH));
        float angleC = acosf((BOSS_ARM_LENGTH * BOSS_ARM_LENGTH + BOSS_ARM_LENGTH * BOSS_ARM_LENGTH - dist * dist) / (2.0f * BOSS_ARM_LENGTH * BOSS_ARM_LENGTH));
        float angleD = atan2f(targets[i].y - shoulders[i].y, targets[i].x - shoulders[i].x);

        float a1 = angleD - (i == 0 ? angleB : -angleB);
        float a2 = a1 + (i == 0 ? PI - angleC : -(PI - angleC));

        Vector2 elbow = {shoulders[i].x + cosf(a1) * BOSS_ARM_LENGTH, shoulders[i].y + sinf(a1) * BOSS_ARM_LENGTH};
        Vector2 hand = {elbow.x + cosf(a2) * BOSS_ARM_LENGTH, elbow.y + sinf(a2) * BOSS_ARM_LENGTH};

        // Draw The Arms
        DrawRectanglePro((Rectangle){shoulders[i].x, shoulders[i].y, BOSS_ARM_LENGTH, BOSS_JOINT_SIZE}, (Vector2){0, BOSS_JOINT_SIZE * 0.5f}, a1 * RAD2DEG, GRAY);
        DrawRectanglePro((Rectangle){elbow.x, elbow.y, BOSS_ARM_LENGTH, BOSS_JOINT_SIZE}, (Vector2){0, BOSS_JOINT_SIZE * 0.5f}, a2 * RAD2DEG, GRAY);

        // Draw The Joints
        DrawCircleV(shoulders[i], BOSS_JOINT_SIZE, DARKGRAY);
        DrawCircleV(elbow, BOSS_JOINT_SIZE, DARKGRAY);

        // Draw The Hands
        if (i == 0)
            DrawRectanglePro((Rectangle){hand.x, hand.y, BOSS_RED_HAND_SIZE, BOSS_HAND_SIZE_X}, (Vector2){BOSS_RED_HAND_SIZE * 0.5f, BOSS_HAND_SIZE_X * 0.5f}, a2 * RAD2DEG, DARKGRAY);
        else
            DrawCircle(hand.x, hand.y, BOSS_RED_HAND_SIZE * 0.5f, RED);
    }

    Vector2 rightTargetDir = Vector2Normalize(Vector2Subtract(enemy->extraData.boss.rightArmTargetPoint, rightShoulder));
    float rightDist = Vector2Distance(rightShoulder, enemy->extraData.boss.rightArmTargetPoint);
    if (rightDist >= BOSS_ARM_LENGTH * 2.0f)
        rightDist = BOSS_ARM_LENGTH * 2.0f - 0.1f;
    Vector2 rightHandPos = Vector2Add(rightShoulder, Vector2Scale(rightTargetDir, rightDist));

    switch (enemy->extraData.boss.state)
    {
    case BOSS_LASER_WINDUP:
    {
        float laserAngleRad = atan2f(enemy->extraData.boss.rightArmTargetPoint.y - rightHandPos.y, enemy->extraData.boss.rightArmTargetPoint.x - rightHandPos.x);
        float laserLength = getLaserLength(rightHandPos, laserAngleRad, BOSS_LASER_LENGTH);
        DrawRectanglePro((Rectangle){rightHandPos.x, rightHandPos.y, laserLength, BOSS_LASER_WIDTH}, (Vector2){0, BOSS_LASER_WIDTH * 0.5f}, laserAngleRad * RAD2DEG, Fade(RED, 0.2f));
        break;
    }
    case BOSS_LASER_FIRE:
    {
        float laserAngleRad = atan2f(enemy->extraData.boss.rightArmTargetPoint.y - rightHandPos.y, enemy->extraData.boss.rightArmTargetPoint.x - rightHandPos.x);
        float laserLength = getLaserLength(rightHandPos, laserAngleRad, BOSS_LASER_LENGTH);
        DrawRectanglePro((Rectangle){rightHandPos.x, rightHandPos.y, laserLength, BOSS_LASER_WIDTH}, (Vector2){0, BOSS_LASER_WIDTH * 0.5f}, laserAngleRad * RAD2DEG, RED);
        break;
    }
    default:
        break;
    }

    drawHealthBar(enemy->size, enemy->position, (float)enemy->currentHealth / enemy->maxHealth, GREEN);
}

void updateBoss1(Enemy *currentEnemy)
{
    currentEnemy->extraData.boss.stateTimer -= GetFrameTime();
    if (currentEnemy->extraData.boss.stateTimer <= 0)
    {
        switch (currentEnemy->extraData.boss.state)
        {
        case BOSS_IDLE:
            currentEnemy->extraData.boss.state = BOSS_SPIN_WINDUP;
            currentEnemy->extraData.boss.stateTimer = 1.0f;
            break;
        case BOSS_SPIN_WINDUP:
            currentEnemy->extraData.boss.state = BOSS_SPIN;
            currentEnemy->extraData.boss.stateTimer = 3.0f;
            triggerScreenShake(3.0f, 3);
            break;
        case BOSS_SPIN:
            currentEnemy->extraData.boss.state = BOSS_LASER_WINDUP;
            currentEnemy->extraData.boss.stateTimer = 1.5f;
            break;
        case BOSS_LASER_WINDUP:
            currentEnemy->extraData.boss.state = BOSS_LASER_FIRE;
            currentEnemy->extraData.boss.stateTimer = 1.0f;
            triggerScreenShake(0.3f, 14);
            break;
        case BOSS_LASER_FIRE:
            currentEnemy->extraData.boss.state = BOSS_IDLE;
            currentEnemy->extraData.boss.stateTimer = 2.0f;
            break;
        }
    }

    Vector2 leftShoulder = Vector2Add(currentEnemy->position, Vector2Rotate((Vector2){-currentEnemy->size.x * 0.5f, 0.0f}, currentEnemy->rotation * DEG2RAD));
    Vector2 rightShoulder = Vector2Add(currentEnemy->position, Vector2Rotate((Vector2){currentEnemy->size.x * 0.5f, 0.0f}, currentEnemy->rotation * DEG2RAD));
    Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));

    float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
    float enemyAngle = currentEnemy->rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    Vector2 rightTargetDir = Vector2Normalize(Vector2Subtract(currentEnemy->extraData.boss.rightArmTargetPoint, rightShoulder));
    float rightDist = Vector2Distance(rightShoulder, currentEnemy->extraData.boss.rightArmTargetPoint);
    if (rightDist >= BOSS_ARM_LENGTH * 2.0f)
        rightDist = BOSS_ARM_LENGTH * 2.0f - 0.1f;
    Vector2 rightHandPos = Vector2Add(rightShoulder, Vector2Scale(rightTargetDir, rightDist));

    switch (currentEnemy->extraData.boss.state)
    {
    case BOSS_IDLE:
    {
        // Shoot at the player if possible.
        if (currentEnemy->attackCooldown <= 0)
        {
            spawnProjectile((Projectile){
                .color = RED,
                .damage = currentEnemy->attackDamage,
                .direction = dirToPlayer,
                .moveSpeed = 80,
                .owner = EnemyProj,
                .position = rightHandPos,
                .size = 20,
            });
            currentEnemy->attackCooldown += currentEnemy->attackSpeed;
        }
        currentEnemy->attackCooldown -= GetFrameTime();

        Vector2 targetPoint = mainPlayer.position;

        currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
        Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
        currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);

        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){0.0f, -BOSS_ARM_LENGTH}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = mainPlayer.position;

        if (Vector2Distance(mainPlayer.position, currentEnemy->position) < currentEnemy->size.x * 0.5f)
        {
            damagePlayer(currentEnemy->attackDamage * GetFrameTime());
        }
        break;
    }
    case BOSS_SPIN_WINDUP:
    {
        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){BOSS_ARM_LENGTH * 0.5f, 0.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){-BOSS_ARM_LENGTH * 0.5f, 0.0f}, currentEnemy->rotation * DEG2RAD));
        break;
    }
    case BOSS_SPIN:
    {
        currentEnemy->rotation += 600.0f * GetFrameTime();
        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){-(BOSS_ARM_LENGTH * 2.0f), 0.0f}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = Vector2Add(rightShoulder, Vector2Rotate((Vector2){(BOSS_ARM_LENGTH * 2.0f), 0.0f}, currentEnemy->rotation * DEG2RAD));

        float spinRadius = BOSS_ARM_LENGTH * 2.0f + currentEnemy->size.x * 0.5f;
        if (Vector2Distance(mainPlayer.position, currentEnemy->position) < spinRadius)
        {
            damagePlayer(currentEnemy->attackDamage * GetFrameTime());
            Vector2 knockbackDir = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
            mainPlayer.position = moveWithCollision(mainPlayer.position, mainPlayer.size, Vector2Scale(knockbackDir, 90.0f));
        }
        break;
    }
    case BOSS_LASER_WINDUP:
    {
        if (delta < 0)
            currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
        else
            currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();

        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){0.0f, -BOSS_ARM_LENGTH}, currentEnemy->rotation * DEG2RAD));
        currentEnemy->extraData.boss.rightArmTargetPoint = mainPlayer.position;
        break;
    }
    case BOSS_LASER_FIRE:
    {
        currentEnemy->extraData.boss.leftArmTargetPoint = Vector2Add(leftShoulder, Vector2Rotate((Vector2){0.0f, -BOSS_ARM_LENGTH}, currentEnemy->rotation * DEG2RAD));

        float laserAngleRad = atan2f(currentEnemy->extraData.boss.rightArmTargetPoint.y - rightHandPos.y, currentEnemy->extraData.boss.rightArmTargetPoint.x - rightHandPos.x);
        float laserLength = getLaserLength(rightHandPos, laserAngleRad, BOSS_LASER_LENGTH);

        Vector2 pRot = Vector2Rotate(Vector2Subtract(mainPlayer.position, rightHandPos), -laserAngleRad);
        if (pRot.x > 0.0f && pRot.x < laserLength && pRot.y > -BOSS_LASER_WIDTH * 0.5f && pRot.y < BOSS_LASER_WIDTH * 0.5f)
        {
            static float timeSinceLastAttack = 0.;
            if (timeSinceLastAttack <= 0)
            {
                damagePlayer(currentEnemy->attackDamage / 2);
                timeSinceLastAttack += 0.2;
            }
            timeSinceLastAttack -= GetFrameTime();
        }
        break;
    }
    }
}

void spawnBoss1Pos(Vector2 spawnPos)
{

    enemies[numEnemies] = (Enemy){
        .attackCooldown = 0.,
        .attackSpeed = 0.6,
        .color = GREEN,
        .moveSpeed = 30,
        .position = spawnPos,
        .rotation = 0,
        .size = {80.f, 80.f},
        .maxHealth = 1000,
        .currentHealth = 1000,
        .type = ENEMY_BOSS1,
        .attackDamage = 45,
        .extraData.boss.stateTimer = 3.f};
    numEnemies++;
}