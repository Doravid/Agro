#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <stdio.h>

#include "audio.h"
#include "camera.h"
#include "enemies.h"
#include "levelManager.h"
#include "player.h"
#include "projectile.h"
#include "traps.h"
#include "ui.h"
void updateGame(Camera2D *camera);
void initGame();

int main(void) {
    // Init
    initGame();

    // Main Game Loop
    while (!WindowShouldClose()) {
        // Update the game
        updateGame(&camera);
        // Render the game
        drawGame();
    }

    CloseWindow();
    return 0;
}

void initGame() {
    initUi();
    SetTargetFPS(240);
    initSounds();
    initRoomTexture();
    initCamera();
    initPlayer((PlayerHistory){
        .currentPlayerWeapon = WEAPON_BIGSHOT,
        .playerUpgrades = NO_UPGRADES,
    });
}

void updateGame(Camera2D *camera) {
    updatePlayer(*camera);
    updateCamera(camera);
    updateProjectiles();
    updateRooms();
    updateEnemies();
    updateSound();
    updateTraps();
}