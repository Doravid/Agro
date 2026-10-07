#include <stdio.h>

#include "enemies.h"
#include "camera.h"
#include "levelManager.h"
#include "projectile.h"
#include "boss.h"

void drawBoss1(Enemy *enemy);
void updateBoss1(Enemy *currentEnemy);
static float rotateTowardsTarget(float currentRotation, Vector2 currentPos, Vector2 targetPos,
                                 float rotationSpeed);
static Vector2 calculateOffsetMovement(Vector2 currentPos, Vector2 targetPos, float offsetDistance);

Enemy enemies[MAX_ENEMIES];
uint32_t numEnemies = 0;
bool gameOver = false;

bool damageEnemy(uint32_t enemyIndex, uint32_t damage) {
    Enemy *enemy = &enemies[enemyIndex];
    if (enemy->type == ENEMY_GHOST && enemy->extraData.ghost.state == GHOST_HIDING)
        return false;

    if (enemy->currentHealth <= damage) {
        enemy->currentHealth = 0;
        *enemy = enemies[numEnemies - 1];
        numEnemies--;
        if (enemy->type == ENEMY_BOSS1) {
            gameOver = true;
            history.coins += 50;
        } else {
            history.coins += 3;
        }
    } else
        enemy->currentHealth -= damage;
    return true;
}

static void drawMeleeEnemy(Enemy *enemy) {
    // Draw the Player Cube
    Color playerDrawColor = enemy->color;

    Rectangle playerRec = {enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y};
    Vector2 playerOrigin = {enemy->size.x * 0.5f, enemy->size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, enemy->rotation, playerDrawColor);

    // Draw the Health12 Bar
    drawHealthBar(enemy->size, enemy->position, (float)enemy->currentHealth / enemy->maxHealth,
                  enemy->color);
}

static void drawShooter(Enemy *enemy) { drawPlayer(*(Player *)enemy); }

void drawBuffshot(Enemy enemy) {
    Vector2 barrelSize = {17.0f, 30.0f};
    Color barrelDrawColor = DARKPURPLE;
    drawEntityWithBarrel(enemy.position, enemy.size, enemy.rotation, barrelSize, enemy.color,
                         barrelDrawColor, (float)enemy.currentHealth / enemy.maxHealth);
}

void drawGhost(Enemy enemy) {
    Color barrelDrawColor =
        enemy.extraData.ghost.state == GHOST_HIDING ? Fade(DARKPURPLE, 0.2) : DARKPURPLE;
    Color bodyColor =
        enemy.extraData.ghost.state == GHOST_HIDING ? Fade(enemy.color, 0.2) : enemy.color;
    drawEntityWithBarrel(enemy.position, enemy.size, enemy.rotation, (Vector2){17.0f, 17.0f},
                         bodyColor, barrelDrawColor, (float)enemy.currentHealth / enemy.maxHealth);
}

void drawEnemies() {
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++) {
        switch (enemies[enemyIndex].type) {
            case ENEMY_SHOOTER:
                drawShooter(&enemies[enemyIndex]);
                break;
            case ENEMY_MELEE:
                drawMeleeEnemy(&enemies[enemyIndex]);
                break;
            case ENEMY_BUFFSHOT:
                drawBuffshot(enemies[enemyIndex]);
                break;
            case ENEMY_GHOST:
                drawGhost(enemies[enemyIndex]);
                break;
            case ENEMY_BOSS1:
                drawBoss1(&enemies[enemyIndex]);
                break;
            default:
                break;
        }
    }
}
static void spawnEnemy(float rotationSpeed, float attackSpeed, uint32_t maxHealth, float moveSpeed,
                       Color color, Vector2 position, EnemyType type, uint32_t attackDamage) {
    enemies[numEnemies] = (Enemy){
        .rotationSpeed = rotationSpeed,
        .attackCooldown = 0.,
        .attackSpeed = attackSpeed,
        .color = color,
        .moveSpeed = moveSpeed,
        .position = position,
        .rotation = 0,
        .size = {40.f, 40.f},
        .maxHealth = maxHealth,
        .currentHealth = maxHealth,
        .type = type,
        .attackDamage = attackDamage,
    };
    numEnemies++;
}

