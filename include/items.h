#pragma once
#include <raylib.h>

typedef enum {
    ITEM_MAX_HEALTH = 1,
    ITEM_CURRENT_HEALTH = 2,
    ITEM_DAMAGE = 3,
} ItemType;

typedef struct {
    ItemType itemType;
    Vector2 position;
} Item;

void drawItems();
void updateItems();
bool addItem(Item item);

extern uint32_t numItems;