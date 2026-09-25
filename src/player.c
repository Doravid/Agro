#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "projectile.h"
#include "audio.h"
#include "camera.h"
#include "levelManager.h"

Player mainPlayer;

static void initShooter(PlayerUpgrades upgrades) {
    mainPlayer = (Player){
        .maxHealth = 100,
        .currentHealth = 100,
        .position = {400.0f, 200.0f},
        .size = {40.0f, 40.0f},
        .rotation = -90.f,
        .rotationSpeed = 300.f,
        .moveSpeed = 400.0f,
        .color = GREEN,
        .movementVector = (Vector2){0},
        .dashTimer = 0.0f,
        .attackDamage = 15,
        .attackCooldown = 0.1f,
        .attackSpeed = 0.3,
        .currentWeapon = WEAPON_SHOOTER,
    };
}

static void initBigshot(PlayerUpgrades upgrades) {
    mainPlayer = (Player){
        .maxHealth = 50,
        .currentHealth = 50,
        .position = {400.0f, 200.0f},
        .size = {60.0f, 60.0f},
        .rotation = -90.f,
        .rotationSpeed = 100.f,
        .moveSpeed = 150.0f,
        .color = ORANGE,
        .movementVector = (Vector2){0},
        .dashTimer = 0.0f,
        .attackDamage = 75,
        .attackCooldown = 0.1f,
        .attackSpeed = 1.0,
        .currentWeapon = WEAPON_BIGSHOT,
    };
}
static void applyPlayerUpgrades(PlayerUpgrades upgrades) {
    // Tell the player what upgrades it has.
    mainPlayer.currentUpgrades = upgrades;
    // For now we will just modify the values directly. In the future I way want
    // to just adjust the attack damage when we actually attack given the
    // current upgrades

    // Damage Upgrades
    if (upgrades & UPGRADE_DAMAGE_4) {
        mainPlayer.attackDamage *= 2.00;
    } else if (upgrades & UPGRADE_DAMAGE_3) {
        mainPlayer.attackDamage *= 1.75;
    } else if (upgrades & UPGRADE_DAMAGE_2) {
        mainPlayer.attackDamage *= 1.50;
    } else if (upgrades & UPGRADE_DAMAGE_1) {
        mainPlayer.attackDamage *= 1.25;
    }

    // Health Upgrades
    if (upgrades & UPGRADE_HEALTH_4) {
        mainPlayer.maxHealth *= 2.00;
    } else if (upgrades & UPGRADE_HEALTH_3) {
        mainPlayer.maxHealth *= 1.75;
    } else if (upgrades & UPGRADE_HEALTH_2) {
        mainPlayer.maxHealth *= 1.50;
    } else if (upgrades & UPGRADE_HEALTH_1) {
        mainPlayer.maxHealth *= 1.25;
    }
    mainPlayer.currentHealth = mainPlayer.maxHealth;

    // Dash Upgrades
    if (upgrades & UPGRADE_DASH_COOLDOWN_2) {
        mainPlayer.dashCooldown *= 0.5;
    } else if (upgrades & UPGRADE_DASH_COOLDOWN_1) {
        mainPlayer.dashCooldown *= 0.75;
    }

    // Move Speed Upgrades
    if (upgrades & UPGRADE_MOVE_SPEED_2) {
        mainPlayer.moveSpeed *= 1.25;
    } else if (upgrades & UPGRADE_MOVE_SPEED_1) {
        mainPlayer.moveSpeed *= 1.50;
    }
}

void initPlayer(PlayerHistory history) {
    switch (history.currentPlayerWeapon) {
        case WEAPON_SHOOTER:
            initShooter(history.playerUpgrades);
            break;
        case WEAPON_FLAME:
            puts("TODO");
            break;
        case WEAPON_KNIGHT:
            puts("TODO");
            break;
        case WEAPON_BIGSHOT:
            initBigshot(history.playerUpgrades);
            break;

        default:
            puts("ERROR: THIS SHOULD NOT BE POSSIBLE, INIT PLAYER");
            break;
    }
    applyPlayerUpgrades(history.playerUpgrades);
}