void spawnShooterPos(uint32_t difficulty, Vector2 spawnPos) {
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 100.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty,
               baseMaxHealth * difficulty, baseMoveSpeed * difficulty,
               ColorLerp(BLUE, DARKBLUE, (float)randValue / 100), spawnPos, ENEMY_SHOOTER,
               baseAttackDamage);
    enemies[numEnemies - 1].currentWeapon = WEAPON_SHOOTER;
}

void spawnGhostPos(uint32_t difficulty, Vector2 spawnPos) {
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 300.;
    float baseAttackSpeed = 0.8;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 130.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty,
               baseMaxHealth * difficulty, baseMoveSpeed * difficulty,
               ColorLerp(BLUE, DARKBLUE, (float)randValue / 100), spawnPos, ENEMY_GHOST,
               baseAttackDamage);
    enemies[numEnemies - 1].currentWeapon = WEAPON_SHOOTER;
}

void spawnMeleePos(uint32_t difficulty, Vector2 spawnPos) {
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 200.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty,
               baseMaxHealth * difficulty, baseMoveSpeed * difficulty,
               ColorLerp(BLUE, PINK, (float)randValue / 100), spawnPos, ENEMY_MELEE,
               baseAttackDamage);
}

void spawnBuffshotPos(uint32_t difficulty, Vector2 spawnPos) {
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 5.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 14.;
    uint32_t baseAttackDamage = 40;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty,
               baseMaxHealth * difficulty, baseMoveSpeed * difficulty,
               ColorLerp(BLUE, PINK, (float)randValue / 100), spawnPos, ENEMY_BUFFSHOT,
               baseAttackDamage);
}

void spawnRandomEnemyPos(uint32_t difficulty, Vector2 spawnPos) {
    int randValue = GetRandomValue(ENEMY_SHOOTER, ENEMY_GHOST);
    switch (randValue) {
        case ENEMY_SHOOTER:
            spawnShooterPos(difficulty, spawnPos);
            break;
        case ENEMY_MELEE:
            spawnMeleePos(difficulty, spawnPos);
            break;
        case ENEMY_BUFFSHOT:
            spawnBuffshotPos(difficulty, spawnPos);
            break;
        case ENEMY_GHOST:
            spawnGhostPos(difficulty, spawnPos);
            break;
        default:
            break;
    }
}

void updateShooter(Enemy *enemy) {
    // Shoot at the player if possible.
    if (enemy->attackCooldown <= 0) {
        spawnProjectileFromPlayer(*(Player *)enemy, EnemyProj);
        enemy->attackCooldown += enemy->attackSpeed;
    }
    enemy->attackCooldown -= GetFrameTime();

    // Rotate Towards the player
    enemy->rotation = rotateTowardsTarget(enemy->rotation, enemy->position, mainPlayer.position,
                                          enemy->rotationSpeed);
    // Move towards the player
    const float offsetDistance = 150.f;
    enemy->movementVector =
        calculateOffsetMovement(enemy->position, mainPlayer.position, offsetDistance);

    Vector2 move =
        Vector2Scale(Vector2Normalize(enemy->movementVector), enemy->moveSpeed * GetFrameTime());
    enemy->position = moveWithCollision(enemy->position, enemy->size, move);
}

