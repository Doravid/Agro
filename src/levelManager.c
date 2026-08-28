#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

#include "levelManager.h"
#include "enemies.h"
Level currentLevel;

void spawnSwarm(uint32_t swarmSize, uint32_t difficulty)
{

    for (uint16_t enemyIndex = 0; enemyIndex < swarmSize; enemyIndex++)
    {
        EnemyType type = GetRandomValue(ENEMY_SHOOTER, ENEMY_MELEE);
        switch (type)
        {
        case ENEMY_SHOOTER:
            spawnShooter(difficulty);
            break;
        case ENEMY_MELEE:
            spawnMelee(difficulty);
            break;
        default:
            break;
        }
    }

    return;
}

void updateLevel()
{
    if (currentLevel.numSwarms <= 0)
    {
        return;
    }
    if (currentLevel.currentLevelTime > currentLevel.timeBetweenSwarms)
    {
        spawnSwarm(currentLevel.swarmSize, currentLevel.difficultyLevel);
        currentLevel.currentLevelTime = 0.f;
        currentLevel.numSwarms--;
    }
    currentLevel.currentLevelTime += GetFrameTime();
}

void startLevel(Level level)
{
    currentLevel = level;
    currentLevel.currentLevelTime = 0;
}