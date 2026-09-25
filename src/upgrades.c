#include <string.h>

#include "upgrades.h"
#include "settings.h"
#include "ui.h"
#include "levelManager.h"
#include "player.h"

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                 \
    (Clay_Vector2) { .x = vector.x, .y = vector.y }
#define CLAY_STR(str)                                                          \
    ((Clay_String){.length = (uint32_t)strlen(str), .chars = (str)})

#include <stdint.h>
#include <stdio.h>

typedef enum {
    UPGRADE_CATEGORY_NONE = -1,
    UPGRADE_CATEGORY_CHARACTER = 0,
    UPGRADE_CATEGORY_GENERAL = 1
} UpgradeCategory;

typedef enum {
    GENERAL_UPGRADE_HEALTH = 1,
    GENERAL_UPGRADE_DAMAGE,
    GENERAL_UPGRADE_REGENERATION,
    GENERAL_UPGRADE_HEALING_ROOM,
    GENERAL_UPGRADE_LOOT_ROOM,
    GENERAL_UPGRADE_MOVE_SPEED,
    GENERAL_UPGRADE_DASH_UNLOCK,
    GENERAL_UPGRADE_DASH_COOLDOWN,
    GENERAL_UPGRADE_REMOVE_ROOM
} GeneralUpgradeType;

typedef struct {
    UpgradeCategory category;
    union {
        PlayerWeapon weaponId;
        GeneralUpgradeType upgradeId;
        int32_t id;
    };
    const char *name;
    const char *desc;
    uint32_t currentLevel;
    uint32_t maxLevel;
    uint32_t price;
} UIUpgradeData;

static UpgradeCategory selectedCategory = UPGRADE_CATEGORY_NONE;
static int32_t selectedId = -1;
static PlayerWeapon equippedCharacter = WEAPON_SHOOTER;

static UIUpgradeData characterData[] = {
    {
        .category = UPGRADE_CATEGORY_CHARACTER,
        .weaponId = WEAPON_SHOOTER,
        .name = "Shooter",
        .desc =
            "Fast Attack Speed, Normal Damage, Ranged.\n\nUpgrades:\n- Attack "
            "Speed\n- Scatter Shot\n- Big Third Shot",
        .currentLevel = 1,
        .maxLevel = 4,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_CHARACTER,
        .weaponId = WEAPON_FLAME,
        .name = "Flame Shot",
        .desc =
            "Fast Attack, Normal Damage, Applies Burn.\n\nUpgrades:\n- Hotter "
            "Shots\n- Faster Flame\n- Rotation Speed",
        .currentLevel = 0,
        .maxLevel = 4,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_CHARACTER,
        .weaponId = WEAPON_KNIGHT,
        .name = "Knight",
        .desc = "Slow Attack, Large Damage, Melee.\n\nUpgrades:\n- Combo "
                "Attack\n- Parry\n- Minions",
        .currentLevel = 0,
        .maxLevel = 4,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_CHARACTER,
        .weaponId = WEAPON_BIGSHOT,
        .name = "Big Shot",
        .desc = "Absurd damage, low speed.\n\nUpgrades:\n- Bigger Shot\n- "
                "Explosive Shot\n- Stationary Bonus",
        .currentLevel = 0,
        .maxLevel = 4,
        .price = 50,
    },
};

