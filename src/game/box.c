#include "game/entities.h"
#include "game/flash.h"
#include "engine/collision.h"
#include "engine/window.h"

void
box_init(struct box_data *self) {
  (void)self;
  self->amount = self->capacity;
  uint32_t i;
  for (i = 0; i < self->amount; i++) self->sprite[i] = SPR_BOX_TEST;
  for (i = 0; i < self->amount; i++) self->size[i] = V2(1.5f, 1.5f);
  for (i = 0; i < self->amount; i++) self->depth[i] = 0.0f;
  for (i = 0; i < self->amount; i++) self->flash[i] = 0.0f;
  for (i = 0; i < self->amount; i++) self->item_held[i] = ITEM_NONE;
}

void
box_update(struct box_data *self, float dt) {
  auto player = entities_get_player_data();
  if (player->item_held == -1) {
    for (uint32_t i = 0; i < self->amount; i++) self->flash_target[i] = 0.0f;
  } else {
    int32_t target = flash_update_target(self->amount, self->flash_target, self->flash, self->position, self->size);
    if (target != -1) {
      if (self->item_held[target] != ITEM_NONE) {
        self->flash_target[target] = 0.0f;
      } else if (window_is_key_press(K_A)) {
        auto item = entities_get_item_data();
        self->item_held[target] = item->type[player->item_held];
        item_remove(item, player->item_held);
        player->item_held = -1;
      }
    }
  }
  flash_update(self->amount, self->flash_target, self->flash, dt);
}

void
box_render(struct box_data *self) {
  renderer_request_sprites(self->amount, self->sprite, self->position, 0, 0, 0, 0, 0, self->depth, self->flash);
}
