#include "game/entities.h"
#include "game/flash.h"
#include "engine/window.h"
#include "engine/core.h"
#include "engine/collision.h"

#define FOLLOW_SPEED 14.0f
#define LAUNCH_MIN_SPEED 0.1f
#define LAUNCH_MAX_SPEED 0.25f
#define LAUNCH_DECREASE_SPEED 10.0f

void
item_push(struct item_data *self, enum item_type type, struct v2 position) {
  if (self->amount >= self->capacity) {
    log_warnlf("%s: items capacity is already full", __func__);
    return;
  }
  if (type >= ITEM_AMOUNT) {
    log_warnlf("%s: '%d' isn't a valid item type", __func__, type);
    return;
  }
  uint32_t i = self->amount++;
  self->flash[i]            = 0.0f;
  self->flash_target[i]     = 0.0f;
  self->launch_velocity[i]  = V2(0.0f, 0.0f);
  self->position[i]         = position;
  self->type[i]             = type;
  switch (type) {
    case ITEM_TEST: {
      self->sprite[i] = SPR_ITEM_TEST;
      self->size[i]   = V2(0.5f, 0.5f);
    } break;
    case ITEM_TEST2: {
      self->sprite[i] = SPR_ITEM_TEST2;
      self->size[i]   = V2(1.0f, 1.0f);
    } break;
    case ITEM_AMOUNT:
      break;
  }
}

void
item_remove(struct item_data *self, uint32_t index) {
  if (index > self->amount) {
    log_warnlf("%s: index '%u' bigger than the items amount", __func__, index);
    return;
  }
  static_assert(sizeof (struct item_data) == sizeof (void *) * 10 + 8, "update the item removal, missing fields or a field was removed");
  uint32_t i, max = --self->amount;
  for (i = index; i < max; i++) self->position[i]        = self->position[i + 1];
  for (i = index; i < max; i++) self->sprite[i]          = self->sprite[i + 1];
  for (i = index; i < max; i++) self->depth[i]           = self->depth[i + 1];
  for (i = index; i < max; i++) self->flash[i]           = self->flash[i + 1];
  for (i = index; i < max; i++) self->size[i]            = self->size[i + 1];
  for (i = index; i < max; i++) self->flash_target[i]    = self->flash_target[i + 1];
  for (i = index; i < max; i++) self->position_target[i] = self->position_target[i + 1];
  for (i = index; i < max; i++) self->launch_velocity[i] = self->launch_velocity[i + 1];
  for (i = index; i < max; i++) self->next_position[i]   = self->next_position[i + 1];
  for (i = index; i < max; i++) self->type[i]            = self->type[i + 1];
}

