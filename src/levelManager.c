#include <stdio.h>
#include "levelManager.h"
#include "enemies.h"
Level currentLevel;

void spawnSwarm(uint32_t swarmSize, uint32_t difficulty)
{
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 1.;
    uint32_t baseMaxHealth = 100;
    float baseMoveSpeed = 100.;

    for (uint16_t enemyIndex = 0; enemyIndex < swarmSize; enemyIndex++)
    {
        int randValue = GetRandomValue(1, 100);
        spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed / difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, DARKBLUE, (float)randValue / 100), (Vector2){10 * randValue - 50, 10 * randValue - 50});
    }
    return;
}

void updateLevel()
{
    if (currentLevel.numSwarms <= 0)
    {
        return;
    }
    // printf("curTime: %f, time: %f\n", currentLevel.currentLevelTime, currentLevel.timeBetweenSwarms);
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
    spawnSwarm(currentLevel.swarmSize, currentLevel.difficultyLevel);
    currentLevel.numSwarms--;
}