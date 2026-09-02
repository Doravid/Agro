#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>

#include "levelManager.h"
#include "enemies.h"
#include "audio.h"

Level currentLevel;
RoomData rooms[16];
uint32_t numRoomsLoaded = 0;
const float myGridSize = 85.0f;

bool roomDone(RoomData *room)
{
    if (room->isCleared)
        return true;
    if (numEnemies == 0)
    {
        room->isCleared = true;
        return true;
    }
    return false;
}

void loadRoom(const char *filepath, RoomData *room, Vector2 targetEntrance)
{
    *room = (RoomData){0};

    char *fileText = LoadFileText(filepath);
    if (!fileText)
        return;

    cJSON *root = cJSON_Parse(fileText);
    if (!root)
    {
        UnloadFileText(fileText);
        return;
    }

    Vector2 entranceLocal = {0, 0};
    cJSON *layers = cJSON_GetObjectItemCaseSensitive(root, "layerInstances");
    cJSON *layer = NULL;

    cJSON_ArrayForEach(layer, layers)
    {
        cJSON *layerId = cJSON_GetObjectItemCaseSensitive(layer, "__identifier");
        if (strcmp(layerId->valuestring, "IntGrid") == 0)
        {
            int gridWid = cJSON_GetObjectItemCaseSensitive(layer, "__cWid")->valueint;
            cJSON *gridCsv = cJSON_GetObjectItemCaseSensitive(layer, "intGridCsv");
            cJSON *tileValue = NULL;
            int tileIndex = 0;
            cJSON_ArrayForEach(tileValue, gridCsv)
            {
                if (tileValue->valueint == TILE_ENTRANCE)
                {
                    entranceLocal.x = (tileIndex % gridWid) * myGridSize;
                    entranceLocal.y = (tileIndex / gridWid) * myGridSize;
                }
                tileIndex++;
            }
        }
    }

    Vector2 offset = {targetEntrance.x - entranceLocal.x, targetEntrance.y - entranceLocal.y};

    cJSON_ArrayForEach(layer, layers)
    {
        cJSON *layerId = cJSON_GetObjectItemCaseSensitive(layer, "__identifier");

        // parse grid
        if (strcmp(layerId->valuestring, "IntGrid") == 0)
        {
            int gridWid = cJSON_GetObjectItemCaseSensitive(layer, "__cWid")->valueint;

            cJSON *gridCsv = cJSON_GetObjectItemCaseSensitive(layer, "intGridCsv");

            int totalTiles = cJSON_GetArraySize(gridCsv);
            room->colliders = malloc(sizeof(Collider) * totalTiles);

            cJSON *tileValue = NULL;
            int tileIndex = 0;

            cJSON_ArrayForEach(tileValue, gridCsv)
            {
                int type = tileValue->valueint;

                if (type == TILE_WALL || type == TILE_ENTRANCE || type == TILE_EXIT)
                {
                    int x = (tileIndex % gridWid) * myGridSize + offset.x;
                    int y = (tileIndex / gridWid) * myGridSize + offset.y;

                    room->colliders[room->numColliders].bounds = (Rectangle){(float)x, (float)y, (float)myGridSize, (float)myGridSize};
                    room->colliders[room->numColliders].type = type;
                    room->numColliders++;
                }
                tileIndex++;
            }
        }

        // PARSE THE ENTITIES
        else if (strcmp(layerId->valuestring, "Entities") == 0)
        {
            int ldtkGridSize = cJSON_GetObjectItemCaseSensitive(layer, "__gridSize")->valueint;

            float scaleFactor = myGridSize / (float)ldtkGridSize;

            cJSON *entities = cJSON_GetObjectItemCaseSensitive(layer, "entityInstances");
            cJSON *entity = NULL;

            cJSON_ArrayForEach(entity, entities)
            {
                cJSON *entId = cJSON_GetObjectItemCaseSensitive(entity, "__identifier");
                cJSON *pxArray = cJSON_GetObjectItemCaseSensitive(entity, "px");

                float rawX = (float)cJSON_GetArrayItem(pxArray, 0)->valueint;
                float rawY = (float)cJSON_GetArrayItem(pxArray, 1)->valueint;

                float scaledX = rawX * scaleFactor + myGridSize / 2.0f + offset.x;
                float scaledY = rawY * scaleFactor + myGridSize / 2.0f + offset.y;

                if (strcmp(entId->valuestring, "PlayerSpawn") == 0)
                {
                    room->playerSpawn = (Vector2){scaledX, scaledY};
                }
                else if (strcmp(entId->valuestring, "EnemySpawn") == 0)
                {
                    spawnRandomEnemyPos(1, (Vector2){.x = scaledX, scaledY});
                }
                else if (strcmp(entId->valuestring, "Boss1") == 0)
                {
                    spawnBoss1Pos((Vector2){.x = scaledX, scaledY});
                }
            }
        }
    }

    cJSON_Delete(root);
    UnloadFileText(fileText);

    return;
}

