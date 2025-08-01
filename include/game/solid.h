#ifndef __SOLID_H__
#define __SOLID_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct solid_data {
  struct v2 *position;
  struct v2 *size;
  uint32_t capacity;
  uint32_t amount;
};

void solid_render(struct solid_data *self);

#endif/*__SOLID_H__*/
