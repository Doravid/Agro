#include "enemies.h"

Player enemies[MAX_PROJECTILES];
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

void drawEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        drawPlayer(enemies[enemyIndex]);
    }
}

void updateEnemies()
{
    for (uint16_t enemyIndex = 0; enemyIndex < numEnemies; enemyIndex++)
    {
        Player *currentEnemy = &enemies[enemyIndex];
        if (currentEnemy->attackCooldown <= 0)
        {
            spawnProjectileFromPlayer(*currentEnemy, EnemyProj);
            currentEnemy->attackCooldown += currentEnemy->attackSpeed;
        }
        currentEnemy->attackCooldown -= GetFrameTime();

        float ang = atan2f(mainPlayer.position.y - currentEnemy->position.y, mainPlayer.position.x - currentEnemy->position.x);
        float enemyAngle = currentEnemy->rotation * DEG2RAD;
        float delta = enemyAngle - ang;
        delta = atan2f(sinf(delta), cosf(delta));

        if (delta < 0)
            currentEnemy->rotation += currentEnemy->rotationSpeed * GetFrameTime();
        else
            currentEnemy->rotation -= currentEnemy->rotationSpeed * GetFrameTime();
    }
}