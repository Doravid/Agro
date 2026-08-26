#include <raylib.h>
#include <stdint.h>

typedef struct
{
    float currentLevelTime;
    float timeBetweenSwarms;
    uint32_t numSwarms, swarmSize;
    uint32_t difficultyLevel;
} Level;
void startLevel(Level level);
void updateLevel();