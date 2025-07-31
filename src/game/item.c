#include "game/item.h"
#include "game/entities.h"
#include "engine/window.h"
#include "engine/core.h"
#include "engine/utils.h"

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
  if (!self->amount) return;
  bool interacting = window_is_key_press(K_A);
  auto player = entities_get_player_data();
  if (player->held_item >= 0) {
    int32_t i = player->held_item;
    self->position_target[i] = player->scale.x > 0.0f ? V2(player->position.x - player->size.x * 0.5f, player->position.y)
                                                      : V2(player->position.x + player->size.x * 0.5f, player->position.y);
    if (interacting) {
      static_assert(sizeof (struct item_data) == sizeof (void *) * 6 + 8, "update the items swap, missing fields or a field was removed");
      self->depth[i] = player->depth + 1.0f;
      /* the code below is moving the current held item into the end of the list
       * this may seem useless, but it's necessary to make you able to swap between multiple items */
      auto position         = self->position[i];
      auto texture_position = self->texture_position[i];
      auto texture_size     = self->texture_size[i];
      auto size             = self->size[i];
      auto position_target  = self->position_target[i];
      for (uint32_t j = i; j < self->amount - 1; j++) {
        self->position[j]         = self->position[j + 1];
        self->texture_position[j] = self->texture_position[j + 1];
        self->texture_size[j]     = self->texture_size[j + 1];
        self->size[j]             = self->size[j + 1];
        self->position_target[j]  = self->position_target[j + 1];
      }
      self->position[self->amount - 1]         = position;
      self->texture_position[self->amount - 1] = texture_position;
      self->texture_size[self->amount - 1]     = texture_size;
      self->size[self->amount - 1]             = size;
      self->position_target[self->amount - 1]  = position_target;
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
