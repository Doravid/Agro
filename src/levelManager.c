#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>

#include "levelManager.h"
#include "enemies.h"

Level currentLevel;
RoomData currentRoom;

typedef struct
{
    Vector2 position;
    uint32_t maxHealth, currentHealth;
    float size;
    Color color;
} Obstacle;

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
RoomData loadRoom(const char *filepath)
{
    RoomData room = {0};

    // Raylib function to load text file into a string
    char *fileText = LoadFileText(filepath);
    if (!fileText)
        return room;

    cJSON *root = cJSON_Parse(fileText);
    if (!root)
    {
        UnloadFileText(fileText);
        return room;
    }

    cJSON *layers = cJSON_GetObjectItemCaseSensitive(root, "layerInstances");
    cJSON *layer = NULL;

    cJSON_ArrayForEach(layer, layers)
    {
        cJSON *layerId = cJSON_GetObjectItemCaseSensitive(layer, "__identifier");

        // parse grid
        if (strcmp(layerId->valuestring, "IntGrid") == 0)
        {
            int gridWid = cJSON_GetObjectItemCaseSensitive(layer, "__cWid")->valueint;
            int gridSize = 85;

            cJSON *gridCsv = cJSON_GetObjectItemCaseSensitive(layer, "intGridCsv");

            int totalTiles = cJSON_GetArraySize(gridCsv);
            room.colliders = malloc(sizeof(Collider) * totalTiles);

            cJSON *tileValue = NULL;
            int tileIndex = 0;

            cJSON_ArrayForEach(tileValue, gridCsv)
            {
                int type = tileValue->valueint;

                if (type == TILE_WALL || type == TILE_ENTRANCE || type == TILE_EXIT)
                {
                    int x = (tileIndex % gridWid) * gridSize;
                    int y = (tileIndex / gridWid) * gridSize;

                    room.colliders[room.numColliders].bounds = (Rectangle){(float)x, (float)y, (float)gridSize, (float)gridSize};
                    room.colliders[room.numColliders].type = type;
                    room.numColliders++;
                }
                tileIndex++;
            }
        }

        // PARSE THE ENTITIES
        else if (strcmp(layerId->valuestring, "Entities") == 0)
        {
            // Get LDtk's grid size (which is 16)
            int ldtkGridSize = cJSON_GetObjectItemCaseSensitive(layer, "__gridSize")->valueint;

            // Your custom game grid size
            float myGridSize = 85.0f;
            float scaleFactor = myGridSize / (float)ldtkGridSize;

            cJSON *entities = cJSON_GetObjectItemCaseSensitive(layer, "entityInstances");
            cJSON *entity = NULL;

            cJSON_ArrayForEach(entity, entities)
            {
                cJSON *entId = cJSON_GetObjectItemCaseSensitive(entity, "__identifier");
                cJSON *pxArray = cJSON_GetObjectItemCaseSensitive(entity, "px");

                // 1. Get the raw LDtk pixel coordinates
                float rawX = (float)cJSON_GetArrayItem(pxArray, 0)->valueint;
                float rawY = (float)cJSON_GetArrayItem(pxArray, 1)->valueint;

                // 2. Scale them up to your 85x85 grid
                float scaledX = rawX * scaleFactor;
                float scaledY = rawY * scaleFactor;

                // 3. (Optional but recommended) Center the spawn inside the 85x85 tile
                scaledX += myGridSize / 2.0f;
                scaledY += myGridSize / 2.0f;

                if (strcmp(entId->valuestring, "PlayerSpawn") == 0)
                {
                    room.playerSpawn = (Vector2){scaledX, scaledY};
                }
                else if (strcmp(entId->valuestring, "EnemySpawn") == 0)
                {
                    // spawnShooter(1, scaledX, scaledY);
                }
            }
        }
    }

    cJSON_Delete(root);
    UnloadFileText(fileText);

    return room;
}

void drawLevel()
{
    for (uint32_t colliderIndex = 0; colliderIndex < currentRoom.numColliders; colliderIndex++)
    {
        if (currentRoom.colliders[colliderIndex].type == TILE_WALL)
            DrawRectangleRec(currentRoom.colliders[colliderIndex].bounds, WHITE);
        if (currentRoom.colliders[colliderIndex].type == TILE_EXIT)
            DrawRectangleRec(currentRoom.colliders[colliderIndex].bounds, DARKBROWN);
    }
}