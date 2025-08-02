#ifndef __ROPE_ROOM_H__
#define __ROPE_ROOM_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct rope_room_data {
  struct v2 position;
};

void rope_room_init(struct rope_room_data *self);
void rope_room_render(struct rope_room_data *self);

#endif/*__ROPE_ROOM_H__*/
