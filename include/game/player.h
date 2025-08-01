#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "game/core.h"
#include "engine/math.h"
#include "engine/renderer.h"

struct player_data {
  struct v2 position;
  struct v2 scale;
  enum sprite sprite;
  struct v2 origin;
  float angle;
  float depth;
  struct v2 size;
  struct v2 direction;
  struct v2 interact_pos;
  float interact_rad;
  float wiggle_cur;
  float wiggle_target;
  int32_t item_held;
};

void player_init(struct player_data *self);
void player_update(struct player_data *self, float dt);
void player_render(struct player_data *self);

#endif/*__PLAYER_H__*/
