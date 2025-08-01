#include "engine/collision.h"
#include "game/entities.h"
#include "game/flash.h"

#define FLASH_TARGET 0.7f
#define FLASH_SPEED 8.0f
#define FLASH_EPSILON 0.01f

int32_t
flash_update_target(uint32_t amount, float *flash_targets, float *flashes, struct v2 *positions, struct v2 *sizes) {
  auto player = entities_get_player_data();
  int32_t target = -1;
  for (uint32_t i = 0; i < amount; i++) {
    if (!check_rect_circle(positions[i], sizes[i], player->interact_pos, player->interact_rad) || target != -1) {
      flash_targets[i] = 0.0f;
      continue;
    }
    if      (flash_targets[i] == 0.0f && flashes[i] <=         0.0f+FLASH_EPSILON) flash_targets[i] = FLASH_TARGET;
    else if (flash_targets[i]  > 0.0f && flashes[i] >= FLASH_TARGET-FLASH_EPSILON) flash_targets[i] = 0.0f;
    target = i;
  }
  return target;
}

void
flash_update(uint32_t amount, float *flash_targets, float *flashes, float dt) {
  for (uint32_t i = 0; i < amount; i++) {
    flashes[i] = lerp(flashes[i], flash_targets[i], FLASH_SPEED * dt);
  }
}
