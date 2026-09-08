#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "projectile.h"
#include "enemies.h"
#include "levelManager.h"
#include "camera.h"
#include "audio.h"
#include "traps.h"
void updateGame(Camera2D *camera);
void initGame();

int main(void)
{
    // Init
    initGame();

    loadRoom("maps/thing/Level_0.ldtkl", &rooms[numRoomsLoaded], (Vector2){0, 0});
    mainPlayer.position = rooms[numRoomsLoaded].playerSpawn;
    numRoomsLoaded++;

    // Main Game Loop
    while (!WindowShouldClose())
    {
        // Update the game
        updateGame(&camera);
        // Render the game
        drawGame();
    }

    CloseWindow();
    return 0;
}

void initGame()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Stupid");
    SetTargetFPS(240);
    initSounds();
    initRoomTexture();
    initCamera();
}

void updateGame(Camera2D *camera)
{
    updatePlayer(*camera);
    updateCamera(camera);

    updateProjectiles();
    updateRooms();
    updateEnemies();
    updateSound();
    updateTraps();
}