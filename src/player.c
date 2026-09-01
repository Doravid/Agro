#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <stdio.h>

#include "player.h"
#include "projectile.h"
#include "audio.h"
#include "camera.h"
#include "levelManager.h"

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
    .attackDamage = 15,
};

void drawHealthBar(Vector2 size, Vector2 position, float healthPercent, Color color)
{
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

void drawPlayer(Player player)
{
    const float fadeAmount = 0.3f;
    Vector2 childOffset = {30.0f, 0.0f};
    Vector2 childSize = {20.0f, 15.0f};

    Vector2 rotatedOffset = Vector2Rotate(childOffset, player.rotation * DEG2RAD);
    Vector2 childWorldPos = Vector2Add(player.position, rotatedOffset);

    Color barrelDrawColor = player.dashTimer <= 0.f ? DARKPURPLE : Fade(DARKPURPLE, fadeAmount);
    Rectangle childRec = {childWorldPos.x, childWorldPos.y, childSize.x, childSize.y};
    Vector2 childOrigin = {childSize.x * 0.5f, childSize.y * 0.5f};
    DrawRectanglePro(childRec, childOrigin, player.rotation, barrelDrawColor);

    Color playerDrawColor = player.dashTimer <= 0.f ? player.color : Fade(player.color, fadeAmount);

    Rectangle playerRec = {player.position.x, player.position.y, player.size.x, player.size.y};
    Vector2 playerOrigin = {player.size.x * 0.5f, player.size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, player.rotation, playerDrawColor);

    drawHealthBar(player.size, player.position, (float)player.currentHealth / player.maxHealth, player.color);
}
void dashPlayer(Player *player)
{
    if (player->dashTimer > 0.f || Vector2LengthSqr(player->movementVector) <= 0)
        return;
    PlaySound(dash);
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

    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        PlaySound(shoot);
        spawnProjectileFromPlayer(mainPlayer, PlayerProj);
    }

    if (IsKeyPressed(KEY_LEFT_SHIFT))
        dashPlayer(&mainPlayer);

    if (mainPlayer.dashTimer > 0.f)
    {
        mainPlayer.dashTimer -= GetFrameTime();

        float currentDashSpeed = mainPlayer.moveSpeed * 3.5f;
        Vector2 offset = Vector2Scale(mainPlayer.movementVector, currentDashSpeed * GetFrameTime());
        mainPlayer.position = moveWithCollision(mainPlayer.position, mainPlayer.size, offset);
        return;
    }
    // Move
    Vector2 norm = Vector2Normalize(mainPlayer.movementVector);
    Vector2 offset = Vector2Scale(norm, GetFrameTime() * mainPlayer.moveSpeed);
    mainPlayer.position = moveWithCollision(mainPlayer.position, mainPlayer.size, offset);

    // Rotate
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
    float ang = atan2f(mousePos.y - mainPlayer.position.y, mousePos.x - mainPlayer.position.x);
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

void damagePlayer(uint32_t damage)
{
    if (damage > mainPlayer.currentHealth)
        mainPlayer.currentHealth = 0;
    else
        mainPlayer.currentHealth -= damage;
    triggerScreenShake(0.25, 4.5f);
    PlaySound(hit);
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
                puts("HELLO");
                Vector2 targetEntrance = {lastRoom->colliders[i].bounds.x, lastRoom->colliders[i].bounds.y};

                const char *nextMaps[] = {"maps/thing/Level_1.ldtkl", "maps/thing/Level_1.ldtkl"};
                int randIndex = GetRandomValue(0, 1);

                loadRoom(nextMaps[randIndex], &rooms[numRoomsLoaded], targetEntrance);
                numRoomsLoaded++;
                break;
            }
        }
    }
}