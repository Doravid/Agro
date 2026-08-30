#include <stdio.h>
#include "enemies.h"
#include "camera.h"
#include "levelManager.h"

Enemy enemies[MAX_PROJECTILES];
uint32_t numEnemies = 0;

void damageEnemy(uint32_t enemyIndex, uint32_t damage)
{
    if (enemies[enemyIndex].currentHealth <= damage)
    {
        enemies[enemyIndex].currentHealth = 0;
        enemies[enemyIndex] = enemies[numEnemies - 1];
        numEnemies--;
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

        default:
            break;
        }
    }
}
static Enemy *spawnEnemy(float rotationSpeed, float attackSpeed, uint32_t maxHealth, float moveSpeed, Color color, Vector2 position, EnemyType type)
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
    };
    numEnemies++;
    return &enemies[numEnemies];
}

Enemy *spawnShooter(uint32_t difficulty)
{
    int randValueX = GetRandomValue(0, 100);
    int randValueY = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 100.;
    Vector2 spawnPos = Vector2Add(mainPlayer.position, (Vector2){10.f * randValueX - 500.f, 10.f * randValueY - 500.f});

    return spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, DARKBLUE, (float)randValueX / 100), spawnPos, ENEMY_SHOOTER);
}
void spawnMelee(uint32_t difficulty)
{
    int randValueX = GetRandomValue(0, 100);
    int randValueY = GetRandomValue(0, 100);
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 200.;
    Vector2 spawnPos = Vector2Add(mainPlayer.position, (Vector2){10.f * randValueX - 500.f, 10.f * randValueY - 500.f});

    spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, DARKBLUE, (float)randValueX / 100), spawnPos, ENEMY_MELEE);
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
    currentEnemy->position = Vector2Add(move, currentEnemy->position);
}
void updateMelee(Enemy *currentEnemy)
{
    currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();

    Vector2 targetPoint = mainPlayer.position;

    currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
    Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
    currentEnemy->position = Vector2Add(move, currentEnemy->position);

    if (Vector2Distance(targetPoint, currentEnemy->position) < 15.f)
    {
        Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(currentEnemy->position, mainPlayer.position));
        Vector2 offset = Vector2Add(Vector2Scale(dirToPlayer, 90.0f), mainPlayer.position);
        currentEnemy->position = offset;
        damagePlayer(currentEnemy->maxHealth / 10);
    }
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
        default:
            break;
        }

        // Rotate towards the player
    }
}