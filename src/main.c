#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "player.h"
#include "projectile.h"
#include "enemies.h"

// Base virtual design resolution
const float VIRTUAL_WIDTH = 1920.0f;
const float VIRTUAL_HEIGHT = 1080.0f;

int main(void)
{
    // Init
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Stupid");
    SetTargetFPS(0);

    // User-controlled zoom multiplier
    float userZoom = 1.0f;

    Camera2D camera = {0};
    enemies[0] = (Player){.rotationSpeed = 50.f, .attackCooldown = 0., .attackSpeed = 0.2, .color = BLUE, .moveSpeed = 400.f, .position = {100, 50}, .rotation = 0, .size = {40.f, 40.f}, .maxHealth = 100, .currentHealth = 100};
    numEnemies = 1;

    Shader bloom = LoadShader(0, "resources/bloom.fs");
    int sizeLoc = GetShaderLocation(bloom, "size");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);
    RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);

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
        userZoom = expf(logf(userZoom) + ((float)GetMouseWheelMove() * 0.1f));
        if (userZoom > 3.0f)
            userZoom = 3.0f;
        if (userZoom < 0.1f)
            userZoom = 0.1f;

        // FOV Scaling
        float scaleX = (float)GetScreenWidth() / VIRTUAL_WIDTH;
        float scaleY = (float)GetScreenHeight() / VIRTUAL_HEIGHT;
        float windowScale = fminf(scaleX, scaleY);
        camera.zoom = userZoom * windowScale;
        camera.offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
        camera.target = mainPlayer.position;
        camera.rotation = 0.0f;

        // Movement Updates
        updateProjectiles();

        updateEnemies();

        // Render

        BeginTextureMode(target); // Enable drawing to texture

        ClearBackground(BLACK);

        BeginMode2D(camera);
        drawProjectiles(projectiles, numProjectiles);

        drawPlayer(mainPlayer);
        drawEnemies();

        EndMode2D();
        EndTextureMode();

        // Draw The Game
        BeginDrawing();
        BeginShaderMode(bloom);
        DrawTextureRec(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Vector2){0, 0}, WHITE);
        EndShaderMode();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}