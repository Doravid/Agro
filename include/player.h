#pragma once

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

typedef enum {
    WEAPON_SHOOTER = 1,
    WEAPON_FLAME = 2,
    WEAPON_KNIGHT = 4,
    WEAPON_BIGSHOT = 8
} PlayerWeapon;

typedef enum {
    ITEM_BURN = 1,
    ITEM_FLAME = 2,
    ITEM_DAMAGE = 4,
    ITEM_HEALTH = 8,
} PlayerItem;
/**
 * @brief The Following ENUMS represents the UNLOCK level for each of the
 * upgrades. They are in order, and must be unlocked in order.
 */
typedef enum {
    SHOOTER_NOT_UNLOCKED,
    SHOOTER_ATTACK_SPEED,
    SHOOTER_ROTATION_SPEED,
    SHOOTER_COMBO,
} ShooterUpgradeLevel;

typedef enum {
    FLAME_SHOT_NOT_UNLOCKED,
    FLAME_SHOT_MORE_BURN,
    FLAME_SHOT_FASTER_FLAME,
    FLAME_SHOT_ROTATION_SPEED,
} FlameShotUpgradeLevel;

typedef enum {
    KNIGHT_NOT_UNLOCKED,
    KNIGHT_PARRY,
    KNIGHT_COMBO_ATTACK,
    KNIGHT_MINIONS,
} KnightUpgradeLevel;

typedef enum {
    BIGSHOT_NOT_UNLOCKED,
    BIGSHOT_ATTACK_SPEED,
    BIGSHOT_ROTATION_SPEED,
    BIGSHOT_COMBO,
} BigshotUpgradeLevel;

typedef enum {
    NO_UPGRADES = 0,
    // Health Upgrades
    UPGRADE_HEALTH_1 = 1 << 17,
    UPGRADE_HEALTH_2 = 1 << 0,
    UPGRADE_HEALTH_3 = 1 << 1,
    UPGRADE_HEALTH_4 = 1 << 2,
    // Damage Upgrades
    UPGRADE_DAMAGE_1 = 1 << 3,
    UPGRADE_DAMAGE_2 = 1 << 4,
    UPGRADE_DAMAGE_3 = 1 << 5,
    UPGRADE_DAMAGE_4 = 1 << 6,
    // Regeneration Upgrades
    UPGRADE_REGENERATION_1 = 1 << 7,
    UPGRADE_REGENERATION_2 = 1 << 8,
    UPGRADE_REGENERATION_3 = 1 << 9,
    // New Room Upgrades
    UPGRADE_HEALING_ROOM = 1 << 10,
    UPGRADE_LOOT_ROOM,
    // MOVE SPEED UPGRADE
    UPGRADE_MOVE_SPEED_1 = 1 << 11,
    UPGRADE_MOVE_SPEED_2 = 1 << 12,
    // Dodge Upgrade
    UPGRADE_DASH_UNLOCK = 1 << 13,
    UPGRADE_DASH_COOLDOWN_1 = 1 << 14,
    UPGRADE_DASH_COOLDOWN_2 = 1 << 15,
    // Room Decrease Upgrade
    UPGRADE_REMOVE_ROOM = 1 << 16,
} PlayerUpgrades;

typedef struct {
    PlayerUpgrades playerUpgrades; // Bitmask.
    PlayerWeapon currentPlayerWeapon;
} PlayerHistory;

typedef struct {
    Vector2 position, size, movementVector;
    Color color;
    float rotation, moveSpeed, rotationSpeed, attackSpeed, attackCooldown;
    uint32_t maxHealth, currentHealth;
    float dashTimer, dashCooldown, dashCooldownTimer;
    uint32_t attackDamage;
    PlayerItem currentItems; // Bitmask.
    PlayerWeapon currentWeapon;
    PlayerUpgrades currentUpgrades;
} Player;

void drawPlayer(Player player);
void updatePlayer(Camera2D camera);
void damagePlayer(uint32_t damage);
void drawHealthBar(Vector2 size, Vector2 position, float healthPercent,
                   Color color);
void initPlayer(PlayerHistory history);

extern Player mainPlayer;