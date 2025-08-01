#include "game/entities.h"
#include "game/flash.h"
#include "engine/collision.h"

void
box_init(struct box_data *self) {
  (void)self;
  self->amount = self->capacity;
  for (uint32_t i = 0; i < self->amount; i++) {
    self->sprite[i] = SPR_BOX_TEST;
    self->size[i] = V2(1.5f, 1.5f);
    self->depth[i] = 0.0f;
    self->flash[i] = 0.0f;
  }
}

void
box_update(struct box_data *self, float dt) {
  (void)flash_update_target(self->amount, self->flash_target, self->flash, self->position, self->size);
  flash_update(self->amount, self->flash_target, self->flash, dt);
}

void
box_render(struct box_data *self) {
  renderer_request_sprites(self->amount, self->sprite, self->position, 0, 0, 0, 0, 0, self->depth, self->flash);
}
