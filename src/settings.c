#include <clay.h>
#include <raylib.h>

#include "ui.h"

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                 \
  (Clay_Vector2) { .x = vector.x, .y = vector.y }

// Element Representing the Entire Screen.

Clay_ElementDeclaration settingsRootConfig = {
    .layout = {
        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
        .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
    }};

Clay_ElementDeclaration settingsContainerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                          .height = CLAY_SIZING_PERCENT(.8)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER},
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childGap = 15,
               .padding = {15, 15, 15, 15}},
    .backgroundColor = {90, 70, 40, 255},
    .cornerRadius = CLAY_CORNER_RADIUS(12),
    .border = {.color = {70, 50, 20, 255}, .width = CLAY_BORDER_OUTSIDE(3)},
};

Clay_ElementDeclaration line = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_GROW(),
                       .height = CLAY_SIZING_FIXED(8.f)},
            .childAlignment = {.x = CLAY_ALIGN_X_LEFT,
                               .y = CLAY_ALIGN_Y_CENTER},
        },
    .backgroundColor = (Clay_Color){0, 0, 0, 255},
};

Clay_ElementDeclaration handle = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_FIXED(50),
                       .height = CLAY_SIZING_FIXED(30.f)},
        },
    .backgroundColor = (Clay_Color){100, 0, 0, 255},
    .border = {.color = {90, 20, 20, 255}, .width = CLAY_BORDER_OUTSIDE(2)},
    .cornerRadius = CLAY_CORNER_RADIUS(12),
    .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                 .attachPoints = {.element = CLAY_ATTACH_POINT_LEFT_CENTER,
                                  .parent = CLAY_ATTACH_POINT_LEFT_CENTER}}};

Clay_ElementDeclaration sliderHolder = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_PERCENT(1.f),
                       .height = CLAY_SIZING_FIT(10)},
            .padding = {.right = 10, .left = 10, .top = 10, .bottom = 10},
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                               .y = CLAY_ALIGN_Y_CENTER},
        },
    .backgroundColor = (Clay_Color){100, 0, 100, 0},
};

float sliderValue = 0.0f;

Clay_RenderCommandArray getSettingsMenu() {

  Clay_SetPointerState(RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition()),
                       IsMouseButtonDown(0));
  Clay_SetLayoutDimensions(
      (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});

  Vector2 mousePos = GetMousePosition();
  Clay_SetPointerState((Clay_Vector2){mousePos.x, mousePos.y},
                       IsMouseButtonDown(MOUSE_LEFT_BUTTON));

  Clay_BeginLayout();

  CLAY(CLAY_ID("OuterBox"), settingsRootConfig) {
    CLAY(CLAY_ID("ButtonContainer"), settingsContainerConfig) {
      CLAY(CLAY_ID("Slider"), sliderHolder) {
        CLAY_TEXT(CLAY_STRING("Volume"),
                  CLAY_TEXT_CONFIG(
                      {.fontSize = 100, .textColor = {200, 200, 200, 255}}));
        CLAY(CLAY_ID("InnerBox"), line) {
          float innerWidth =
              Clay_GetElementData(CLAY_ID("InnerBox")).boundingBox.width;
          float maxOffset = innerWidth - 50.0f;
          if (maxOffset < 0.0f)
            maxOffset = 0.0f;

          Clay_ElementDeclaration activeHandle = handle;
          activeHandle.floating.offset.x = sliderValue * maxOffset;

          CLAY(CLAY_ID("Handle"), activeHandle) {}
        }
      }
    }
  }

  return Clay_EndLayout(GetFrameTime());
}