static UIUpgradeData upgradeData[] = {
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_HEALTH,
        .name = "Health Boost",
        .desc = "Flat Health Upgrade (+)",
        .currentLevel = 0,
        .maxLevel = 4,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_DAMAGE,
        .name = "Damage Boost",
        .desc = "Flat Damage Upgrade (+)",
        .currentLevel = 0,
        .maxLevel = 4,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_REGENERATION,
        .name = "Regeneration",
        .desc = "Heal 5 health after each room.",
        .currentLevel = 0,
        .maxLevel = 3,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_HEALING_ROOM,
        .name = "Healing Room",
        .desc = "New Room: Heals to Full, adds 15 max HP.",
        .currentLevel = 0,
        .maxLevel = 1,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_LOOT_ROOM,
        .name = "Loot Room",
        .desc = "New Room: Gives a random item.",
        .currentLevel = 0,
        .maxLevel = 1,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_MOVE_SPEED,
        .name = "Move Speed",
        .desc = "Increase flat Move Speed.",
        .currentLevel = 0,
        .maxLevel = 2,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_DASH_UNLOCK,
        .name = "Dodge Unlock",
        .desc = "Gain the ability to dodge.",
        .currentLevel = 0,
        .maxLevel = 1,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_DASH_COOLDOWN,
        .name = "Dodge CD",
        .desc = "Decrease dodge cooldown time.",
        .currentLevel = 0,
        .maxLevel = 2,
        .price = 50,
    },
    {
        .category = UPGRADE_CATEGORY_GENERAL,
        .upgradeId = GENERAL_UPGRADE_REMOVE_ROOM,
        .name = "Less Rooms",
        .desc = "Decrease rooms per floor by 1.",
        .currentLevel = 0,
        .maxLevel = 1,
        .price = 50,
    },
};
void updatePlayerHistoryFromPurchase(UIUpgradeData *item) {
    if (item->category == UPGRADE_CATEGORY_CHARACTER) {
        switch (item->weaponId) {
            case WEAPON_SHOOTER:
                history.shooterUpgradeLevel |= 0 << (item->currentLevel - 1);
                break;
            case WEAPON_BIGSHOT:
                history.bigshotUpgradeLevel |= 0 << (item->currentLevel - 1);
                break;
            case WEAPON_FLAME:
                history.flameShotUpgradeLevel |= 0 << (item->currentLevel - 1);
                break;
            case WEAPON_KNIGHT:
                history.knightUpgradeLevel |= 0 << (item->currentLevel - 1);
                break;
            default:
                break;
        }
    }

    if (item->category == UPGRADE_CATEGORY_GENERAL) {
        puts("Unlock!!");
        switch (item->upgradeId) {
            case GENERAL_UPGRADE_DAMAGE:
                history.playerUpgrades |= UPGRADE_DAMAGE_1
                                          << (item->currentLevel - 1);
                break;
            case GENERAL_UPGRADE_REGENERATION:
                history.playerUpgrades |= UPGRADE_REGENERATION_1
                                          << (item->currentLevel - 1);
                break;
            case GENERAL_UPGRADE_HEALING_ROOM:
                history.playerUpgrades |= UPGRADE_HEALING_ROOM;
                break;
            case GENERAL_UPGRADE_LOOT_ROOM:
                history.playerUpgrades |= UPGRADE_LOOT_ROOM;
                break;
            case GENERAL_UPGRADE_MOVE_SPEED:
                history.playerUpgrades |= UPGRADE_MOVE_SPEED_1
                                          << (item->currentLevel - 1);
                break;
            case GENERAL_UPGRADE_DASH_UNLOCK:
                history.playerUpgrades |= UPGRADE_DASH_UNLOCK;
                break;
            case GENERAL_UPGRADE_DASH_COOLDOWN:
                history.playerUpgrades |= UPGRADE_DASH_COOLDOWN_1
                                          << (item->currentLevel - 1);
                break;
            case GENERAL_UPGRADE_REMOVE_ROOM:
                history.playerUpgrades |= UPGRADE_REMOVE_ROOM;
                break;
            default:
                break;
        }

        printf("%s\n", ((history.playerUpgrades & UPGRADE_DASH_UNLOCK) != 0)
                           ? "true"
                           : "false");
        printf("%d\n", item->upgradeId);
    }
}

Clay_ElementDeclaration upgradesRootConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_GROW(0)},
               .layoutDirection = CLAY_LEFT_TO_RIGHT},
    .backgroundColor = settingsBlack};

Clay_ElementDeclaration leftPanelConfig = {
    .layout = {
        .sizing = {.width = CLAY_SIZING_PERCENT(0.65),
                   .height = CLAY_SIZING_GROW(0)},
        .childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP},
        .layoutDirection = CLAY_TOP_TO_BOTTOM,
        .childGap = 30,
        .padding = {40, 40, 40, 40}}};

Clay_ElementDeclaration rightPanelConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.35),
                          .height = CLAY_SIZING_GROW(0)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_TOP},
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childGap = 20,
               .padding = {40, 40, 40, 40}},
    .backgroundColor = settingsBlack,
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)}};

Clay_ElementDeclaration gridRowConfig = {
    .layout =
        {
            .sizing = {.width = CLAY_SIZING_GROW(0),
                       .height = CLAY_SIZING_FIT(0)},
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childGap = 0,
        },
};

Clay_ElementDeclaration itemBoxConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_FIXED(160),
                          .height = CLAY_SIZING_FIXED(120)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER},
               .layoutDirection = CLAY_TOP_TO_BOTTOM,
               .childGap = 15,
               .padding = {10, 10, 10, 10}},
    .backgroundColor = toggleBackgroundColor,
    .cornerRadius = CLAY_CORNER_RADIUS(8),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)}};

Clay_ElementDeclaration sectionHeaderConfig = {
    .layout = {
        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
        .childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER},
        .padding = {.bottom = 10}}};

Clay_ElementDeclaration buttonConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_FIXED(60)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER}},
    .backgroundColor = toggleBackgroundColor,
    .cornerRadius = CLAY_CORNER_RADIUS(8),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)}};

