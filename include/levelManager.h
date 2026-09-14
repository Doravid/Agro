#include <raylib.h>
#include <stdint.h>
typedef enum {
  TILE_EMPTY = 0,
  TILE_WALL = 1,
  TILE_ENTRANCE = 2,
  TILE_EXIT = 3,
  TILE_FLOOR = 4
} TileType;

typedef struct {
  Rectangle bounds;
  TileType type;
} Collider;

typedef struct {
  Vector2 playerSpawn;

  Collider *colliders;
  uint32_t numColliders;
  uint32_t numEnemies;
  bool isCleared;
} RoomData;

typedef struct {
  float currentLevelTime;
  float timeBetweenSwarms;
  uint32_t numSwarms, swarmSize;
  uint32_t difficultyLevel;
} Level;

typedef enum {
  STATE_MAIN_MENU,
  STATE_SETTINGS,
  STATE_SHOP,
  STATE_PLAYING,
} GameState;

/**
 * Upgrades:
 * - Flat Health Upgrade
 * - Flat Damage Upgrade
 * - Regeneration Upgrade (Per Room, e.g Heal 5 health after each room).
 * - New Rooms Upgrade
 *    - Health Room (Heals to Full and/or adds 15 to your max health)
 *    - Chest Room (Gives you a random Item)
 * - Add the ability To dodge.
 * - Increase Move Speed.
 * - Decrease the number of rooms on each floor by 1 (Toggle-able)
 * Items:
 *  - For each attack will add two weaker attacks (BTD Ship Grade Upgrade)
 *  - Burning Attacks (Your attacks deal BURN to enemies)
 *  - Freezing Attack (Your attacks momentarily freeze a non-boss enemy)
 *  - Flat Damage Boost
 *  - Max Health Boost
 *  Requires Dodge:
 *  - If you dodge through an attack gain +2 health
 *  - If you dodge through an attack gain damage for you next attack
 * Weapons (And Their Upgrades)
 *  - Basic Shooter (Pretty Fast Attack Speed, Normal Damage, Ranged)
 *    - Attack Speed
 *    - Rotation Speed.
 *    - Every Third Shot is bigger and deals more damage
 *  - Flame Shot (Fast Attack Speed, Normal Damage, Applies Burn, Ranged)
 *    - Hotter Shots (More Burn)
 *    - Faster Flame (The fire itself moves faster, with a longer range)
 *    - Rotation Speed
 * - Knight (Slow Attack Speed, Very Large Damage, Melee)
 *    - Combo! (Hold Left Click to continuously Attack)
 *    - Parry This You Filthy Casual! (Adds the ability parry ALL attacks)
 *    - Minions that will run towards enemies (melee enemy but on your side.
 *      Will die on enemies it hits and regenerate every few seconds)
 * - Big Shot (Makes no sense, absurd damage, low: health, attack speed, move
 * speed, and turn speed)
 *    - The Shot Gets Bigger.
 *    - The Shot Will now Explode
 *    - The Shot gains more size / damage if you are not moving when shooting.
 */
typedef struct {

} Progress;

void updateRooms();
void initRoomTexture();
void drawRooms();
void startGame();
void loadRoom(const char *filepath, RoomData *room, Vector2 targetEntrance);
bool roomDone(RoomData *room);
Vector2 moveWithCollision(Vector2 currentPos, Vector2 size, Vector2 offset);

extern RoomData currentRoom;
extern uint32_t numRoomsLoaded;
extern RoomData rooms[16];
extern GameState currentState;