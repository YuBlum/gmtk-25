#ifndef __BOX_H__
#define __BOX_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct box_data {
  struct v2 *position;
  enum sprite *sprite;
  float *depth;
  float *flash;
  struct v2 *size;
  float *flash_target;
  enum box_type *type;
  enum item_type *item_drop_type;
  bool *can_drop;
  uint32_t capacity;
  uint32_t amount;
};

void box_init(struct box_data *self);
void box_update(struct box_data *self, float dt);
void box_render(struct box_data *self);

#endif/*__BOX_H__*/
