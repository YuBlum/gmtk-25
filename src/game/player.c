#include "game/entities.h"
#include "engine/window.h"
#include "engine/collision.h"
#include "game/scene.h"
#include "game/global.h"

#define SPEED 10.0f
#define WIGGLE 1.0f
#define WIGGLE_SPEED 20.0f
#define WIGGLE_EPSILON 0.01f
#define WIGGLE_POS (+WIGGLE-WIGGLE_EPSILON)
#define WIGGLE_NEG (-WIGGLE+WIGGLE_EPSILON)

#if DEV
bool show_colliders;
#endif

void
player_init(struct player_data *self) {
  *self = global.player_state;
}

void
player_update(struct player_data *self, float dt) {
#if DEV
  if (window_is_key_press(K_B)) show_colliders = !show_colliders;
#endif
  if (scene_is_in_transition()) {
   return;
  }
  /* movement */
  struct v2 move_direction = v2_muls(v2_unit(V2(
    window_is_key_down(K_RIGHT) - window_is_key_down(K_LEFT),
    window_is_key_down(K_UP)    - window_is_key_down(K_DOWN)
  )), SPEED * dt);
  struct v2 next_position = v2_add(self->position, move_direction),
            next_x        = { next_position.x, self->position.y },
            next_y        = { self->position.x, next_position.y };
  auto solids = entities_get_solid_data();
  int32_t collided_x = -1;
  for (int32_t i = 0; i < (int32_t)solids->amount; i++) {
    if (!check_rect_rect(next_x, self->size, solids->position[i], solids->size[i])) continue;
    collided_x = i;
    break;
  }
  if (collided_x != -1) {
    move_direction.x = 0.0f;
    next_position.x = resolve_rect_rect_axis(self->position.x, self->size.x, solids->position[collided_x].x, solids->size[collided_x].x);
  }
  int32_t collided_y = -1;
  for (int32_t i = 0; i < (int32_t)solids->amount; i++) {
    if (!check_rect_rect(next_y, self->size, solids->position[i], solids->size[i])) continue;
    collided_y = i;
    break;
  }
  if (collided_y != -1) {
    move_direction.y = 0.0f;
    next_position.y = resolve_rect_rect_axis(self->position.y, self->size.y, solids->position[collided_y].y, solids->size[collided_y].y);
  }
  self->position = next_position;
  self->interact_pos = v2_add(self->position, self->origin);
  bool moving = move_direction.x != 0.0f || move_direction.y != 0.0f;
  /* wiggle */
  self->wiggle_cur = lerp(self->wiggle_cur, self->wiggle_target, WIGGLE_SPEED * dt);
  if (moving) {
    if (self->wiggle_target == 0.0f) self->wiggle_target = randf() < 0.5f ? WIGGLE : -WIGGLE;
    if ((self->wiggle_target > 0.0f && self->wiggle_cur >= WIGGLE_POS) ||
        (self->wiggle_target < 0.0f && self->wiggle_cur <= WIGGLE_NEG)) self->wiggle_target *= -1.0f;
    if (move_direction.x) self->scale.x = signf(move_direction.x);
  } else {
    self->wiggle_target = 0.0f;
    if ((self->wiggle_cur > 0.0f && self->wiggle_cur <=  WIGGLE_EPSILON) ||
        (self->wiggle_cur < 0.0f && self->wiggle_cur >= -WIGGLE_EPSILON)) self->wiggle_cur = 0.0f;
  }
  self->angle = self->wiggle_cur * 0.5f;
  if (self->position.y - self->size.y * 0.5f > GAME_H * 0.5f) {
    global.player_state.position      = V2(self->position.x, -GAME_H * 0.5f + 1.5f);
    global.player_state.scale         = self->scale;
    global.player_state.wiggle_cur    = self->wiggle_cur;
    global.player_state.wiggle_target = self->wiggle_target;
    if (self->item_held != -1) {
      auto item = entities_get_item_data();
      global.extra_item_type     = item->type[self->item_held];
      global.extra_item_position = v2_sub(item->position[self->item_held], V2(0.0f, GAME_H));
    }
    auto box = entities_get_box_data();
    switch (box->item_drop_type[global.content_box]) {
      case ITEM_LOCK: {
        global.next_item_type = ITEM_TRASH;
      } break;
      case ITEM_TRASH: {
        global.next_item_type = ITEM_BOX;
      } break;
      case ITEM_BOX: {
        global.next_item_type = ITEM_ROPE;
      } break;
      case ITEM_ROPE: {
        global.next_item_type = ITEM_MIRROR;
      } break;
      case ITEM_MIRROR: {
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_ROCK: {
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_GLASS: {
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_BROOM: {
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_KNIFE: {
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_KEY: {
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_NONE:
        global.next_item_type = ITEM_LOCK;
        break;
    }
    scene_transition_to(MAP_DEFAULT_ROOM);
  }
}

void
player_render(struct player_data *self) {
  renderer_request_sprite(
    self->sprite,
    self->position,
    self->origin,
    self->angle,
    self->scale,
    WHITE,
    1.0f,
    self->depth,
    0.0f
  );

#if DEV
  if (show_colliders) {
    renderer_request_rect(self->position, self->size, RGB(1.0f, 0.0f, 1.0f), 0.4f, -100.0f);
    renderer_request_circle(v2_add(self->position, self->origin), self->interact_rad, GREEN, 0.1f);
  }
#endif
}
