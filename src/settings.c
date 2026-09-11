#include <clay.h>
#include <raylib.h>

#include "ui.h"

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                 \
  (Clay_Vector2) { .x = vector.x, .y = vector.y }

// Element Representing the Entire Screen.

Clay_RenderCommandArray getSettingsMenu() {

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
      CLAY(CLAY_ID("InnerBox"), getInnerConfig(Clay_Hovered())) {}
    }
  }

  return Clay_EndLayout(GetFrameTime());
}