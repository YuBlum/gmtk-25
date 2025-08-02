#ifndef __GAME_CORE_H__
#define __GAME_CORE_H__

#include "engine/math.h"
#include "engine/core.h"

struct item_data;

enum item_type {
  ITEM_LOCK = 0,
  ITEM_TRASH,
  ITEM_BOX,
  ITEM_ROPE,
  ITEM_MIRROR,
  ITEM_ROCK,
  ITEM_GLASS,
  ITEM_BROOM,
  ITEM_KNIFE,
  ITEM_KEY,
  ITEM_AMOUNT,
  ITEM_NONE = ITEM_AMOUNT
};

enum box_type {
  BOX_LAYOUT = 0,
  BOX_CONTENT,
};

enum room_layout {
  ROOM_DEFAULT = 0,
  ROOM_LOCK,
};

void item_push(struct item_data *self, enum item_type type, struct v2 position, bool flip);
void item_remove(struct item_data *self, uint32_t index);

bool box_blocked_button(void);

#endif/*__GAME_CORE_H__*/
