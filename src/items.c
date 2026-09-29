#include <stdint.h>
#include <stdio.h>
#include "items.h"
#define MAX_ITEMS 10

Item items[MAX_ITEMS];
uint32_t numItems = 0;

static void drawMaxHealthItem(Vector2 pos) {
    const Vector2 size = {.x = 15, .y = 15};
    const float rotation = 0.f;
    const Color drawColor = RED;
    Rectangle playerRec = {pos.x, pos.y, size.x, size.y};
    Vector2 playerOrigin = {size.x * 0.5f, size.y * 0.5f};
    DrawRectanglePro(playerRec, playerOrigin, rotation, drawColor);
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