#ifndef __ITEM_H__
#define __ITEM_H__

#include "engine/math.h"
#include "engine/renderer.h"

struct item_data {
  struct v2 *position;
  struct v2u *texture_position;
  struct v2u *texture_size;
  float *depth;
  struct v2 *size;
  struct v2 *position_target;
  uint32_t capacity;
  uint32_t amount;
};

void item_init(struct item_data *self);
void item_update(struct item_data *self, float dt);
void item_render(struct item_data *self);

#endif/*__ITEM_H__*/
