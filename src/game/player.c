#include "game/player.h"
#include "engine/window.h"

#define SPEED 5.0f

void
player_init(struct player_data *self) {
  self->position = V2(0.0f, 0.0f);
  self->texture_position = V2U(48, 0);
}

void
player_update(struct player_data *self, float dt) {
  struct v2 direction = v2_unit(V2(
    window_is_key_down(K_RIGHT) - window_is_key_down(K_LEFT),
    window_is_key_down(K_UP)    - window_is_key_down(K_DOWN)
  ));
  self->position.x += direction.x * SPEED * dt;
  self->position.y += direction.y * SPEED * dt;
}

void
player_render(struct player_data *self) {
  renderer_request_quad(
    self->position,
    V2(1.0f, 1.0f),
    self->texture_position,
    V2U(16, 16),
    WHITE,
    1.0f,
    1.0f
  );
}
