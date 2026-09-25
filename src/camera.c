#include <stdio.h>

#include "camera.h"
#include "enemies.h"
#include "levelManager.h"
#include "player.h"
#include "projectile.h"
#include "traps.h"
#include "ui.h"
#include "settings.h"
#include "upgrades.h"

static float shakeDuration = 0.0f;
static float shakeIntensity = 0.0f;
Camera2D camera;
static Shader bloom;
static RenderTexture2D target;
static int sizeLoc;

void drawGraphPaper(Camera2D camera, int screenWidth, int screenHeight) {
    int spacing = 100;
    Color gridColor = (Color){17, 17, 17, 255};

    Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 bottomRight = GetScreenToWorld2D(
        (Vector2){(float)screenWidth, (float)screenHeight}, camera);

    int startX = (int)(topLeft.x / spacing) * spacing - spacing;
    int endX = (int)(bottomRight.x / spacing) * spacing + spacing;
    int startY = (int)(topLeft.y / spacing) * spacing - spacing;
    int endY = (int)(bottomRight.y / spacing) * spacing + spacing;

    for (int i = startX; i <= endX; i += spacing) {
        DrawLine(i, startY, i, endY, gridColor);
    }
    for (int i = startY; i <= endY; i += spacing) {
        DrawLine(startX, i, endX, i, gridColor);
    }
}

void triggerScreenShake(float duration, float intensity) {
    shakeDuration = duration;
    shakeIntensity = intensity * userScreenShake;
}

void initCamera() {
    bloom = LoadShader(0, "resources/bloom.fs");
    sizeLoc = GetShaderLocation(bloom, "size");
    float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);

    target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);
}
void myToggleFullscreen() {
    int currentMonitor = GetCurrentMonitor();
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
        SetWindowSize(1280, 720);
    } else {
        SetWindowSize(GetMonitorWidth(currentMonitor),
                      GetMonitorHeight(currentMonitor));
        ToggleFullscreen();
    }
    fullScreen = IsWindowFullscreen();
}
void updateScreen() {
    if (IsKeyPressed(KEY_F11)) {
        myToggleFullscreen();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (currentState == STATE_PLAYING || currentState == STATE_MAIN_MENU) {
            currentState = STATE_SETTINGS;
        } else if (currentState == STATE_SETTINGS) {
            if (getIsInGame()) {
                currentState = STATE_PLAYING;
            } else {
                currentState = STATE_MAIN_MENU;
            }
        } else if (currentState == STATE_SHOP) {
            currentState = STATE_MAIN_MENU;
        }
    }

    if (IsWindowResized()) {
        // Update the render texture.
        UnloadRenderTexture(target);
        target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        SetTextureWrap(target.texture, TEXTURE_WRAP_CLAMP);
        // Update the shader.
        float resolution[2] = {(float)GetScreenWidth(),
                               (float)GetScreenHeight()};
        SetShaderValue(bloom, sizeLoc, resolution, SHADER_UNIFORM_VEC2);
    }
}

void updateCamera(Camera2D *camera) {

    static float userZoom = 1.0f;
    userZoom = expf(logf(userZoom) + ((float)GetMouseWheelMove() * 0.1f));

    if (userZoom > 2.0f)
        userZoom = 2.0f;
    if (userZoom < 0.5f)
        userZoom = 0.6f;

    // FOV Scaling
    float scaleX = (float)GetScreenWidth() / VIRTUAL_WIDTH;
    float scaleY = (float)GetScreenHeight() / VIRTUAL_HEIGHT;
    float windowScale = fminf(scaleX, scaleY);
    camera->zoom = userZoom * windowScale;
    camera->offset =
        (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera->target = mainPlayer.position;
    camera->rotation = 0.0f;

    // Screen Shake

    if (shakeDuration > 0.f) {

        if (shakeDuration > 0.0f) {
            float offsetX =
                (GetRandomValue(-100, 100) / 100.0f) * shakeIntensity;
            float offsetY =
                (GetRandomValue(-100, 100) / 100.0f) * shakeIntensity;

            camera->offset.x += offsetX;
            camera->offset.y += offsetY;

            shakeDuration -= GetFrameTime();
            if (shakeDuration < 0.0f) {
                shakeDuration = 0.0f;
                shakeIntensity = 0.0f;
            }
        }
    }
}

void drawGame() {
    BeginTextureMode(target);

    ClearBackground((Color){.r = 7, .g = 7, .b = 7, .a = 255});
    char moneyString[100] = {0};
    snprintf(moneyString, 100, "%u$", mainPlayer.coins);
    if (currentState == STATE_PLAYING) {
        BeginMode2D(camera);
        drawProjectiles(projectiles, numProjectiles);
        drawPlayer(mainPlayer);
        drawEnemies();
        drawRooms();
        drawTraps();

        EndMode2D();

        DrawText(moneyString, GetScreenWidth() - GetScreenWidth() / 10,
                 GetScreenHeight() / 20, 40, ORANGE);
    }

    EndTextureMode();

    // Draw The Game
    BeginDrawing();
    BeginShaderMode(bloom);

    DrawTextureRec(target.texture,
                   (Rectangle){0, 0, (float)target.texture.width,
                               (float)-target.texture.height},
                   (Vector2){0, 0}, WHITE);
    EndShaderMode();

    // Draw Menus
    if (currentState == STATE_MAIN_MENU) {
        renderMainMenu();
    }
    if (currentState == STATE_SETTINGS) {
        renderSettingsMenu();
    }
    if (currentState == STATE_SHOP) {
        renderUpgradesMenu();
    }

    DrawFPS(10, 10);

    if (gameOver)
        DrawText("YOU WIN!", GetScreenWidth() / 4, GetScreenHeight() / 3, 150,
                 GOLD);
    if (mainPlayer.currentHealth == 0 && currentState == STATE_PLAYING)
        DrawText("YOU LOSE :(", GetScreenWidth() / 5, GetScreenHeight() / 4,
                 150, RED);
    EndDrawing();
}