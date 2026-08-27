#include <stdio.h>
#include "enemies.h"

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
void spawnEnemy(float rotationSpeed, float attackSpeed, uint32_t maxHealth, float moveSpeed, Color color, Vector2 position)
{
    enemies[numEnemies] = (Enemy){.rotationSpeed = rotationSpeed, .attackCooldown = 0., .attackSpeed = attackSpeed, .color = color, .moveSpeed = moveSpeed, .position = position, .rotation = 0, .size = {40.f, 40.f}, .maxHealth = maxHealth, .currentHealth = maxHealth};
    numEnemies++;
}

void updateEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        Enemy *currentEnemy = &enemies[enemyIndex];

        if (currentEnemy->attackCooldown <= 0)
        {
            spawnProjectileFromPlayer(*(Player *)currentEnemy, EnemyProj);
            currentEnemy->attackCooldown += currentEnemy->attackSpeed;
        }
        currentEnemy->attackCooldown -= GetFrameTime();

        // Rotate towards the player.
        float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
        float enemyAngle = currentEnemy->rotation * DEG2RAD;
        float delta = enemyAngle - ang;
        delta = atan2f(sinf(delta), cosf(delta));

        if (delta < 0)
            currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
        else
            currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();

        // Move towards the player
        Vector2 dirToPlayer = Vector2Normalize(Vector2Subtract(mainPlayer.position, currentEnemy->position));
        Vector2 offset = Vector2Scale(dirToPlayer, 150.0f);
        Vector2 targetPoint = Vector2Subtract(mainPlayer.position, offset);

        currentEnemy->movementVector = Vector2Subtract(targetPoint, currentEnemy->position);
        Vector2 move = Vector2Scale(Vector2Normalize(currentEnemy->movementVector), currentEnemy->moveSpeed * GetFrameTime());
        currentEnemy->position = Vector2Add(move, currentEnemy->position);
    }
}