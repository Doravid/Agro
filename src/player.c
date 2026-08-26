#include <raylib.h>
#include <raymath.h>

#include "player.h"
#include "projectile.h"

Player mainPlayer = {
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
};

/**
 * @brief Draws a player
 *
 * @param player the player that will be drawn.
 */
void drawPlayer(Player player)
{
    const float fadeAmount = 0.3f;
    // Draw the Barrel
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {20.0f, 15.0f};

    Vector2 rotatedOffset = Vector2Rotate(childOffset, player.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(player.position, rotatedOffset);

    Color barrelDrawColor = player.dashTimer <= 0.f ? DARKPURPLE : Fade(DARKPURPLE, fadeAmount);
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x, childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, player.rotation, barrelDrawColor);

    // Draw the Player Cube
    Color playerDrawColor = player.dashTimer <= 0.f ? player.color : Fade(player.color, fadeAmount);

    Rectangle playerRec = {player.position.x, player.position.y, player.size.x, player.size.y};
    Vector2 playerOrigin = {player.size.x * 0.5f, player.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, player.rotation, playerDrawColor);

    // Draw the Health Bar
    const float healthBarStart = (player.position.x - player.size.x / 4);
    const float healthBarMaxWidth = player.size.x * 1.5;
    Rectangle healthRec = {
        .x = healthBarStart,
        .y = player.position.y + 49.,
        .width = healthBarMaxWidth * ((float)player.currentHealth / player.maxHealth),
        .height = player.size.y / 10.0,
    };
    Rectangle redHealthRec = {
        .x = healthBarStart + healthRec.width,
        .y = player.position.y + 49.,
        .width = healthBarMaxWidth - healthBarMaxWidth * ((float)player.currentHealth / player.maxHealth),
        .height = player.size.y / 10.0,
    };
    DrawRectanglePro(healthRec, playerOrigin, 0, player.color);
    DrawRectanglePro(redHealthRec, playerOrigin, 0, RED);
}
void dashPlayer(Player *player)
{
    if (player->dashTimer > 0.f || Vector2LengthSqr(player->movementVector) <= 0)
        return;
    player->dashTimer = 0.15f;
}
void updatePlayer(Camera2D camera)
{
    // Input & Movement
    if (IsKeyDown(KEY_D))
        mainPlayer.movementVector.x = 1;
    else if (IsKeyDown(KEY_A))
        mainPlayer.movementVector.x = -1;
    else
        mainPlayer.movementVector.x = 0;
    if (IsKeyDown(KEY_W))
        mainPlayer.movementVector.y = -1;
    else if (IsKeyDown(KEY_S))
        mainPlayer.movementVector.y = 1;
    else
        mainPlayer.movementVector.y = 0;

    if (IsKeyDown(KEY_Q))
        mainPlayer.rotation -= mainPlayer.rotationSpeed * GetFrameTime();
    if (IsKeyDown(KEY_E))
        mainPlayer.rotation += mainPlayer.rotationSpeed * GetFrameTime();
    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        spawnProjectileFromPlayer(mainPlayer, PlayerProj);
    if (IsKeyPressed(KEY_LEFT_SHIFT))
        dashPlayer(&mainPlayer);

    if (mainPlayer.dashTimer > 0.f)
    {
        mainPlayer.dashTimer -= GetFrameTime();

        float currentDashSpeed = mainPlayer.moveSpeed * 3.5f;
        Vector2 offset = Vector2Scale(mainPlayer.movementVector, currentDashSpeed * GetFrameTime());
        mainPlayer.position = Vector2Add(mainPlayer.position, offset);
        return;
    }
    // Move
    Vector2 norm = Vector2Normalize(mainPlayer.movementVector);
    Vector2 offset = Vector2Scale(norm, GetFrameTime() * mainPlayer.moveSpeed);
    mainPlayer.position = Vector2Add(mainPlayer.position, offset);

    // Rotate
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
    float ang = atan2f(mousePos.y - mainPlayer.position.y, mousePos.x - mainPlayer.position.x);
    float enemyAngle = mainPlayer.rotation * DEG2RAD;
    float delta = enemyAngle - ang;
    delta = atan2f(sinf(delta), cosf(delta));

    if (delta < 0)
        mainPlayer.rotation += mainPlayer.rotationSpeed * GetFrameTime();
    else
        mainPlayer.rotation -= mainPlayer.rotationSpeed * GetFrameTime();
}

void damagePlayer(uint32_t damage)
{
    mainPlayer.currentHealth -= damage;
}