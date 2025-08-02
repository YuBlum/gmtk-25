#include "game/entities.h"
#include "game/flash.h"
#include "engine/collision.h"
#include "engine/window.h"

static bool g_block_btn;

bool
box_blocked_button(void) {
  return g_block_btn;
}

void
box_init(struct box_data *self) {
  self->amount = self->capacity;
  uint32_t i;
  for (i = 0; i < self->amount; i++) self->sprite[i] = SPR_BOX_TEST;
  for (i = 0; i < self->amount; i++) self->size[i] = V2(1.5f, 1.5f);
  for (i = 0; i < self->amount; i++) self->depth[i] = 1.0f;
  for (i = 0; i < self->amount; i++) self->flash[i] = 0.0f;
  for (i = 0; i < self->amount; i++) self->item_drop_type[i] = ITEM_NONE;
  for (i = 0; i < self->amount; i++) self->can_drop[i] = false;
  g_block_btn = false;
}

void
box_update(struct box_data *self, float dt) {
  auto player = entities_get_player_data();
  auto item = entities_get_item_data();
  if (g_block_btn) g_block_btn = false;
  int32_t target = flash_update_target(self->amount, self->flash_target, self->flash, self->position, self->size);
  if (target != -1) {
    if (player->item_held == -1) {
      if (self->item_drop_type[target] == ITEM_NONE) {
        self->flash_target[target] = 0.0f;
      } else if (window_is_key_press(K_A) && self->can_drop[target]) {
        player->item_held = item->amount;
        item_push(item, self->item_drop_type[target], self->position[target], false, false, 0.0f);
        item->depth[player->item_held] = player->depth - 1.0f;
        self->item_drop_type[target] = ITEM_NONE;
        self->can_drop[target] = false;
        g_block_btn = true;
      }
    } else {
      if (self->item_drop_type[target] != ITEM_NONE) {
        self->flash_target[target] = 0.0f;
      } else if (window_is_key_press(K_A)) {
        item->position_target[player->item_held] = self->position[target];
        item->timer_to_die[player->item_held] = 1.0f;
        item->box_index[player->item_held] = target;
        self->item_drop_type[target] = item->type[player->item_held];
        player->item_held = -1;
        g_block_btn = true;
      }
    }
  }
  flash_update(self->amount, self->flash_target, self->flash, dt);
}

void
box_render(struct box_data *self) {
  renderer_request_sprites(self->amount, self->sprite, self->position, 0, 0, 0, 0, 0, self->depth, self->flash);
}
