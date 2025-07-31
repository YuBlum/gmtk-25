#include "game/entities.h"
#include "engine/window.h"
#include "engine/core.h"

#define FOLLOW_SPEED 14.0f
#define LAUNCH_MIN_SPEED 0.1f
#define LAUNCH_MAX_SPEED 0.25f
#define LAUNCH_DECREASE_SPEED 10.0f
#define FLASH_TARGET 0.7f
#define FLASH_SPEED 8.0f
#define FLASH_EPSILON 0.01f

void
item_init(struct item_data *self) {
  self->amount = self->capacity;
  for (size_t i = 0; i < self->amount; i++) {
    self->texture_position[i] = V2U(16, 0);
    self->texture_size[i]     = V2U(8, 8);
    self->position[i]         = V2(randf() * GAME_W - GAME_W * 0.5f, randf() * GAME_H - GAME_H * 0.5f);
    self->position_target[i]  = self->position[i];
    self->size[i]             = V2(0.5f, 0.5f);
    self->launch_velocity[i]  = V2(0.0f, 0.0f);
    self->flash[i]            = 0.0f;
    self->flash_target[i]     = 0.0f;
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
    self->position[i] = v2_lerp(self->position[i], self->position_target[i], FOLLOW_SPEED * dt);
    if (!interacting) return;
    static_assert(sizeof (struct item_data) == sizeof (void *) * 9 + 8, "update the items swap, missing fields or a field was removed");
    /* the code below is moving the current held item into the end of the list
       * this may seem useless, but it's necessary to make you able to swap between multiple items
       * the 'launch_velocity' and 'position_target' fields don't need to be added to the swapping
       * 'depth' field also isn't needed because of the code line above */
    auto position         = self->position[i];
    auto texture_position = self->texture_position[i];
    auto texture_size     = self->texture_size[i];
    auto size             = self->size[i];
    auto flash            = self->flash[i];
    auto flash_target     = self->flash_target[i];
    for (uint32_t j = i; j < self->amount - 1; j++) {
      self->position[j]         = self->position[j + 1];
      self->texture_position[j] = self->texture_position[j + 1];
      self->texture_size[j]     = self->texture_size[j + 1];
      self->size[j]             = self->size[j + 1];
      self->flash[j]            = self->flash[j + 1];
      self->flash_target[j]     = self->flash_target[j + 1];
    }
    self->position[self->amount - 1]         = position;
    self->texture_position[self->amount - 1] = texture_position;
    self->texture_size[self->amount - 1]     = texture_size;
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
    player->held_item = -1;
    return;
  }
  if (interacting) {
    for (uint32_t i = 0; i < self->amount; i++) {
      if (!check_rect_circle(self->position[i], self->size[i], player->position, player->interact_rad)) continue;
      self->depth[i] = player->depth - 1.0f;
      self->launch_velocity[i] = V2(0.0f, 0.0f);
      player->held_item = i;
      break;
    }
    for (uint32_t i = 0; i < self->amount; i++) {
      self->flash_target[i] = 0.0f;
      self->flash[i]        = 0.0f;
    }
  } else {
    bool found_target = false;
    for (uint32_t i = 0; i < self->amount; i++) {
      if (!check_rect_circle(self->position[i], self->size[i], player->position, player->interact_rad) || found_target) {
        self->flash_target[i] = 0.0f;
        self->depth[i] = player->depth + i * 0.1f;
      } else {
        if (self->flash_target[i] == 0.0f && self->flash[i] <= 0.0f+FLASH_EPSILON) self->flash_target[i] = FLASH_TARGET;
        else if (self->flash_target[i] > 0.0f && self->flash[i] >= FLASH_TARGET-FLASH_EPSILON) self->flash_target[i] = 0.0f;
        self->depth[i] = player->depth + 0.01f;
        found_target = true;
      }
    }
  }
  for (uint32_t i = 0; i < self->amount; i++) {
    self->flash[i] = lerp(self->flash[i], self->flash_target[i], FLASH_SPEED * dt);
  }
  for (uint32_t i = 0; i < self->amount; i++) {
    self->launch_velocity[i] = v2_lerp(self->launch_velocity[i], V2(0.0f, 0.0f), LAUNCH_DECREASE_SPEED * dt);
    self->position[i] = v2_add(self->position[i], self->launch_velocity[i]);
  }
}

void
item_render(struct item_data *self) {
  renderer_request_quads(
    self->amount,
    self->position,
    self->texture_position,
    self->texture_size,
    0, 0, 0, 0, 0,
    self->depth,
    self->flash
  );
}