Clay_ElementDeclaration upgradesBackButtonConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_FIXED(150),
                          .height = CLAY_SIZING_FIXED(50)},
               .childAlignment = {.x = CLAY_ALIGN_X_CENTER,
                                  .y = CLAY_ALIGN_Y_CENTER}},
    .backgroundColor = toggleBackgroundColor,
    .cornerRadius = CLAY_CORNER_RADIUS(8),
    .border = {.color = settingsBlue, .width = CLAY_BORDER_OUTSIDE(2)}};

Clay_ElementDeclaration descSpacerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_FIXED(10)}}};

Clay_ElementDeclaration descSpacer2Config = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_FIXED(20)}}};

Clay_ElementDeclaration btnSpacerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_GROW(0)}}};

Clay_ElementDeclaration selectSpacerConfig = {
    .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_FIXED(20)}}};

void renderItemBox(UIUpgradeData *item) {
    Clay_ElementDeclaration config = itemBoxConfig;

    bool isSelected =
        (selectedCategory == item->category && selectedId == item->id);
    if (isSelected) {
        config.border.color = settingsOrange;
    }

    Clay_ElementId clayId =
        CLAY_IDI("ItemBox", item->category * 1000 + item->id);

    if (Clay_PointerOver(clayId) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        selectedCategory = item->category;
        selectedId = item->id;
    }

    CLAY(clayId, config) {
        CLAY_TEXT(CLAY_STR(item->name),
                  CLAY_TEXT_CONFIG({.fontSize = 24, .textColor = textColor}));

        if (item->category == 0) {
            if (equippedCharacter == item->id) {
                CLAY_TEXT(CLAY_STRING("EQUIPPED"),
                          CLAY_TEXT_CONFIG(
                              {.fontSize = 20, .textColor = settingsOrange}));
            } else {
                CLAY_TEXT(
                    item->currentLevel > 0 ? CLAY_STRING("OWNED")
                                           : CLAY_STRING("LOCKED"),
                    CLAY_TEXT_CONFIG({.fontSize = 20, .textColor = textColor}));
            }
        } else {
            static char levelBuf[64][32];
            static int levelIdx = 0;
            levelIdx = (levelIdx + 1) % 64;
            snprintf(levelBuf[levelIdx], 32, "%d / %d", item->currentLevel,
                     item->maxLevel);

            CLAY_TEXT(
                CLAY_STR(levelBuf[levelIdx]),
                CLAY_TEXT_CONFIG({.fontSize = 20, .textColor = textColor}));
        }
    }
}

void drawGrid(int category, UIUpgradeData *items, int count, int columns) {
    int rows = (count + columns - 1) / columns;
    for (int r = 0; r < rows; r++) {
        CLAY(CLAY_IDI("GridRow", category * 100 + r), gridRowConfig) {
            for (int c = 0; c < columns; c++) {
                int i = r * columns + c;
                if (i < count) {
                    renderItemBox(&items[i]);
                }
            }
        }
    }
}

