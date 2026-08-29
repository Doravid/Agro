
#include "camera.h"
#include "player.h"

static float shakeDuration = 0.0f;
static float shakeIntensity = 0.0f;

void triggerScreenShake(float duration, float intensity)
{
    shakeDuration = duration;
    shakeIntensity = intensity;
}

void updateCamera(Camera2D *camera, float *userZoom)
{
    *userZoom = expf(logf(*userZoom) + ((float)GetMouseWheelMove() * 0.1f));

    if (*userZoom > 3.0f)
        *userZoom = 3.0f;
    if (*userZoom < 0.1f)
        *userZoom = 0.1f;

    // FOV Scaling
    float scaleX = (float)GetScreenWidth() / VIRTUAL_WIDTH;
    float scaleY = (float)GetScreenHeight() / VIRTUAL_HEIGHT;
    float windowScale = fminf(scaleX, scaleY);
    camera->zoom = *userZoom * windowScale;
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