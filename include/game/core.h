#ifndef __GAME_CORE_H__
#define __GAME_CORE_H__

#include "engine/math.h"
#include "engine/core.h"

struct item_data;

enum item_type {
  ITEM_TEST = 0,
  ITEM_AMOUNT
};

void item_push(struct item_data *self, enum item_type type, struct v2 position);
void item_pop(struct item_data *self, uint32_t index);

#endif/*__GAME_CORE_H__*/
