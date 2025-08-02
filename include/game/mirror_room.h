#ifndef __MIRROR_ROOM_H__
#define __MIRROR_ROOM_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct mirror_room_data {
  struct v2 position;
};

void mirror_room_init(struct mirror_room_data *self);
void mirror_room_render(struct mirror_room_data *self);

#endif/*__MIRROR_ROOM_H__*/
