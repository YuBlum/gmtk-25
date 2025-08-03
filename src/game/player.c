#include "game/entities.h"
#include "engine/window.h"
#include "engine/collision.h"
#include "game/scene.h"
#include "game/global.h"
#include "game/sound.h"

#define SPEED 10.0f
#define WIGGLE 1.0f
#define WIGGLE_SPEED 20.0f
#define WIGGLE_EPSILON 0.01f
#define WIGGLE_POS (+WIGGLE-WIGGLE_EPSILON)
#define WIGGLE_NEG (-WIGGLE+WIGGLE_EPSILON)
#define STEP_SPEED 4.0f

#if DEV
bool show_colliders;
#endif

void
player_init(struct player_data *self) {
  *self = global.player_state;
  self->end_opacity = 0.0f;
}

void
player_update(struct player_data *self, float dt) {
#if DEV
  if (window_is_key_press(K_COLLIDERS)) {
    show_colliders = !show_colliders;
    global.going_out = true;
  }
#endif
  if (scene_is_in_transition()) return;
  if (global.end && !check_rect_rect(self->position, V2(2.0f, 1.0f), V2S(0.0f), V2(GAME_W, GAME_H))) {
    if (self->end_opacity < 1.0f) {
      self->end_opacity += dt;
    }
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
  /* step sounds */
  if (moving) {
    self->step_timer += STEP_SPEED * dt;
    if (self->step_timer >= 1.0f) {
      self->step_timer = 0.0f;
      (void)mixer_sound_play(sound_get(SND_STEP));
    }
  }
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
  /* go to next room state */
  if (!global.end && self->position.y - self->size.y * 0.5f > GAME_H * 0.5f) {
    global.player_state.position      = V2(self->position.x, -GAME_H * 0.5f + 1.5f);
    global.player_state.scale         = self->scale;
    global.player_state.wiggle_cur    = self->wiggle_cur;
    global.player_state.wiggle_target = self->wiggle_target;
    if (self->item_held != -1) {
      auto item = entities_get_item_data();
      global.extra_item_type     = item->type[self->item_held];
      global.extra_item_position = v2_sub(item->position[self->item_held], V2(0.0f, GAME_H));
      global.extra_item_sprite   = item->sprite[self->item_held];
    }
    auto box = entities_get_box_data();
    switch (box->item_drop_type[global.layout_box]) {
      case ITEM_NONE:
        global.next_room_layout = ROOM_DEFAULT;
        switch (box->item_drop_type[global.content_box]) {
          case ITEM_NONE:
            global.next_item_type = ITEM_LOCK;
            break;
          case ITEM_LOCK: {
            global.next_item_type = ITEM_ROPE;
          } break;
          case ITEM_ROPE: {
            global.next_item_type = ITEM_TRASH;
          } break;
          case ITEM_TRASH:
          case ITEM_RANDOM_TRASH: {
            global.next_item_type = ITEM_BOX;
          } break;
          case ITEM_BOX: {
            global.next_item_type = ITEM_MIRROR;
          } break;
          case ITEM_MIRROR: {
            global.next_item_type = ITEM_ROCK;
          } break;
          case ITEM_ROCK: {
            global.next_item_type = ITEM_MIRROR;
          } break;
          case ITEM_GLASS: {
            global.next_item_type = ITEM_BOX;
          } break;
          case ITEM_BROOM: {
            global.next_item_type = ITEM_TRASH;
          } break;
          case ITEM_KNIFE: {
            global.next_item_type = ITEM_ROPE;
          } break;
          case ITEM_KEY: {
            global.next_item_type = ITEM_LOCK;
          } break;
        }
        break;
      case ITEM_LOCK: {
        if (box->item_drop_type[global.content_box] == ITEM_KEY) {
          global.next_room_layout = ROOM_UNLOCKED_LOCK;
        } else {
          global.next_room_layout = ROOM_LOCK;
        }
        global.next_item_type = ITEM_NONE;
      } break;
      case ITEM_ROPE: {
        if (box->item_drop_type[global.content_box] == ITEM_KNIFE) {
          global.next_room_layout = ROOM_CUT_ROPE;
        } else {
          global.next_room_layout = ROOM_ROPE;
        }
        global.next_item_type = ITEM_KEY;
      } break;
      case ITEM_TRASH:
      case ITEM_RANDOM_TRASH: {
        if (box->item_drop_type[global.content_box] == ITEM_BROOM) {
          global.next_room_layout = ROOM_CLEANED_TRASH;
          global.next_item_type = ITEM_KNIFE;
        } else {
          global.next_room_layout = ROOM_TRASH;
          global.next_item_type = ITEM_NONE;
        }
      } break;
      case ITEM_BOX: {
        if (box->item_drop_type[global.content_box] == ITEM_GLASS || box->item_drop_type[global.content_box] == ITEM_KNIFE) {
          global.next_room_layout = ROOM_OPENED_BOX;
          global.next_item_type = ITEM_BROOM;
        } else {
            global.next_room_layout = ROOM_BOX;
            global.next_item_type = ITEM_NONE;
        }
      } break;
      case ITEM_MIRROR: {
        if (box->item_drop_type[global.content_box] == ITEM_ROCK) {
          global.next_room_layout = ROOM_BROKEN_MIRROR;
          global.next_item_type = ITEM_GLASS;
        } else {
          global.next_room_layout = ROOM_MIRROR;
          global.next_item_type = ITEM_NONE;
        }
      } break;
      default:
        global.next_room_layout = ROOM_DEFAULT;
        break;
    }
    if (global.going_out && global.next_room_layout == ROOM_DEFAULT) {
      global.end = true;
      scene_transition_to(MAP_OUTSIDE);
    } else {
      scene_transition_to(MAP_DEFAULT_ROOM);
    }
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
  if (global.end) {
    renderer_request_sprite(
      SPR_LOGO,
      V2S(0.0f),
      V2(0.0f, 2.0f),
      0.0f,
      V2S(2.0f),
      WHITE,
      self->end_opacity,
      -100.0f,
      0.0f
    );
    renderer_request_sprite(
      SPR_BY_YUBLUM,
      V2S(0.0f),
      V2(1.0f, -2.0f),
      0.0f,
      V2S(1.0f),
      WHITE,
      self->end_opacity,
      -100.0f,
      0.0f
    );
  }

#if DEV
  if (show_colliders) {
    renderer_request_rect(self->position, self->size, RGB(1.0f, 0.0f, 1.0f), 0.4f, -100.0f);
    renderer_request_circle(v2_add(self->position, self->origin), self->interact_rad, GREEN, 0.1f);
  }
#endif
}
