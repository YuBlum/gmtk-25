#include "game/player.h"
#include "game/entities.h"
#include "engine/window.h"

#define SPEED 10.0f
#define WIGGLE 1.0f
#define WIGGLE_SPEED 20.0f
#define WIGGLE_EPSILON 0.01f
#define WIGGLE_POS (+WIGGLE-WIGGLE_EPSILON)
#define WIGGLE_NEG (-WIGGLE+WIGGLE_EPSILON)

void
player_init(struct player_data *self) {
  self->texture_position = V2U(0, 0);
  self->texture_size     = V2U(16, 16);
  self->position         = V2(0.0f, 0.0f);
  self->size             = V2(1.0f, 1.0f);
  self->scale            = V2(1.0f, 1.0f);
  self->angle            = 0.0f;
  self->wiggle_cur       = 0.0f;
  self->wiggle_target    = 0.0f;
  self->depth            = 0.0f;
  self->interact_rad     = 1.25f;
  self->held_item        = -1;
}

void
player_update(struct player_data *self, float dt) {
  struct v2 direction = v2_unit(V2(
    window_is_key_down(K_RIGHT) - window_is_key_down(K_LEFT),
    window_is_key_down(K_UP)    - window_is_key_down(K_DOWN)
  ));
  self->position.x += direction.x * SPEED * dt;
  self->position.y += direction.y * SPEED * dt;
  bool moving = direction.x != 0.0f || direction.y != 0.0f;
  self->wiggle_cur = lerp(self->wiggle_cur, self->wiggle_target, WIGGLE_SPEED * dt);
  if (moving) {
    if (self->wiggle_target == 0.0f) self->wiggle_target = WIGGLE;
    if ((self->wiggle_target > 0.0f && self->wiggle_cur >= WIGGLE_POS) ||
        (self->wiggle_target < 0.0f && self->wiggle_cur <= WIGGLE_NEG)) self->wiggle_target *= -1.0f;
    if (direction.x) self->scale.x = signf(direction.x);
  } else {
    self->wiggle_target = 0.0f;
    if ((self->wiggle_cur > 0.0f && self->wiggle_cur <=  WIGGLE_EPSILON) ||
        (self->wiggle_cur < 0.0f && self->wiggle_cur >= -WIGGLE_EPSILON)) self->wiggle_cur = 0.0f;
  }
  self->angle = self->wiggle_cur * 0.5f;
}

void
player_render(struct player_data *self) {
  renderer_request_quad(
    self->position,
    self->texture_position,
    self->texture_size,
    V2(0.0f, 0.0f),
    self->angle,
    self->scale,
    WHITE,
    1.0f,
    self->depth,
    0.0f
  );
  //renderer_request_circle(self->position, self->interact_rad, GREEN, 0.4f);
}