void drawHealthBar(Vector2 size, Vector2 position, float healthPercent,
                   Color color) {
    Vector2 playerOrigin = {size.x * 0.5f, size.y * 0.5f};
    const float healthBarStart = (position.x - size.x / 4);
    const float healthBarMaxWidth = size.x * 1.5;
    Rectangle healthRec = {
        .x = healthBarStart,
        .y = position.y + size.y * 1.5,
        .width = healthBarMaxWidth * healthPercent,
        .height = size.y / 10.0,
    };
    Rectangle redHealthRec = {
        .x = healthBarStart + healthRec.width,
        .y = position.y + size.y * 1.5,
        .width = healthBarMaxWidth - healthBarMaxWidth * healthPercent,
        .height = size.y / 10.0,
    };
    DrawRectanglePro(healthRec, playerOrigin, 0, color);
    DrawRectanglePro(redHealthRec, playerOrigin, 0, RED);
}

void drawShooterPlayer(Player player) {
    const float fadeAmount = 0.3f;
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {20.0f, 15.0f};

    Vector2 rotatedOffset =
        Vector2Rotate(childOffset, player.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(player.position, rotatedOffset);

    Color barrelDrawColor =
        player.dashTimer <= 0.f ? DARKPURPLE : Fade(DARKPURPLE, fadeAmount);
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x,
                          childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, player.rotation, barrelDrawColor);

    Color playerDrawColor =
        player.dashTimer <= 0.f ? player.color : Fade(player.color, fadeAmount);

    Rectangle playerRec = {player.position.x, player.position.y, player.size.x,
                           player.size.y};
    Vector2 playerOrigin = {player.size.x * 0.5f, player.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, player.rotation, playerDrawColor);

    drawHealthBar(player.size, player.position,
                  (float)player.currentHealth / player.maxHealth, player.color);
}

void drawBigshot(Player player) {
    const float fadeAmount = 0.3f;
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {40.0f, 45.0f};

    Vector2 rotatedOffset =
        Vector2Rotate(childOffset, player.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(player.position, rotatedOffset);

    Color barrelDrawColor =
        player.dashTimer <= 0.f ? DARKPURPLE : Fade(DARKPURPLE, fadeAmount);
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x,
                          childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, player.rotation, barrelDrawColor);

    Color playerDrawColor =
        player.dashTimer <= 0.f ? player.color : Fade(player.color, fadeAmount);

    Rectangle playerRec = {player.position.x, player.position.y, player.size.x,
                           player.size.y};
    Vector2 playerOrigin = {player.size.x * 0.5f, player.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, player.rotation, playerDrawColor);

    drawHealthBar(player.size, player.position,
                  (float)player.currentHealth / player.maxHealth, player.color);
}

void drawPlayer(Player player) {
    switch (player.currentWeapon) {
        case WEAPON_SHOOTER:
            drawShooterPlayer(player);
            break;
        case WEAPON_FLAME:
            puts("Unimplemented");
            break;
        case WEAPON_KNIGHT:
            puts("Unimplemented");
            break;
        case WEAPON_BIGSHOT:
            drawBigshot(player);
            break;
        default:
            puts("CRITICAL ERROR, VALUE SHOULD BE IMPOSSIBLE");
            break;
    }
}

static bool upgradeObtained(PlayerUpgrades upgrade) {
    return mainPlayer.currentUpgrades & upgrade;
}

static void startPlayerDash(Player *player) {
    if (player->dashTimer > 0.f ||
        Vector2LengthSqr(player->movementVector) <= 0)
        return;
    PlaySound(dash);
    player->dashTimer = 0.15f;
}

static bool updatePlayerDash() {
    if (mainPlayer.dashTimer > 0.f) {
        mainPlayer.dashTimer -= GetFrameTime();

        float currentDashSpeed = mainPlayer.moveSpeed * 3.5f;
        Vector2 offset = Vector2Scale(mainPlayer.movementVector,
                                      currentDashSpeed * GetFrameTime());
        mainPlayer.position =
            moveWithCollision(mainPlayer.position, mainPlayer.size, offset);
        return true;
    }
    return false;
}

