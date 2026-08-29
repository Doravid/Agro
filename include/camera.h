#pragma once
#include <raylib.h>
#define VIRTUAL_WIDTH (1920.0f);
#define VIRTUAL_HEIGHT (1080.0f);

void triggerScreenShake(float duration, float intensity);
void updateCamera(Camera2D *camera, float *userZoom);