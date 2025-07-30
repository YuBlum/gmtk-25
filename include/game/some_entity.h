#ifndef __SOME_ENTITY_H__
#define __SOME_ENTITY_H__

#include "engine/math.h"
#include "engine/renderer.h"

struct some_entity_data {
  struct v2 *position;
  struct v2 *size;
  struct v2u *texture_position;
  struct v2u *texture_size;
  float *angle;
  struct color *color;
  float *opacity;
  float *depth;
  uint32_t amount;
};

void some_entity_init(struct some_entity_data *self);
void some_entity_update(struct some_entity_data *self, float dt);
void some_entity_render(struct some_entity_data *self);

#endif/*__SOME_ENTITY_H__*/
