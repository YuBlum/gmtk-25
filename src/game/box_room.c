#include "game/entities.h"

void
box_room_init(struct box_room_data *self) {
  self->position = V2(0.0f, 2.0f);
}

void
box_room_render(struct box_room_data *self) {
  renderer_request_sprite(SPR_BOX_TAPED, self->position, V2S(0.0f), 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
}
