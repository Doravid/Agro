#pragma once
#include <raylib.h>
#define VIRTUAL_WIDTH (1920.0f);
#define VIRTUAL_HEIGHT (1080.0f);
extern Camera2D camera;

void triggerScreenShake(float duration, float intensity);
void initCamera();
void updateCamera(Camera2D *camera);
void drawGraphPaper(Camera2D camera, int screenWidth, int screenHeight);
void drawGame();
void myToggleFullscreen();
void updateScreen();