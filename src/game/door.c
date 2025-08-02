#include "engine/collision.h"
#include "game/scene.h"
#include "game/entities.h"
#include "game/global.h"

#if DEV
extern bool show_colliders;
#endif

#define SPEED 8.0f

void
door_init(struct door_data *self) {
  self->left  = v2_sub(DOOR_POSITION, V2(1.0f, 0.0f));
  self->right = v2_add(DOOR_POSITION, V2(1.0f, 0.0f));
}

void
door_update(struct door_data *self, float dt) {
  if (self->locked) return;
  auto player = entities_get_player_data();
  if (!scene_is_in_transition() && check_rect_circle(DOOR_POSITION, self->size, player->interact_pos, player->interact_rad)) {
    self->left  = v2_lerp(self->left,  v2_sub(DOOR_POSITION, V2(3.0f, 0.0f)), SPEED * dt);
    self->right = v2_lerp(self->right, v2_add(DOOR_POSITION, V2(3.0f, 0.0f)), SPEED * dt);
  } else {
    self->left  = v2_lerp(self->left,  v2_sub(DOOR_POSITION, V2(1.0f, 0.0f)), SPEED * dt);
    self->right = v2_lerp(self->right, v2_add(DOOR_POSITION, V2(1.0f, 0.0f)), SPEED * dt);
  }
}

void
door_render(struct door_data *self) {
  if (self->locked) {
    renderer_request_sprite(SPR_DOOR_LOCKED, DOOR_POSITION, DOOR_ORIGIN, 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
  } else {
    renderer_request_sprite(SPR_DOOR, self->left, DOOR_ORIGIN, 0.0f, V2(+1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
    renderer_request_sprite(SPR_DOOR, self->right, DOOR_ORIGIN, 0.0f, V2(-1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
  }
#if DEV
  if (show_colliders) renderer_request_rect(DOOR_POSITION, self->size, RGB(0.7, 0.9, 0.9), 0.4f, -100.0f);
#endif
}
