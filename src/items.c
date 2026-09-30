#include <stdint.h>
#include <stdio.h>
#include <raylib.h>
#include "player.h"
#include "items.h"
#define MAX_ITEMS 10

Item items[MAX_ITEMS];
uint32_t numItems = 0;

static void drawMaxHealthItem(Vector2 pos) {
    const Vector2 size = {.x = 42, .y = 13};

    const float rotation = 0.f;
    const Color drawColor = RED;
    Rectangle playerRec = {pos.x, pos.y, size.x, size.y};
    Vector2 playerOrigin = {size.x * 0.5f, size.y * 0.5f};

    Rectangle playerRecFlipped = {pos.x, pos.y, size.y, size.x};
    Vector2 playerOriginFlipped = {size.y * 0.5f, size.x * 0.5f};

    DrawRectanglePro(playerRec, playerOrigin, rotation, drawColor);
    DrawRectanglePro(playerRecFlipped, playerOriginFlipped, rotation,
                     drawColor);
}

bool addItem(Item item) {
    if (numItems < MAX_ITEMS) {
        items[numItems] = item;
        numItems++;
        return true;
    }
    return false;
}

void drawItems() {
    if (numItems > MAX_ITEMS) {
        puts("ERROR: TOO MANY ITEMS");
        return;
    }
    for (uint32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
        switch (items[itemIndex].itemType) {
            case ITEM_MAX_HEALTH:
                drawMaxHealthItem(items[itemIndex].position);
                break;
            case ITEM_CURRENT_HEALTH:
                break;
            case ITEM_DAMAGE:
                break;

            default:
                puts("ERROR: Impossible Item Type");
                break;
        }
    }
}
static void removeItem(uint32_t index) {
    if (index >= numItems) {
        return;
    }
    numItems--;
    items[index] = items[numItems];
}
void updateItems() {
    // This is mostly for collision checks with the player.

    for (uint32_t itemIndex = 0; itemIndex < numItems; itemIndex++) {
        if (Vector2Distance(items[itemIndex].position, mainPlayer.position) <
            40.0)
            switch (items[itemIndex].itemType) {
                case ITEM_MAX_HEALTH:
                    mainPlayer.maxHealth += 15;
                    mainPlayer.currentHealth += 15;
                    removeItem(itemIndex);
                    break;
                case ITEM_CURRENT_HEALTH:
                    break;
                case ITEM_DAMAGE:
                    break;
                default:
                    puts("ERROR: Impossible Item Type");
                    break;
            }
    }
}