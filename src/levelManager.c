
#include "levelManager.h"
#include "enemies.h"
Level currentLevel;

void spawnSwarm(uint32_t swarmSize, uint32_t difficulty)
{
    float baseRotationSpeed = 100.;
    float baseAttackSpeed = 10.;
    int baseMaxHealth = 100;
    float baseMoveSpeed = 100.;
    int randValue = GetRandomValue(1, 100);
    for (uint16_t enemyIndex = 0; enemyIndex < swarmSize; enemyIndex++)
    {
        spawnEnemy(baseRotationSpeed * difficulty, baseAttackSpeed * difficulty, baseMaxHealth * difficulty, baseMoveSpeed * difficulty, ColorLerp(BLUE, GREEN, randValue / 100), (Vector2){100, 100});
    }
    return;
}

void updateLevel()
{
    if (currentLevel.numSwarms == 0)
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