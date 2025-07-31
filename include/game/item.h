#ifndef __ITEM_H__
#define __ITEM_H__

#include "engine/math.h"
#include "engine/renderer.h"

struct item_data {
  struct v2 *position;
  enum sprite *sprite;
  float *depth;
  float *flash;
  struct v2 *size;
  struct v2 *position_target;
  struct v2 *launch_velocity;
  struct v2 *next_position;
  float *flash_target;
  uint32_t capacity;
  uint32_t amount;
};

void item_init(struct item_data *self);
void item_update(struct item_data *self, float dt);
void item_render(struct item_data *self);

#endif/*__ITEM_H__*/
