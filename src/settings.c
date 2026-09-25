#include <clay.h>
#include <raylib.h>
#include "settings.h"
#include "levelManager.h"

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                 \
    (Clay_Vector2) { .x = vector.x, .y = vector.y }

float musicVolume = 1.0f;
float sfxVolume = 1.0f;
bool fullScreen = false;
float screenShake = 0.5f;
bool bloomEnabled = true;
bool backClicked = false;

// Element Representing the Entire Screen.
Clay_ElementDeclaration settingsRootConfig = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_GROW(0),
                       .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                               .y = CLAY_ALIGN_Y_CENTER},
        },
    .backgroundColor = settingsBlack};

Clay_ElementDeclaration settingsContainerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.6),
                          .height = CLAY_SIZING_FIT(0)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_TOP},
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childGap = 30,
               .padding = {40, 40, 40, 40}},
    .backgroundColor = {10, 10, 15, 255},
    .cornerRadius = CLAY_CORNER_RADIUS(16),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)},
};

Clay_ElementDeclaration rowConfig = {
    .layout = {
        .sizing = {.width = CLAY_SIZING_GROW(0),
                   .height = CLAY_SIZING_FIXED(40)},
        .childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER},
        .childGap = 0,
        .layoutDirection = CLAY_LEFT_TO_RIGHT}};

Clay_ElementDeclaration labelConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                          .height = CLAY_SIZING_FIT(0)}}};

Clay_ElementDeclaration line = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_GROW(0),
                       .height = CLAY_SIZING_FIXED(4.f)},
            .childAlignment = {.x = CLAY_ALIGN_X_LEFT,
                               .y = CLAY_ALIGN_Y_CENTER},
        },
    .backgroundColor = settingsBlue,
};

Clay_ElementDeclaration handle = {
    .layout = {.sizing = {.width = CLAY_SIZING_FIXED(20),
                          .height = CLAY_SIZING_FIXED(30.f)}},
    .backgroundColor = settingsOrange,
    .cornerRadius = CLAY_CORNER_RADIUS(4),
    .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                 .attachPoints = {.element = CLAY_ATTACH_POINT_CENTER_CENTER,
                                  .parent = CLAY_ATTACH_POINT_LEFT_CENTER}}};

Clay_ElementDeclaration sliderHolder = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_GROW(0),
                       .height = CLAY_SIZING_GROW(0)},
            .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                               .y = CLAY_ALIGN_Y_CENTER},
        },
    .backgroundColor = settingsTransparent,
};

Clay_ElementDeclaration toggleBox = {
    .layout = {.sizing = {.width = CLAY_SIZING_FIXED(30),
                          .height = CLAY_SIZING_FIXED(30)}},
    .backgroundColor = toggleBackgroundColor,
    .cornerRadius = CLAY_CORNER_RADIUS(2),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)}};

Clay_ElementDeclaration toggleKnob = {
    .layout = {.sizing = {.width = CLAY_SIZING_FIXED(22),
                          .height = CLAY_SIZING_FIXED(22)}},
    .cornerRadius = CLAY_CORNER_RADIUS(2),
    .floating = {.attachTo = CLAY_ATTACH_TO_PARENT,
                 .offset = {.x = 4, .y = 4}}};

Clay_ElementDeclaration backButtonConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_FIXED(50)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER}},
    .backgroundColor = toggleBackgroundColor,
    .cornerRadius = CLAY_CORNER_RADIUS(8),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)},
};

void RenderSlider(int index, Clay_ElementId id, Clay_String labelText,
                  float *value) {
    CLAY(CLAY_IDI("Row", index), rowConfig) {
        CLAY(CLAY_IDI("LabelContainer", index), labelConfig) {
            CLAY_TEXT(labelText, CLAY_TEXT_CONFIG(
                                     {.fontSize = 40, .textColor = textColor}));
        }
        CLAY(id, sliderHolder) {
            if (Clay_PointerOver(id) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                Clay_ElementData data = Clay_GetElementData(id);
                if (data.boundingBox.width > 0) {
                    float relX = GetMousePosition().x - data.boundingBox.x;
                    *value = relX / data.boundingBox.width;
                    if (*value < 0.0f)
                        *value = 0.0f;
                    if (*value > 1.0f)
                        *value = 1.0f;
                }
            }
            Clay_ElementId innerBoxId =
                Clay_GetElementIdWithIndex(CLAY_STRING("InnerBox"), index);
            CLAY(innerBoxId, line) {
                float innerWidth =
                    Clay_GetElementData(innerBoxId).boundingBox.width;
                float maxOffset = innerWidth;
                if (maxOffset < 0.0f)
                    maxOffset = 0.0f;

                Clay_ElementDeclaration activeHandle = handle;
                activeHandle.floating.offset.x = (*value) * maxOffset;

                CLAY(Clay_GetElementIdWithIndex(CLAY_STRING("Handle"), index),
                     activeHandle) {}
            }
        }
    }
}

void RenderToggle(int index, Clay_ElementId id, Clay_String labelText,
                  bool *value) {
    CLAY(CLAY_IDI("Row", index), rowConfig) {
        CLAY(CLAY_IDI("LabelContainer", index), labelConfig) {
            CLAY_TEXT(labelText, CLAY_TEXT_CONFIG(
                                     {.fontSize = 40, .textColor = textColor}));
        }
        CLAY(id, toggleBox) {

            Clay_ElementId subElemId =
                Clay_GetElementIdWithIndex(CLAY_STRING("ToggleKnob"), index);

            if ((Clay_PointerOver(id) || Clay_PointerOver(subElemId)) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                *value = !(*value);
            }
            Clay_ElementDeclaration activeKnob = toggleKnob;
            if (*value) {
                activeKnob.backgroundColor = settingsOrange;
            } else {
                activeKnob.backgroundColor = settingsTransparent;
            }

            CLAY(subElemId, activeKnob) {}
        }
    }
}

void RenderBackButton(Clay_ElementId id, Clay_String labelText) {
    Clay_ElementDeclaration config = backButtonConfig;
    if (Clay_PointerOver(id)) {
        config.backgroundColor = settingsOrange;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentState = STATE_MAIN_MENU;
        }
    }
    CLAY(id, config) {
        CLAY_TEXT(labelText,
                  CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = textColor}));
    }
}

Clay_RenderCommandArray getSettingsMenu() {
    Clay_SetPointerState(RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition()),
                         IsMouseButtonDown(MOUSE_LEFT_BUTTON));
    Clay_SetLayoutDimensions(
        (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterBox"), settingsRootConfig) {
        CLAY(CLAY_ID("ButtonContainer"), settingsContainerConfig) {
            CLAY_TEXT(CLAY_STRING("SETTINGS"),
                      CLAY_TEXT_CONFIG(
                          {.fontSize = 48, .textColor = settingsOrange}));
            RenderSlider(1, CLAY_ID("MusicSlider"), CLAY_STRING("Music Audio"),
                         &musicVolume);
            RenderSlider(2, CLAY_ID("SFXSlider"), CLAY_STRING("SFX Audio"),
                         &sfxVolume);
            RenderToggle(3, CLAY_ID("FullScreenToggle"),
                         CLAY_STRING("Full Screen"), &fullScreen);
            RenderSlider(4, CLAY_ID("ShakeSlider"), CLAY_STRING("Screen Shake"),
                         &screenShake);
            RenderToggle(5, CLAY_ID("BloomToggle"), CLAY_STRING("Bloom"),
                         &bloomEnabled);
            RenderBackButton(CLAY_ID("BackButton"), CLAY_STRING("Back"));
        }
    }

    return Clay_EndLayout(GetFrameTime());
}