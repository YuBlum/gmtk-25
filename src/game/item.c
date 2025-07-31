#include "game/item.h"
#include "game/entities.h"
#include "engine/window.h"
#include "engine/core.h"

#define FOLLOW_SPEED 10.0f

void
item_init(struct item_data *self) {
  self->amount = self->capacity;
  for (size_t i = 0; i < self->amount; i++) {
    self->texture_position[i] = V2U(16, 0);
    self->texture_size[i]     = V2U(8, 8);
    self->position[i]         = V2(randf() * GAME_W - GAME_W * 0.5f, randf() * GAME_H - GAME_H * 0.5f);
    self->position_target[i]  = self->position[i];
    self->size[i]             = V2(0.5f, 0.5f);
  }
}

void
item_update(struct item_data *self, float dt) {
  bool interacting = window_is_key_press(K_A);
  auto player = entities_get_player_data();
  if (player->held_item >= 0) {
    int32_t i = player->held_item;
    self->position_target[i] = player->scale.x > 0.0f ? V2(player->position.x - player->size.x * 0.5f, player->position.y)
                                                      : V2(player->position.x + player->size.x * 0.5f, player->position.y);
    if (interacting) {
      self->depth[i] = player->depth + 1.0f;
      player->held_item = -1;
    }
  } else {
    for (uint32_t i = 0; interacting && i < self->amount; i++) {
      if (check_rect_circle(self->position[i], self->size[i], player->position, player->interact_rad)) {
        self->depth[i] = player->depth - 1.0f;
        player->held_item = i;
        break;
      }
    }
  }
  for (uint32_t i = 0; i < self->amount; i++) {
    self->position[i] = v2_lerp(self->position[i], self->position_target[i], FOLLOW_SPEED * dt);
  }
}

void
item_render(struct item_data *self) {
  for (uint32_t i = 0; i < self->amount; i++) {
    renderer_request_quad(
      self->position[i],
      self->texture_position[i],
      self->texture_size[i],
      V2(0.0f, 0.0f),
      0.0f,
      V2(1.0f, 1.0f),
      WHITE,
      1.0f,
      self->depth[i]
    );
  }
}
