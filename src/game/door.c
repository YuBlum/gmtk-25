#include "engine/collision.h"
#include "game/scene.h"
#include "game/entities.h"
#include "game/global.h"

#if DEV
extern bool show_colliders;
#endif

#define SPEED 8.0f
#define ORIGIN V2(0.0f, 0.5f)

void
door_init(struct door_data *self) {
  self->size     = V2(4.0f, 2.0f);
  self->position = V2(0.0f, GAME_H * 0.5f - self->size.y * 0.5f);
  self->left     = v2_sub(self->position, V2(1.0f, 0.0f));
  self->right    = v2_add(self->position, V2(1.0f, 0.0f));
}

void
door_update(struct door_data *self, float dt) {
  if (self->locked) return;
  auto player = entities_get_player_data();
  if (!scene_is_in_transition() && check_rect_circle(self->position, self->size, player->interact_pos, player->interact_rad)) {
    self->left  = v2_lerp(self->left,  v2_sub(self->position, V2(3.0f, 0.0f)), SPEED * dt);
    self->right = v2_lerp(self->right, v2_add(self->position, V2(3.0f, 0.0f)), SPEED * dt);
  } else {
    self->left  = v2_lerp(self->left,  v2_sub(self->position, V2(1.0f, 0.0f)), SPEED * dt);
    self->right = v2_lerp(self->right, v2_add(self->position, V2(1.0f, 0.0f)), SPEED * dt);
  }
}

void
door_render(struct door_data *self) {
  if (self->locked) {
    renderer_request_sprite(SPR_DOOR_LOCKED, self->position, ORIGIN, 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
  } else {
    renderer_request_sprite(SPR_DOOR, self->left, ORIGIN, 0.0f, V2(+1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
    renderer_request_sprite(SPR_DOOR, self->right, ORIGIN, 0.0f, V2(-1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
  }
  renderer_request_sprite(SPR_DOOR_LOCKED, v2_sub(self->position, V2(0.0f, GAME_H)), ORIGIN, 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
  if (scene_is_in_transition()) {
    /* NOTE: this is a hack for the room transition, yes it is disgusting */
    if (global.next_room_layout == ROOM_LOCK) {
      renderer_request_sprite(SPR_DOOR_LOCKED, v2_add(self->position, V2(0.0f, GAME_H)), ORIGIN, 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
    } else {
      renderer_request_sprite(SPR_DOOR, v2_add(self->position, V2(-1.0f, GAME_H)), ORIGIN, 0.0f, V2(+1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
      renderer_request_sprite(SPR_DOOR, v2_add(self->position, V2(+1.0f, GAME_H)), ORIGIN, 0.0f, V2(-1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
    }
  }
#if DEV
  if (show_colliders) renderer_request_rect(self->position, self->size, RGB(0.7, 0.9, 0.9), 0.4f, -100.0f);
#endif
}
