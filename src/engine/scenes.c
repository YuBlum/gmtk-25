#include "engine/maps_data.h"
#include "engine/renderer.h"
#include "game/entities.h"

static enum map g_current_map;

bool
scene_load_map(enum map map) {
  #if DEV
  if (map >= MAPS_AMOUNT) {
    log_warnlf("%s: map with number '%d' doesn't exists", __func__, map);
    return false;
  }
  #endif
  struct entities_layout layout = { 0 };
  layout.has_player = true;
  layout.item_capacity = g_maps_data[map].items_amount;
  layout.solid_capacity = g_maps_data[map].solids_amount;
  g_current_map = map;
  if (!entities_layout_set(&layout)) return false;
  auto item = entities_get_item_data();
  for (uint32_t i = 0; i < item->amount; i++) {
    item->position[i] = g_maps_data[map].items_position[i];
  }
  auto solid = entities_get_solid_data();
  for (uint32_t i = 0; i < solid->amount; i++) {
    solid->position[i] = g_maps_data[map].solids_position[i];
    solid->size[i] = g_maps_data[map].solids_size[i];
  }
  return true;
}

void
scene_render(void) {
  auto map = &g_maps_data[g_current_map];
  for (uint32_t i = 0; i < MAP_HEIGHT; i++) {
    for (uint32_t j = 0; j < MAP_WIDTH; j++) {
      struct v2 position = { j - GAME_W * 0.5f + 0.5f, GAME_H * 0.5f - i - 0.5f };
      renderer_request_sprite_slice(
        map->tileset,
        map->tiles_position[i * MAP_WIDTH + j],
        map->tiles_size[i * MAP_WIDTH + j],
        position,
        V2(0.0f, 0.0f),
        0.0f,
        V2(1.0f, 1.0f),
        WHITE,
        1.0f,
        100.0f,
        0.0f
      );
    }
  }
}
