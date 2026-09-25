#pragma once
#include <clay.h>
#include <stdbool.h>

static const Clay_Color settingsOrange = {255, 80, 30, 255};
static const Clay_Color settingsBlack = {0, 0, 0, 255};
static const Clay_Color settingsTransparent = {0, 0, 0, 0};
static const Clay_Color textColor = {200, 200, 200, 255};
static const Clay_Color settingsBlue = {255, 255, 255, 255};
static const Clay_Color toggleBackgroundColor = {40, 40, 50, 255};

extern bool fullScreen;
extern float userScreenShake;
extern bool bloomEnabled;

Clay_RenderCommandArray getSettingsMenu();