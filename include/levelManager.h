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
    bool isCleared;
} RoomData;

typedef struct
{
    float currentLevelTime;
    float timeBetweenSwarms;
    uint32_t numSwarms, swarmSize;
    uint32_t difficultyLevel;
} Level;

typedef struct
{
    Vector2 position;
    uint32_t maxHealth, currentHealth;
    float size;
    Color color;
} Obstacle;

void updateRooms();
void drawRooms();
void loadRoom(const char *filepath, RoomData *room, Vector2 targetEntrance);
bool roomDone(RoomData *room);
Vector2 moveWithCollision(Vector2 currentPos, Vector2 size, Vector2 offset);

extern RoomData currentRoom;
extern uint32_t numRoomsLoaded;
extern RoomData rooms[16];