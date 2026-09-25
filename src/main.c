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
    SetTargetFPS(0);
    SetExitKey(KEY_DELETE);
    initSounds();
    initRoomTexture();
    initCamera();
    initPlayer((PlayerHistory){
        .currentPlayerWeapon = WEAPON_BIGSHOT,
        .playerUpgrades = UPGRADE_DASH_UNLOCK,
    });
}

void updateGame(Camera2D *camera) {
    updateScreen();
    updateSound();

    if (currentState != STATE_PLAYING) {
        return;
    }
    updatePlayer(*camera);
    updateCamera(camera);
    updateProjectiles();
    updateRooms();
    updateEnemies();
    updateTraps();
}