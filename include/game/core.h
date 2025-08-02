#ifndef __GAME_CORE_H__
#define __GAME_CORE_H__

#include "engine/math.h"
#include "engine/core.h"

struct item_data;

enum item_type {
  ITEM_TEST = 0,
  ITEM_TEST2,
  ITEM_AMOUNT,
  ITEM_NONE = ITEM_AMOUNT
};

enum box_type {
  BOX_LAYOUT = 0,
  BOX_CONTENT,
};

void item_push(struct item_data *self, enum item_type type, struct v2 position);
void item_remove(struct item_data *self, uint32_t index);

bool box_blocked_button(void);

#endif/*__GAME_CORE_H__*/