void
item_update(struct item_data *self, float dt) {
  if (!self->amount) return;
  bool interacting = window_is_key_press(K_A);
  auto player = entities_get_player_data();
  if (player->item_held >= 0) {
    int32_t i = player->item_held;
    self->position_target[i] = player->scale.x > 0.0f ? V2(player->position.x - player->size.x * 0.5f, player->position.y)
                                                      : V2(player->position.x + player->size.x * 0.5f, player->position.y);
    self->position[i] = v2_lerp(self->position[i], self->position_target[i], FOLLOW_SPEED * dt);
    if (!interacting) return;
    static_assert(sizeof (struct item_data) == sizeof (void *) * 10 + 8, "update the items swap, missing fields or a field was removed");
    /* the code below is moving the current held item into the end of the list
       * this may seem useless, but it's necessary to make you able to swap between multiple items
       * the 'launch_velocity', 'next_position', 'position_target' and 'depth' fields don't need to be added to the swapping */
    auto type             = self->type[i];
    auto position         = self->position[i];
    auto sprite           = self->sprite[i];
    auto size             = self->size[i];
    auto flash            = self->flash[i];
    auto flash_target     = self->flash_target[i];
    item_remove(self, i);
    self->amount++;
    self->type[self->amount - 1]             = type;
    self->position[self->amount - 1]         = position;
    self->sprite[self->amount - 1]           = sprite;
    self->size[self->amount - 1]             = size;
    self->flash[self->amount - 1]            = flash;
    self->flash_target[self->amount - 1]     = flash_target;
    self->position_target[self->amount - 1]  = position;
    float launch_angle;
    if (player->scale.x < 0.0f) {
      launch_angle = randf() < 0.5f ? randf() * (PI/3.0f) + 5.0f*PI/3.0f : randf() * (PI/3.0f); // between 60 and 300 degrees (forward arc)
    } else {
      launch_angle = randf() * (2.0f*PI/3.0f) + 2.0f*PI/3.0f; // between 120 and 240 degrees
    }
    self->launch_velocity[self->amount - 1]  = v2_muls(V2(cosf(launch_angle), sinf(launch_angle)),
                                                       randf() * (LAUNCH_MAX_SPEED-LAUNCH_MIN_SPEED) + LAUNCH_MIN_SPEED);
    player->item_held = -1;
    return;
  }
  if (interacting) {
    for (uint32_t i = 0; i < self->amount; i++) {
      if (!check_rect_circle(self->position[i], self->size[i], player->interact_pos, player->interact_rad)) continue;
      self->depth[i] = player->depth - 1.0f;
      self->launch_velocity[i] = V2(0.0f, 0.0f);
      player->item_held = i;
      break;
    }
    for (uint32_t i = 0; i < self->amount; i++) {
      self->flash_target[i] = 0.0f;
      self->flash[i]        = 0.0f;
    }
  } else {
    int32_t target = flash_update_target(self->amount, self->flash_target, self->flash, self->position, self->size);
    for (uint32_t i = 0; i < self->amount; i++) self->depth[i] = player->depth + i * 0.1f;
    if (target != -1) self->depth[target] = player->depth + 0.01f;
  }
  flash_update(self->amount, self->flash_target, self->flash, dt);
  for (uint32_t i = 0; i < self->amount; i++) {
    self->launch_velocity[i] = v2_lerp(self->launch_velocity[i], V2(0.0f, 0.0f), LAUNCH_DECREASE_SPEED * dt);
    self->next_position[i] = v2_add(self->position[i], self->launch_velocity[i]);
  }
  auto solids = entities_get_solid_data();
  int32_t collided;
  struct v2 next;
  for (uint32_t i = 0; i < self->amount; i++) {
    collided = -1;
    next = V2(self->next_position[i].x, self->position[i].y);
    for (int32_t j = 0; j < (int32_t)solids->amount; j++) {
      if (!check_rect_rect(next, self->size[i], solids->position[j], solids->size[j])) continue;
      collided = j;
      break;
    }
    if (collided == -1) continue;
    self->launch_velocity[i].x *= -1.0f;
    self->next_position[i].x = resolve_rect_rect_axis(self->position[i].x,self->size[i].x, solids->position[collided].x,solids->size[collided].x);
  }
  for (uint32_t i = 0; i < self->amount; i++) {
    collided = -1;
    next = V2(self->position[i].x, self->next_position[i].y);
    for (int32_t j = 0; j < (int32_t)solids->amount; j++) {
      if (!check_rect_rect(next, self->size[i], solids->position[j], solids->size[j])) continue;
      collided = j;
      break;
    }
    if (collided == -1) continue;
    self->launch_velocity[i].y *= -1.0f;
    self->next_position[i].y = resolve_rect_rect_axis(self->position[i].y,self->size[i].y, solids->position[collided].y,solids->size[collided].y);
  }
  for (uint32_t i = 0; i < self->amount; i++) {
    self->position[i] = self->next_position[i];
  }
}

void
item_render(struct item_data *self) {
  renderer_request_sprites(
    self->amount,
    self->sprite,
    self->position,
    0, 0, 0, 0, 0,
    self->depth,
    self->flash
  );
}
