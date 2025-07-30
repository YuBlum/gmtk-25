#include "game/player.h"
#include "engine/window.h"

#define SPEED 10.0f
#define WIGGLE 1.0f
#define WIGGLE_SPEED 20.0f
#define WIGGLE_EPISOLON 0.01f
#define WIGGLE_POS (+WIGGLE-WIGGLE_EPISOLON)
#define WIGGLE_NEG (-WIGGLE+WIGGLE_EPISOLON)

void
player_init(struct player_data *self) {
  self->texture_position = V2U(0, 0);
  self->position         = V2(0.0f, 0.0f);
  self->scale            = V2(1.0f, 1.0f);
  self->angle            = 0.0f;
  self->wiggle_cur       = 0.0f;
  self->wiggle_target    = 0.0f;
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
      (self->wiggle_target < 0.0f && self->wiggle_cur <= WIGGLE_NEG)) {
      self->wiggle_target *= -1.0f;
    }
    if (direction.x) self->scale.x = signf(direction.x);
  } else {
    self->wiggle_target = 0.0f;
    if ((self->wiggle_cur > 0.0f && self->wiggle_cur <=  WIGGLE_EPISOLON) ||
        (self->wiggle_cur < 0.0f && self->wiggle_cur >= -WIGGLE_EPISOLON)) self->wiggle_cur = 0.0f;
  }
  self->angle = self->wiggle_cur * 0.5f;
}

void
player_render(struct player_data *self) {
  renderer_request_quad(
    self->position,
    self->texture_position,
    V2U(16, 16),
    V2(-0.125f, -0.125f),
    self->angle,
    self->scale,
    BLACK,
    0.4f,
    1.0f
  );
  renderer_request_quad(
    self->position,
    self->texture_position,
    V2U(16, 16),
    V2(0.0f, 0.0f),
    self->angle,
    self->scale,
    WHITE,
    1.0f,
    0.0f
  );
}
