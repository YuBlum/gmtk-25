#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "engine/math.h"
#include "engine/renderer.h"

struct player_data {
  struct v2 position;
  struct v2 scale;
  struct v2u texture_position;
  struct v2 pivot;
  float angle;
  float depth;
  float wiggle_cur;
  float wiggle_target;
};

void player_init(struct player_data *self);
void player_update(struct player_data *self, float dt);
void player_render(struct player_data *self);

#endif/*__PLAYER_H__*/
