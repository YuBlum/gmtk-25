#include "game/item.h"
#include "game/entities.h"
#include "engine/window.h"
#include "engine/core.h"

void
item_init(struct item_data *self) {
  self->texture_position = V2U(16, 0);
  self->texture_size     = V2U(8, 8);
  self->position         = V2(0.0f, 0.0f);
  self->position_target  = self->position;
  self->size             = V2(0.5f, 0.5f);
  self->being_held       = false;
}

void
item_update(struct item_data *self, float dt) {
  auto player = entities_get_player_data();
  if (window_is_key_press(K_A)) {
    if (self->being_held) self->being_held = false;
    else if (v2_intersect(self->position, self->size, player->position, player->size)) self->being_held = true;
  }
  if (self->being_held) {
    self->position_target = player->scale.x > 0.0f ? V2(player->position.x - player->size.x * 0.5f, player->position.y)
                                                   : V2(player->position.x + player->size.x * 0.5f, player->position.y);
  }
  self->position = v2_lerp(self->position, self->position_target, 4.0f * dt);
}

void
item_render(struct item_data *self) {
  auto player = entities_get_player_data();
  renderer_request_quad(
    self->position,
    self->texture_position,
    self->texture_size,
    V2(0.0f, 0.0f),
    0.0f,
    V2(1.0f, 1.0f),
    WHITE,
    1.0f,
    self->being_held ? player->depth - 1.0f : player->depth + 1.0f
  );
}
