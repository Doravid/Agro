#include <raylib.h>
#include <stdint.h>

typedef enum
{
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_ENTRANCE = 2,
    TILE_EXIT = 3,
    TILE_FLOOR = 4
} TileType;

typedef struct
{
    Rectangle bounds;
    TileType type;
} Collider;

typedef struct
{
    Vector2 playerSpawn;

    Collider *colliders;
    uint32_t numColliders;
    uint32_t numEnemies;
} RoomData;

typedef struct
{
    float currentLevelTime;
    float timeBetweenSwarms;
    uint32_t numSwarms, swarmSize;
    uint32_t difficultyLevel;
} Level;

void startLevel(Level level);
void updateLevel();
void drawLevel();
void loadRoom(const char *filepath, RoomData *room);
bool roomDone(RoomData *room);
bool checkEntityCollision(Vector2 pos, Vector2 size);
Vector2 moveWithCollision(Vector2 currentPos, Vector2 size, Vector2 offset);

extern RoomData currentRoom;