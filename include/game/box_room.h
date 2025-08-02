#ifndef __BOX_ROOM_H__
#define __BOX_ROOM_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct box_room_data {
  struct v2 position;
};

void box_room_init(struct box_room_data *self);
void box_room_render(struct box_room_data *self);

#endif/*__BOX_ROOM_H__*/
