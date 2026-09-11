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

void initUi() {
  uint64_t clayMemorySize = Clay_MinMemorySize();
  Clay_Arena clayArena = Clay_CreateArenaWithCapacityAndMemory(
      clayMemorySize, malloc(clayMemorySize));
  Clay_Initialize(clayArena, (Clay_Dimensions){1920, 1080},
                  (Clay_ErrorHandler){0});

  Clay_Raylib_Initialize(1920, 1080, "Stupid",
                         FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

  grandover[0] = LoadFontEx("resources/Grandover.ttf", 100, 0, 400);
  SetTextureFilter(grandover[0].texture, TEXTURE_FILTER_BILINEAR);
  grandover[1] = LoadFontEx("resources/Grandover.ttf", 50, 0, 400);
  SetTextureFilter(grandover[1].texture, TEXTURE_FILTER_BILINEAR);
  Clay_SetMeasureTextFunction(Raylib_MeasureText, grandover);
}

void startButtonHover(Clay_ElementId elementId, Clay_PointerData pointerData,
                      void *userData) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    startGame();
}

void settingsButtonHover(Clay_ElementId elementId, Clay_PointerData pointerData,
                         void *userData) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    currentState = STATE_SETTINGS;
}

Clay_ElementDeclaration getInnerConfig(bool hovered) {
  return (Clay_ElementDeclaration){
      .layout =
          {
              .sizing = {.width = CLAY_SIZING_PERCENT(1.f),
                         .height = CLAY_SIZING_PERCENT(0.25)},
              .padding = {.right = 10, .left = 10, .top = 10, .bottom = 10},
              .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                 .y = CLAY_ALIGN_Y_CENTER},
          },
      .backgroundColor = hovered ? (Clay_Color){220, 120, 80, 255}
                                 : (Clay_Color){180, 80, 50, 255},
      .cornerRadius = CLAY_CORNER_RADIUS(12),
      .border = {.color = {240, 220, 220, 255},
                 .width = CLAY_BORDER_OUTSIDE(2)},

  };
}

Clay_ElementDeclaration rootConfig = {
    .layout = {
        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
        .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
    }};

Clay_ElementDeclaration containerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                          .height = CLAY_SIZING_PERCENT(.8)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER},
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childGap = 15,
               .padding = {15, 15, 15, 15}},
    .backgroundColor = {20, 20, 20, 255},
    .cornerRadius = CLAY_CORNER_RADIUS(12),
    .border = {.color = {240, 220, 220, 255}, .width = CLAY_BORDER_OUTSIDE(2)}};

void renderMainMenu() {

  Clay_SetPointerState(RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition()),
                       IsMouseButtonDown(0));
  Clay_SetLayoutDimensions(
      (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});

  Vector2 mousePos = GetMousePosition();
  Clay_SetPointerState((Clay_Vector2){mousePos.x, mousePos.y},
                       IsMouseButtonDown(MOUSE_LEFT_BUTTON));

  Clay_BeginLayout();

  CLAY(CLAY_ID("OuterBox"), rootConfig) {
    CLAY(CLAY_ID("ButtonContainer"), containerConfig) {
      CLAY(CLAY_ID("InnerBox"), getInnerConfig(Clay_Hovered())) {
        Clay_OnHover(startButtonHover, 0);
        CLAY_TEXT(CLAY_STRING("Start"),
                  CLAY_TEXT_CONFIG(
                      {.fontSize = 100, .textColor = {255, 255, 255, 255}}));
      }
      CLAY(CLAY_ID("InnerBox2"), getInnerConfig(Clay_Hovered())) {
        CLAY_TEXT(CLAY_STRING("Upgrades"),
                  CLAY_TEXT_CONFIG(
                      {.fontSize = 100, .textColor = {255, 255, 255, 255}}));
      }
      CLAY(CLAY_ID("InnerBox3"), getInnerConfig(Clay_Hovered())) {
        Clay_OnHover(settingsButtonHover, 0);
        CLAY_TEXT(CLAY_STRING("Config"),
                  CLAY_TEXT_CONFIG(
                      {.fontSize = 100, .textColor = {255, 255, 255, 255}}));
      }
      CLAY(CLAY_ID("InnerBox4"), getInnerConfig(Clay_Hovered())) {
        CLAY_TEXT(CLAY_STRING("Exit"),
                  CLAY_TEXT_CONFIG(
                      {.fontSize = 100, .textColor = {255, 0, 12, 255}}));
      }
    }
  }

  Clay_RenderCommandArray renderCommands = Clay_EndLayout(GetFrameTime());
  Clay_Raylib_Render(renderCommands, grandover);
}

void renderSettingsMenu() { Clay_Raylib_Render(getSettingsMenu(), grandover); }