#ifndef __ITEM_H__
#define __ITEM_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct item_data {
  struct v2 *position;
  struct v2 *scale;
  enum sprite *sprite;
  float *angle;
  float *opacity;
  float *depth;
  float *flash;
  struct v2 *size;
  float *flash_target;
  struct v2 *position_target;
  struct v2 *launch_velocity;
  struct v2 *next_position;
  enum item_type *type;
  float *timer_to_die;
  int32_t *box_index;
  float *spawn_timer;
  uint32_t capacity;
  uint32_t amount;
};

void item_update(struct item_data *self, float dt);
void item_render(struct item_data *self);

#endif/*__ITEM_H__*/
