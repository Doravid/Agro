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
void drawHealthBar(Vector2 size, Vector2 position, float healthPercent, Color color);

extern Player mainPlayer;