void updateGhost(Enemy *enemy) {

    // Handle State Timer and Changes
    if (enemy->extraData.ghost.stateTimer <= 0) {
        switch (enemy->extraData.ghost.state) {
            case GHOST_HIDING:
                enemy->extraData.ghost.state = GHOST_SHOOTING;
                enemy->position = moveWithCollision(enemy->position, enemy->size, (Vector2){0, 0});
                enemy->extraData.ghost.stateTimer += 3.f;
                break;
            case GHOST_SHOOTING:
                enemy->extraData.ghost.state = GHOST_HIDING;
                enemy->extraData.ghost.stateTimer += 3.f;
                break;
        }
    }
    enemy->extraData.ghost.stateTimer -= GetFrameTime();

    // Rotate Towards the player
    enemy->rotation = rotateTowardsTarget(enemy->rotation, enemy->position, mainPlayer.position,
                                          enemy->rotationSpeed);
    // State Specific Operations:

    // 1. If we are in shooting mode then just shoot.
    if (enemy->extraData.ghost.state == GHOST_SHOOTING) {
        // Shoot at the player if possible.
        if (enemy->attackCooldown <= 0) {
            spawnProjectileFromPlayer(*(Player *)enemy, EnemyProj);
            enemy->attackCooldown += enemy->attackSpeed;
        }
        enemy->attackCooldown -= GetFrameTime();

    } // If we are in hiding mode then we will simply move towards the player while invincible.
    else if (enemy->extraData.ghost.state == GHOST_HIDING) {
        enemy->movementVector =
            calculateOffsetMovement(enemy->position, mainPlayer.position, 100.0f);
        Vector2 move = Vector2Scale(Vector2Normalize(enemy->movementVector),
                                    enemy->moveSpeed * GetFrameTime());
        enemy->position = Vector2Add(enemy->position, move);
    }
}
void updateMelee(Enemy *currentEnemy) {
    currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();

    Vector2 targetPoint = mainPlayer.position;

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector),
                                currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);

    // Damage The player if we are in range.
    if (Vector2Distance(targetPoint, currentEnemy->position) < 15.f) {
        Vector2 dirToPlayer =
            Vector2Normalize(Vector2Subtract(currentEnemy->position, mainPlayer.position));
        currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size,
                                                   Vector2Scale(dirToPlayer, 90.0f));
        damagePlayer(currentEnemy->maxHealth / 10);
    }
}

void updateBuffshot(Enemy *enemy) {
    // Shoot at the player if possible.
    if (enemy->attackCooldown <= 0) {
        spawnProjectileFromPlayerPro(*(Player *)enemy, EnemyProj, 20.f, 100.f);
        enemy->attackCooldown += enemy->attackSpeed;
    }
    enemy->attackCooldown -= GetFrameTime();

    // Rotate Towards the player
    enemy->rotation = rotateTowardsTarget(enemy->rotation, enemy->position, mainPlayer.position,
                                          enemy->rotationSpeed);

    // Move towards the player
    enemy->movementVector = calculateOffsetMovement(enemy->position, mainPlayer.position, 150.0f);
    Vector2 move =
        Vector2Scale(Vector2Normalize(enemy->movementVector), enemy->moveSpeed * GetFrameTime());

    enemy->position = moveWithCollision(enemy->position, enemy->size, move);
}

void updateEnemies() {
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++) {
        Enemy *currentEnemy = &enemies[enemyIndex];
        switch (currentEnemy->type) {
            case ENEMY_SHOOTER:
                updateShooter(currentEnemy);
                break;
            case ENEMY_MELEE:
                updateMelee(currentEnemy);
                break;
            case ENEMY_BUFFSHOT:
                updateBuffshot(currentEnemy);
                break;
            case ENEMY_GHOST:
                updateGhost(currentEnemy);
                break;

            case ENEMY_BOSS1:
                updateBoss1(currentEnemy);
                break;
            default:
                break;
        }

        // Rotate towards the player
    }
}

// Helper Functions

static float rotateTowardsTarget(float currentRotation, Vector2 currentPos, Vector2 targetPos,
                                 float rotationSpeed) {
    float ang = atan2f(targetPos.y - currentPos.y, targetPos.x - currentPos.x);
    float currentAngle = currentRotation * DEG2RAD;
    float delta = currentAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (delta < 0)
        return currentRotation + rotationSpeed * GetFrameTime();
    else
        return currentRotation - rotationSpeed * GetFrameTime();
}

static Vector2 calculateOffsetMovement(Vector2 currentPos, Vector2 targetPos,
                                       float offsetDistance) {
    Vector2 dirToTarget = Vector2Normalize(Vector2Subtract(targetPos, currentPos));
    Vector2 offset = Vector2Scale(dirToTarget, offsetDistance);
    Vector2 targetPoint = Vector2Subtract(targetPos, offset);
    return Vector2Subtract(targetPoint, currentPos);
}
