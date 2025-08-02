#ifndef __DOOR_H__
#define __DOOR_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct door_data {
  struct v2 position;
  struct v2 size;
  struct v2 left;
  struct v2 right;
};

void door_init(struct door_data *self);
void door_update(struct door_data *self, float dt);
void door_render(struct door_data *self);

#endif/*__DOOR_H__*/
