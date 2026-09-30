#pragma once
#include <raylib.h>

typedef enum {
    ITEM_MAX_HEALTH,
    ITEM_CURRENT_HEALTH,
    ITEM_DAMAGE,
} ItemType;

typedef struct {
    ItemType itemType;
    Vector2 position;
} Item;

void drawItems();
void updateItems();
bool addItem(Item item);