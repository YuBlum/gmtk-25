#include "game/item.h"

void
item_init(struct item_data *self) {
  self->texture_position = V2U(16, 0);
  self->texture_size     = V2U(8, 8);
  self->position         = V2(0.0f, 0.0f);
  self->size             = V2(0.5f, 0.5f);
}

void
item_update(struct item_data *self, float dt) {
  (void)self; (void)dt;
  //log_warnlf("%s: not implemented", __func__);
}

void
item_render(struct item_data *self) {
  renderer_request_quad(
    self->position,
    self->texture_position,
    self->texture_size,
    V2(0.0f, 0.0f),
    0.0f,
    V2(1.0f, 1.0f),
    WHITE,
    1.0f,
    1.0f
  );
  renderer_request_rect(
    self->position,
    self->size,
    GREEN,
    1.0f,
    1.0f
  );
}
