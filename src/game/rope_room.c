#include "game/entities.h"

void
rope_room_init(struct rope_room_data *self) {
  self->position = V2(0.0f, -2.0f);
}

void
rope_room_render(struct rope_room_data *self) {
  struct v2 pos = V2(-GAME_W * 0.5f + 1.5f, self->position.y);
  renderer_request_sprite_slice(
    SPR_ROPE_ROOM,
    V2U(0, 0),
    V2U(16, 8),
    pos,
    V2S(0.0f),
    0.0f,
    V2S(1.0f),
    WHITE,
    1.0f,
    0.1f,
    0.0f
  );
  for (pos.x++; pos.x < GAME_W * 0.5f - 2.0f; pos.x++) {
    renderer_request_sprite_slice(
      SPR_ROPE_ROOM,
      V2U(16, 0),
      V2U(16, 8),
      pos,
      V2S(0.0f),
      0.0f,
      V2S(1.0f),
      WHITE,
      1.0f,
      0.1f,
      0.0f
    );
  }
  renderer_request_sprite_slice(
    SPR_ROPE_ROOM,
    V2U(32, 0),
    V2U(16, 8),
    pos,
    V2S(0.0f),
    0.0f,
    V2S(1.0f),
    WHITE,
    1.0f,
    0.1f,
    0.0f
  );
}