static void handleShooterAttack() {
    if (mainPlayer.attackCooldown > 0.f) {
        mainPlayer.attackCooldown -= GetFrameTime();
        return;
    }

    if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        PlaySound(shoot);
        spawnProjectileFromPlayer(mainPlayer, PlayerProj);
        mainPlayer.attackCooldown += mainPlayer.attackSpeed;
    }
}

static void handleBigshotAttack() {
    if (mainPlayer.attackCooldown > 0.f) {
        mainPlayer.attackCooldown -= GetFrameTime();
        return;
    }

    if (IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        PlaySound(shoot);

        Vector2 directionVector = {.x = cosf(mainPlayer.rotation * DEG2RAD),
                                   .y = sinf(mainPlayer.rotation * DEG2RAD)};

        spawnProjectile((Projectile){
            .color = ColorLerp(PINK, mainPlayer.color, 0.5),
            .direction = directionVector,
            .position = {mainPlayer.position.x + directionVector.x * 40,
                         mainPlayer.position.y + directionVector.y * 40},
            .size = 20.f,
            .moveSpeed = 150.f,
            .damage = mainPlayer.attackDamage,
            .owner = PlayerProj,
            .lifetime = 50.f,
        });

        mainPlayer.attackCooldown += mainPlayer.attackSpeed;
        triggerScreenShake(0.25, 4.5f);
    }
}

void updatePlayer(Camera2D camera) {
    if (mainPlayer.currentHealth == 0)
        return;
    // Input & Movement

    // X
    if (IsKeyDown(KEY_D))
        mainPlayer.movementVector.x = 1;
    else if (IsKeyDown(KEY_A))
        mainPlayer.movementVector.x = -1;
    else
        mainPlayer.movementVector.x = 0;
    // Y
    if (IsKeyDown(KEY_W))
        mainPlayer.movementVector.y = -1;
    else if (IsKeyDown(KEY_S))
        mainPlayer.movementVector.y = 1;
    else
        mainPlayer.movementVector.y = 0;

    // Handle the Dashing Logic.
    if (IsKeyPressed(KEY_LEFT_SHIFT) && upgradeObtained(UPGRADE_DASH_UNLOCK)) {
        startPlayerDash(&mainPlayer);
    }
    if (updatePlayerDash())
        return;

    switch (mainPlayer.currentWeapon) {
        case WEAPON_SHOOTER:
            handleShooterAttack();
            break;
        case WEAPON_FLAME:
            puts("TODO");
            break;
        case WEAPON_KNIGHT:
            puts("TODO");
            break;
        case WEAPON_BIGSHOT:
            handleBigshotAttack();
            break;
        default:
            puts("ERROR: State should not be possible.");
            break;
    }

    // Movement
    Vector2 norm = Vector2Normalize(mainPlayer.movementVector);
    Vector2 offset = Vector2Scale(norm, GetFrameTime() * mainPlayer.moveSpeed);
    mainPlayer.position =
        moveWithCollision(mainPlayer.position, mainPlayer.size, offset);

    // Rotate the Player
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
    float ang = atan2f(mousePos.y - mainPlayer.position.y,
                       mousePos.x - mainPlayer.position.x);
    float enemyAngle = mainPlayer.rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));
    if (fabs(delta) < 0.02f)
        return;

    if (delta < 0)
        mainPlayer.rotation += mainPlayer.rotationSpeed * GetFrameTime();
    else
        mainPlayer.rotation -= mainPlayer.rotationSpeed * GetFrameTime();
}

void damagePlayer(uint32_t damage) {
    if (damage > mainPlayer.currentHealth)
        mainPlayer.currentHealth = 0;
    else
        mainPlayer.currentHealth -= damage;
    triggerScreenShake(0.25, 4.5f);
    PlaySound(hit);
}
