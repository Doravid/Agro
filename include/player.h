#pragma once

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

typedef struct
{
    Vector2 position, size, movementVector;
    Color color;
    float rotation, moveSpeed, rotationSpeed, attackSpeed, attackCooldown;
    uint32_t maxHealth, currentHealth;
    float dashTimer;
} Player;

void drawPlayer(Player player);
void updatePlayer(Camera2D camera);
void damagePlayer(uint32_t damage);

extern Player mainPlayer;