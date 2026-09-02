#include <stdio.h>

#include "enemies.h"
#include "camera.h"
#include "levelManager.h"
#include "projectile.h"
#include "boss.h"

void drawBoss1(Enemy *enemy);
void updateBoss1(Enemy *currentEnemy);

Enemy enemies[MAX_PROJECTILES];
uint32_t numEnemies = 0;

bool gameOver = false;

void damageEnemy(uint32_t enemyIndex, uint32_t damage)
{
    if (enemies[enemyIndex].currentHealth <= damage)
    {
        enemies[enemyIndex].currentHealth = 0;
        enemies[enemyIndex] = enemies[numEnemies - 1];
        numEnemies--;
        if (enemies[enemyIndex].type == ENEMY_BOSS1)
        {
            gameOver = true;
        }
    }
    else
        enemies[enemyIndex].currentHealth -= damage;
}

static void drawMeleeEnemy(Enemy *enemy)
{
    // Draw the Player Cube
    Color playerDrawColor = enemy->color;

    Rectangle playerRec = {enemy->position.x, enemy->position.y, enemy->size.x, enemy->size.y};
    Vector2 playerOrigin = {enemy->size.x * 0.5f, enemy->size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, enemy->rotation, playerDrawColor);

    // Draw the Health12 Bar
    drawHealthBar(enemy->size, enemy->position, (float)enemy->currentHealth / enemy->maxHealth, enemy->color);
}

static void drawShooter(Enemy *enemy)
{
    drawPlayer(*(Player *)enemy);
}

void drawBuffshot(Enemy enemy)
{
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {17.0f, 30.0f};

    Vector2 rotatedOffset = Vector2Rotate(childOffset, enemy.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(enemy.position, rotatedOffset);

    Color barrelDrawColor = DARKPURPLE;
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x, childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, enemy.rotation, barrelDrawColor);

    Rectangle playerRec = {enemy.position.x, enemy.position.y, enemy.size.x, enemy.size.y};
    Vector2 playerOrigin = {enemy.size.x * 0.5f, enemy.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, enemy.rotation, enemy.color);

    drawHealthBar(enemy.size, enemy.position, (float)enemy.currentHealth / enemy.maxHealth, enemy.color);
}

void drawEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        switch (enemies[enemyIndex].type)
        {
        case ENEMY_SHOOTER:
            drawShooter(&enemies[enemyIndex]);
            break;
        case ENEMY_MELEE:
            drawMeleeEnemy(&enemies[enemyIndex]);
            break;
        case ENEMY_BUFFSHOT:
            drawBuffshot(enemies[enemyIndex]);
            break;

        case ENEMY_BOSS1:
            drawBoss1(&enemies[enemyIndex]);
            break;
        default:
            break;
        }
    }
}
static void spawnEnemy(float rotationSpeed, float attackSpeed, uint32_t maxHealth, float moveSpeed, Color color, Vector2 position, EnemyType type, uint32_t attackDamage)
{
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

void spawnShooterPos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 100.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, DARKBLUE, (float)randValue / 100), spawnPos, ENEMY_SHOOTER, baseAttackDamage);
}
void spawnMeleePos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 200.;
    uint32_t baseAttackDamage = 15;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, PINK, (float)randValue / 100), spawnPos, ENEMY_MELEE, baseAttackDamage);
}

void spawnBuffshotPos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 5.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 14.;
    uint32_t baseAttackDamage = 40;

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, PINK, (float)randValue / 100), spawnPos, ENEMY_BUFFSHOT, baseAttackDamage);
}

void spawnRandomEnemyPos(uint32_t difficulty, Vector2 spawnPos)
{
    int randValue = GetRandomValue(ENEMY_SHOOTER, ENEMY_BUFFSHOT);
    switch (randValue)
    {
    case ENEMY_SHOOTER:
        spawnShooterPos(difficulty, spawnPos);
        break;
    case ENEMY_MELEE:
        spawnMeleePos(difficulty, spawnPos);
        break;
    case ENEMY_BUFFSHOT:
        spawnBuffshotPos(difficulty, spawnPos);
        break;
    default:
        break;
    }
}

void updateShooter(Enemy *currentEnemy)
{
    // Shoot at the player if possible.
    if (currentEnemy->attackCooldown <= 0)
    {
        spawnProjectileFromPlayer(*(Player *)currentEnemy, EnemyProj);
        currentEnemy->attackCooldown += currentEnemy->attackSpeed;
    }
    currentEnemy->attackCooldown -= GetFrameTime();

    // Rotate Towards the player
    float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
    float enemyAngle = currentEnemy->rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (delta < 0)
        currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
    else
        currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();
    // Move towards the player (well aktually a point that is 150 units away from the player in the direction of the player.)
    Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
    Vector2 offset = Vector2Scale(dirToPlayer, 150.0f);
    Vector2 targetPoint = Vector2Subtract(mainPlayer.position, offset);

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);
}
void updateMelee(Enemy *currentEnemy)
{
    currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();

    Vector2 targetPoint = mainPlayer.position;

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);

    if (Vector2Distance(targetPoint, currentEnemy->position) < 15.f)
    {
        Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(currentEnemy->position, mainPlayer.position));
        currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, Vector2Scale(dirToPlayer, 90.0f));
        damagePlayer(currentEnemy->maxHealth / 10);
    }
}

void updateBuffshot(Enemy *currentEnemy)
{
    // Shoot at the player if possible.
    if (currentEnemy->attackCooldown <= 0)
    {
        spawnProjectileFromPlayerPro(*(Player *)currentEnemy, EnemyProj, 20.f, 100.f);
        currentEnemy->attackCooldown += currentEnemy->attackSpeed;
    }
    currentEnemy->attackCooldown -= GetFrameTime();

    // Rotate Towards the player
    float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
    float enemyAngle = currentEnemy->rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (delta < 0)
        currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
    else
        currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();

    // Move towards the player (well aktually a point that is 150 units away from the player in the direction of the player.)
    Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
    Vector2 offset = Vector2Scale(dirToPlayer, 150.0f);
    Vector2 targetPoint = Vector2Subtract(mainPlayer.position, offset);

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = moveWithCollision(currentEnemy->position, currentEnemy->size, move);
}

void updateEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        Enemy *currentEnemy = &enemies[enemyIndex];
        switch (currentEnemy->type)
        {
        case ENEMY_SHOOTER:
            updateShooter(currentEnemy);
            break;
        case ENEMY_MELEE:
            updateMelee(currentEnemy);
            break;
        case ENEMY_BUFFSHOT:
            updateBuffshot(currentEnemy);
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