void renderRightPanel(void) {
    if (selectedCategory == -1) {
        CLAY_TEXT(CLAY_STRING("Select an upgrade."),
                  CLAY_TEXT_CONFIG({.fontSize = 30, .textColor = textColor}));
        return;
    }

    UIUpgradeData *item = NULL;
    if (selectedCategory == 0) {
        for (int i = 0; i < 4; i++)
            if (characterData[i].id == selectedId)
                item = &characterData[i];
    } else {
        int count = sizeof(upgradeData) / sizeof(UIUpgradeData);
        for (int i = 0; i < count; i++)
            if (upgradeData[i].id == selectedId)
                item = &upgradeData[i];
    }

    if (!item)
        return;
    uint32_t cost = (1 + item->currentLevel) * item->price;

    CLAY_TEXT(CLAY_STR(item->name),
              CLAY_TEXT_CONFIG({.fontSize = 50, .textColor = settingsOrange}));

    CLAY(CLAY_ID("DescSpacer"), descSpacerConfig) {}
    CLAY_TEXT(CLAY_STR(item->desc),
              CLAY_TEXT_CONFIG({.fontSize = 30, .textColor = textColor}));
    CLAY(CLAY_ID("DescSpacer2"), descSpacer2Config) {}

    static char panelLevelBuf[64];
    snprintf(panelLevelBuf, 64, "Level: %d / %d", item->currentLevel,
             item->maxLevel);

    CLAY_TEXT(CLAY_STR(panelLevelBuf),
              CLAY_TEXT_CONFIG({.fontSize = 35, .textColor = textColor}));
    static char priceBuf[64];
    snprintf(priceBuf, 64, "Price: %u$", cost);
    CLAY_TEXT(CLAY_STR(priceBuf),
              CLAY_TEXT_CONFIG({.fontSize = 35, .textColor = textColor}));

    CLAY(CLAY_ID("BtnSpacer"), btnSpacerConfig) {}

    bool canBuy = item->currentLevel < item->maxLevel;
    bool hasMoney = mainPlayer.coins >= cost;
    Clay_ElementDeclaration buyCfg = buttonConfig;

    if (!canBuy || !hasMoney) {
        buyCfg.backgroundColor = settingsBlack;
    } else if (Clay_PointerOver(CLAY_ID("BuyBtn"))) {
        buyCfg.backgroundColor = settingsOrange;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            mainPlayer.coins -= cost;
            item->currentLevel++;
            updatePlayerHistoryFromPurchase(item);
        }
    }

    CLAY(CLAY_ID("BuyBtn"), buyCfg) {
        if (!canBuy) {
            CLAY_TEXT(
                CLAY_STRING("MAXED"),
                CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = textColor}));
        } else if (!hasMoney) {
            CLAY_TEXT(
                CLAY_STRING("NOT ENOUGH MONEY"),
                CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = textColor}));
        } else {
            static char buyBuf[32];
            snprintf(buyBuf, 32, "BUY (%u$)", cost);
            CLAY_TEXT(
                CLAY_STR(buyBuf),
                CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = textColor}));
        }
    }

    if (item->category == 0 && item->currentLevel > 0) {
        CLAY(CLAY_ID("SelectSpacer"), selectSpacerConfig) {}

        bool isEquipped = (equippedCharacter == item->id);
        Clay_ElementDeclaration selectCfg = buttonConfig;

        if (isEquipped) {
            selectCfg.backgroundColor = settingsBlue;
        } else if (Clay_PointerOver(CLAY_ID("SelectBtn"))) {
            selectCfg.backgroundColor = settingsOrange;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                equippedCharacter = item->id;
                history.currentPlayerWeapon = item->id;
            }
        }

        CLAY(CLAY_ID("SelectBtn"), selectCfg) {
            CLAY_TEXT(
                isEquipped ? CLAY_STRING("EQUIPPED")
                           : CLAY_STRING("SELECT CHARACTER"),
                CLAY_TEXT_CONFIG({.fontSize = 40, .textColor = textColor}));
        }
    }
}

Clay_RenderCommandArray getUpgradesMenu(void) {
    Clay_SetPointerState(RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition()),
                         IsMouseButtonDown(MOUSE_LEFT_BUTTON));
    Clay_SetLayoutDimensions(
        (Clay_Dimensions){(float)GetScreenWidth(), (float)GetScreenHeight()});

    Clay_BeginLayout();

    CLAY(CLAY_ID("UpgradesRoot"), upgradesRootConfig) {
        CLAY(CLAY_ID("LeftPanel"), leftPanelConfig) {

            CLAY(CLAY_ID("HeaderRow"), gridRowConfig) {
                Clay_ElementDeclaration backCfg = upgradesBackButtonConfig;
                if (Clay_PointerOver(CLAY_ID("UpgradesBackBtn"))) {
                    backCfg.backgroundColor = settingsOrange;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        currentState = STATE_MAIN_MENU;
                    }
                }
                CLAY(CLAY_ID("UpgradesBackBtn"), backCfg) {
                    CLAY_TEXT(CLAY_STRING("BACK"),
                              CLAY_TEXT_CONFIG(
                                  {.fontSize = 30, .textColor = textColor}));
                }
                CLAY(
                    CLAY_ID("Spacer"),
                    (Clay_ElementDeclaration){
                        .layout = {.sizing = {.width = CLAY_SIZING_GROW(0)}}}) {
                }
                static char balanceBuf[32];
                snprintf(balanceBuf, 32, "%d$", mainPlayer.coins);
                CLAY_TEXT(CLAY_STR(balanceBuf),
                          CLAY_TEXT_CONFIG(
                              {.fontSize = 48, .textColor = settingsOrange}));
            }
            CLAY(CLAY_ID("CharHeader"), sectionHeaderConfig) {
                CLAY_TEXT(CLAY_STRING("Characters"),
                          CLAY_TEXT_CONFIG(
                              {.fontSize = 40, .textColor = settingsOrange}));
            }
            drawGrid(1, characterData,
                     sizeof(characterData) / sizeof(UIUpgradeData), 5);
            CLAY(CLAY_ID("UpgradeHeader"), sectionHeaderConfig) {
                CLAY_TEXT(CLAY_STRING("UPGRADES"),
                          CLAY_TEXT_CONFIG(
                              {.fontSize = 40, .textColor = settingsOrange}));
            }
            drawGrid(1, upgradeData,
                     sizeof(upgradeData) / sizeof(UIUpgradeData), 5);
        }

        CLAY(CLAY_ID("RightPanel"), rightPanelConfig) { renderRightPanel(); }
    }

    return Clay_EndLayout(GetFrameTime());
}