void drawRooms()
{
    for (uint32_t r = 0; r < numRoomsLoaded; r++)
    {
        RoomData *room = &rooms[r];
        for (uint32_t colliderIndex = 0; colliderIndex < room->numColliders; colliderIndex++)
        {
            if (room->colliders[colliderIndex].type == TILE_WALL)
                DrawRectangleRec(room->colliders[colliderIndex].bounds, GRAY);
            if (room->colliders[colliderIndex].type == TILE_EXIT && !roomDone(room))
                DrawRectangleRec(room->colliders[colliderIndex].bounds, DARKBROWN);
        }
    }
}

static bool checkEntityCollision(Vector2 pos, Vector2 size)
{
    Rectangle entityRec = {pos.x - size.x / 2.0f, pos.y - size.y / 2.0f, size.x, size.y};

    for (uint32_t r = 0; r < numRoomsLoaded; r++)
    {
        RoomData *room = &rooms[r];
        for (uint32_t i = 0; i < room->numColliders; i++)
        {
            if (room->colliders[i].type == TILE_ENTRANCE ||
                (room->colliders[i].type == TILE_EXIT && roomDone(room)))
            {
                continue;
            }

            if (CheckCollisionRecs(entityRec, room->colliders[i].bounds))
            {
                return true;
            }
        }
    }
    return false;
}

Vector2 moveWithCollision(Vector2 currentPos, Vector2 size, Vector2 offset)
{
    Vector2 nextPos = currentPos;

    if (offset.x != 0.0f)
    {
        Vector2 testPosX = {nextPos.x + offset.x, nextPos.y};
        if (!checkEntityCollision(testPosX, size))
        {
            nextPos.x += offset.x;
        }
    }

    if (offset.y != 0.0f)
    {
        Vector2 testPosY = {nextPos.x, nextPos.y + offset.y};
        if (!checkEntityCollision(testPosY, size))
        {
            nextPos.y += offset.y;
        }
    }

    return nextPos;
}

void updateRooms()
{
    if (numRoomsLoaded == 0 || numRoomsLoaded >= 16)
        return;

    RoomData *lastRoom = &rooms[numRoomsLoaded - 1];

    if (roomDone(lastRoom))
    {
        Rectangle playerRec = {mainPlayer.position.x - mainPlayer.size.x / 2.0f, mainPlayer.position.y - mainPlayer.size.y / 2.0f, mainPlayer.size.x, mainPlayer.size.y};

        for (uint32_t i = 0; i < lastRoom->numColliders; i++)
        {
            if (lastRoom->colliders[i].type == TILE_EXIT && CheckCollisionRecs(playerRec, lastRoom->colliders[i].bounds))
            {
                Vector2 targetEntrance = {lastRoom->colliders[i].bounds.x, lastRoom->colliders[i].bounds.y};
                if (numRoomsLoaded == 5)
                {
                    loadRoom("maps/thing/BossLevel.ldtkl", &rooms[numRoomsLoaded], targetEntrance);
                    numRoomsLoaded++;
                    playBossMusic();
                    break;
                }
                const char *nextMaps[] = {"maps/thing/Level_1.ldtkl", "maps/thing/Level_2.ldtkl", "maps/thing/Level_3.ldtkl"};
                int randIndex = GetRandomValue(0, 2);
                loadRoom(nextMaps[randIndex], &rooms[numRoomsLoaded], targetEntrance);
                numRoomsLoaded++;
                break;
            }
        }
    }
}