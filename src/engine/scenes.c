#include "engine/maps_data.h"
#include "engine/renderer.h"
#include "game/entities.h"
#include "game/global.h"

struct scene {
  enum map current_map;
  enum map next_map;
  struct v2 offset;
  bool transition;
};

static struct scene g_scene;

bool
scene_load(enum map map) {
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
  layout.box_capacity = g_maps_data[map].boxes_amount;
  g_scene.current_map = map;
  g_scene.next_map = g_scene.current_map;
  if (!entities_layout_set(&layout)) return false;
  auto item = entities_get_item_data();
  for (uint32_t i = 0; i < item->capacity; i++) {
    item_push(item, g_maps_data[map].items_type[i], g_maps_data[map].items_position[i]);
  }
  auto box = entities_get_box_data();
  for (uint32_t i = 0; i < box->amount; i++) {
    box->position[i] = g_maps_data[map].boxes_position[i];
  }
  auto solid = entities_get_solid_data();
  solid->amount = solid->capacity;
  for (uint32_t i = 0; i < solid->amount; i++) {
    solid->position[i] = g_maps_data[map].solids_position[i];
    solid->size[i] = g_maps_data[map].solids_size[i];
  }
  return true;
}

void
scene_transition_to(enum map map) {
  #if DEV
  if (map >= MAPS_AMOUNT) {
    log_warnlf("%s: map with number '%d' doesn't exists", __func__, map);
    return;
  }
  #endif
  g_scene.next_map = map;
  g_scene.transition = true;
}

bool
scene_is_in_transition(void) {
  return g_scene.transition;
}

void
scene_update(float dt) {
  (void)dt;
  if (g_scene.transition) {
    g_scene.offset.y = lerp(g_scene.offset.y, GAME_H, 8.0f * dt);
    if (g_scene.offset.y >= GAME_H - 0.05f) {
      g_scene.offset.y = 0.0f;
      scene_load(g_scene.next_map);
      g_scene.transition = false;
    }
    renderer_set_offset(g_scene.offset);
  }
}

void
scene_render(void) {
  auto map = &g_maps_data[g_scene.current_map];
  renderer_request_tileset(MAP_TILES_AMOUNT, map->tileset, V2S(0.0f), map->tiles_sprite_position, map->tiles_position, 0);
  if (g_scene.transition) {
    map = &g_maps_data[g_scene.next_map];
    renderer_request_tileset(MAP_TILES_AMOUNT, map->tileset, V2(0.0f, -GAME_H), map->tiles_sprite_position, map->tiles_position, 0);
  }
}
