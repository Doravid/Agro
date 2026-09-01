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

int main(void)
{
    // Init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Stupid");
    SetTargetFPS(0);
    initSounds();

    // User-controlled zoom multiplier

    Camera2D camera = {0};
    Shader bloom = LoadShader(0, "resources/bloom.fs");
    int sizeLoc = GetShaderLocation(bloom, "size");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);

    numRoomsLoaded = 0;
    loadRoom("maps/thing/Level_0.ldtkl", &rooms[numRoomsLoaded], (Vector2){0, 0});
    mainPlayer.position = rooms[numRoomsLoaded].playerSpawn;
    numRoomsLoaded++;

    // Main Game Loop
    while (!WindowShouldClose())
    {
        // Fullscreen Toggle
        if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER)))
        {
            int currentMonitor = GetCurrentMonitor();
            if (IsWindowFullscreen())
            {
                ToggleFullscreen();
                SetWindowSize(1280, 720);
            }
            else
            {
                SetWindowSize(GetMonitorWidth(currentMonitor), GetMonitorHeight(currentMonitor));
                ToggleFullscreen();
            }
        }
        if (IsWindowResized())
        {
            // Update the render texture.
            UnloadRenderTexture(target);
            target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
            SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);
            // Update the shader.
            float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
            SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);
        }
        updatePlayer(camera);
        // Modify base user zoom
        updateCamera(&camera);

        // Movement Updates
        updateProjectiles();
        updateRooms();
        updateEnemies();
        updateSound();

        // Render
        BeginTextureMode(target);

        ClearBackground(BLACK);

        BeginMode2D(camera);

        // Draw Calls
        drawProjectiles(projectiles, numProjectiles);
        drawPlayer(mainPlayer);
        drawEnemies();
        drawRooms();

        EndMode2D();
        EndTextureMode();

        // Draw The Game
        BeginDrawing();
        BeginShaderMode(bloom);
        DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Vector2){0, 0}, WHITE);
        EndShaderMode();
        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}