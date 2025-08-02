#include "game/entities.h"

void
mirror_room_init(struct mirror_room_data *self) {
  self->position = V2(0.0f, 1.0f);
}

void
mirror_room_render(struct mirror_room_data *self) {
  auto player = entities_get_player_data();
  renderer_request_sprite(SPR_MIRROR_ROOM, self->position, V2(0.0f, 1.5f), 0.0f, V2S(1.0f), WHITE, 1.0f,
                          player->position.y > self->position.y ? player->depth-10.0f : player->depth+0.1f, 0.0f);
}
