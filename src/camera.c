
#include "camera.h"
#include "player.h"

static float shakeDuration = 0.0f;
static float shakeIntensity = 0.0f;

void drawGraphPaper(Camera2D camera, int screenWidth, int screenHeight)
{
    int spacing = 100;
    Color gridColor = (Color){17, 17, 17, 255};

    Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 bottomRight = GetScreenToWorld2D((Vector2){(float)screenWidth, (float)screenHeight}, camera);

    int startX = (int)(topLeft.x / spacing) * spacing - spacing;
    int endX = (int)(bottomRight.x / spacing) * spacing + spacing;
    int startY = (int)(topLeft.y / spacing) * spacing - spacing;
    int endY = (int)(bottomRight.y / spacing) * spacing + spacing;

    for (int i = startX; i <= endX; i += spacing)
    {
        DrawLine(i, startY, i, endY, gridColor);
    }
    for (int i = startY; i <= endY; i += spacing)
    {
        DrawLine(startX, i, endX, i, gridColor);
    }
}

void triggerScreenShake(float duration, float intensity)
{
    shakeDuration = duration;
    shakeIntensity = intensity;
}

void updateCamera(Camera2D *camera)
{
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
    camera->offset = (Vector2){GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f};
    camera->target = mainPlayer.position;
    camera->rotation = 0.0f;

    // Screen Shake

    if (shakeDuration > 0.f)
    {

        if (shakeDuration > 0.0f)
        {
            float offsetX = (GetRandomValue(-100, 100) / 100.0f) * shakeIntensity;
            float offsetY = (GetRandomValue(-100, 100) / 100.0f) * shakeIntensity;

            camera->offset.x += offsetX;
            camera->offset.y += offsetY;

            shakeDuration -= GetFrameTime();
            if (shakeDuration < 0.0f)
            {
                shakeDuration = 0.0f;
                shakeIntensity = 0.0f;
            }
        }
    }
}