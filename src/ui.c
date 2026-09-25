#include <raylib.h>
#include "levelManager.h"
#include "ui.h"
#include "settings.h"
#define CLAY_IMPLEMENTATION

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include "clay.h"
#include "clay_renderer_raylib.c"
#pragma GCC diagnostic pop

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                 \
    (Clay_Vector2) { .x = vector.x, .y = vector.y }

Font grandover[2];

Clay_ElementDeclaration menuRootConfig = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_GROW(0),
                       .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                               .y = CLAY_ALIGN_Y_CENTER},
        },
    .backgroundColor = settingsBlack};

Clay_ElementDeclaration menuContainerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                          .height = CLAY_SIZING_FIT(0)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_TOP},
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childGap = 30,
               .padding = {40, 40, 40, 40}},
    .backgroundColor = {10, 10, 15, 255},
    .cornerRadius = CLAY_CORNER_RADIUS(16),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)}};

Clay_ElementDeclaration menuButtonConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_FIXED(80)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER}},
    .backgroundColor = toggleBackgroundColor,
    .cornerRadius = CLAY_CORNER_RADIUS(8),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)},
};

void RenderMenuButton(Clay_ElementId id, Clay_String labelText,
                      int actionType) {
    Clay_ElementDeclaration config = menuButtonConfig;
    if (Clay_PointerOver(id)) {
        config.backgroundColor = settingsOrange;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (actionType == 1)
                startGame();
            if (actionType == 2)
                currentState = STATE_SETTINGS;
        }
    }
    CLAY(id, config) {
        CLAY_TEXT(labelText,
                  CLAY_TEXT_CONFIG({.fontSize = 50, .textColor = textColor}));
    }
}

void initUi() {
    uint64_t clayMemorySize = Clay_MinMemorySize();
    Clay_Arena clayArena = Clay_CreateArenaWithCapacityAndMemory(
        clayMemorySize, malloc(clayMemorySize));
    Clay_Initialize(clayArena, (Clay_Dimensions){1920, 1080},
                    (Clay_ErrorHandler){0});

    Clay_Raylib_Initialize(1920, 1080, "Stupid",
                           FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

    grandover[0] = LoadFontEx("resources/Barlow-SemiBold.ttf", 100, 0, 400);
    SetTextureFilter(grandover[0].texture, TEXTURE_FILTER_BILINEAR);
    grandover[1] = LoadFontEx("resources/Barlow-SemiBold.ttf", 50, 0, 400);
    SetTextureFilter(grandover[1].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, grandover);
}

void renderMainMenu() {
    Clay_SetPointerState(RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition()),
                         IsMouseButtonDown(MOUSE_LEFT_BUTTON));
    Clay_SetLayoutDimensions(
        (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterBox"), menuRootConfig) {
        CLAY(CLAY_ID("ButtonContainer"), menuContainerConfig) {
            CLAY_TEXT(CLAY_STRING("MAIN MENU"),
                      CLAY_TEXT_CONFIG(
                          {.fontSize = 60, .textColor = settingsOrange}));
            RenderMenuButton(CLAY_ID("StartButton"), CLAY_STRING("Start"), 1);
            RenderMenuButton(CLAY_ID("UpgradesButton"), CLAY_STRING("Upgrades"),
                             0);
            RenderMenuButton(CLAY_ID("ConfigButton"), CLAY_STRING("Config"), 2);
            RenderMenuButton(CLAY_ID("ExitButton"), CLAY_STRING("Exit"), 0);
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout(GetFrameTime());
    Clay_Raylib_Render(renderCommands, grandover);
}

void renderSettingsMenu() { Clay_Raylib_Render(getSettingsMenu(), grandover); }