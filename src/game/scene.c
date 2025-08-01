#include "engine/renderer.h"
#include "engine/window.h"
#include "game/maps_data.h"
#include "game/entities.h"
#include "game/global.h"

struct scene {
  enum map current_map;
  enum map next_map;
  struct v2 offset;
  bool transition;
  bool has_rope;
  bool has_box;
  bool has_mirror;
};

#define ROPE_Y -2.0f
#define BOX_Y 1.0f
#define MIRROR_Y 1.0f

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
  bool is_door_locked = global.next_room_layout == ROOM_LOCK;
  g_scene.has_box    = global.next_room_layout == ROOM_BOX;
  g_scene.has_rope   = global.next_room_layout == ROOM_ROPE;
  g_scene.has_mirror = global.next_room_layout == ROOM_MIRROR;
  bool has_extra_item = global.extra_item_type != ITEM_NONE;
  uint32_t items_amount = 0;
  if (global.next_room_layout == ROOM_TRASH) {
    items_amount = 1000;
  } else if (global.next_item_type != ITEM_NONE) {
    items_amount = g_maps_data[map].items_amount;
  }
  layout.has_player = true;
  layout.has_door = true;
  if (g_scene.has_box || g_scene.has_rope || g_scene.has_mirror) {
    layout.solid_capacity = 1;
  }
  layout.item_capacity = items_amount + has_extra_item;
  layout.solid_capacity = g_maps_data[map].solids_amount + is_door_locked;
  layout.box_capacity = g_maps_data[map].boxes_amount;
  g_scene.current_map = map;
  g_scene.next_map = g_scene.current_map;
  if (!entities_layout_set(&layout)) return false;
  auto item = entities_get_item_data();
  if (global.next_room_layout == ROOM_TRASH) {
    struct v2 item_pos;
    for (uint32_t i = 0; i < items_amount; i++) {
      item_pos = V2(randf_from_to(-8.0f, +8.0f), randf_from_to(-8.0f, +4.5f));
      item_push(item, ITEM_RANDOM_TRASH, item_pos, false, true, randf_from_to(4.0f, 8.0f));
    }
  } else {
    for (uint32_t i = 0; i < items_amount; i++) {
      item_push(item, global.next_item_type, g_maps_data[map].items_position[i], false, true, 0.0f);
    }
  }
  if (has_extra_item) {
    auto player = entities_get_player_data();
    player->item_held = item->amount;
    item_push(item, global.extra_item_type, global.extra_item_position, player->scale.x < 0.0f, false, 0.0f);
    item->position_target[player->item_held] = player->position;
    item->depth[player->item_held] = player->depth - 1.0f;
    global.extra_item_type = ITEM_NONE;
  }
  auto box = entities_get_box_data();
  global.layout_box = -1;
  global.content_box = -1;
  for (uint32_t i = 0; i < box->amount; i++) {
    box->position[i] = g_maps_data[map].boxes_position[i];
    box->type[i]     = g_maps_data[map].boxes_type[i];
    switch (box->type[i]) {
      case BOX_LAYOUT:
        #if DEV
        if (global.layout_box != -1) log_errorlf("map '%u' has two layout boxes", map);
        #endif
        global.layout_box = i;
        break;
      case BOX_CONTENT:
        #if DEV
        if (global.content_box != -1) log_errorlf("map '%u' has two content boxes", map);
        #endif
        global.content_box = i;
        break;
    }
  }
  #if DEV
  if (global.layout_box  == -1) log_errorlf("map '%u' has no layout boxes", map);
  if (global.content_box == -1) log_errorlf("map '%u' has no content boxes", map);
  #endif
  auto solid = entities_get_solid_data();
  solid->amount = solid->capacity;
  for (uint32_t i = 0; i < g_maps_data[map].solids_amount; i++) {
    solid->position[i] = g_maps_data[map].solids_position[i];
    solid->size[i] = g_maps_data[map].solids_size[i];
  }
  if (is_door_locked) {
    auto door = entities_get_door_data();
    door->locked = true;
    solid->position[solid->amount-1] = door->position;
    solid->size[solid->amount-1] = door->size;
  }
  if (g_scene.has_box) {
    solid->position[solid->amount-1] = V2(0.0f, BOX_Y);
    solid->size[solid->amount-1] = V2(2.0f, 0.5f);
  }
  if (g_scene.has_rope) {
    solid->position[solid->amount-1] = V2(0.0f, ROPE_Y);
    solid->size[solid->amount-1] = V2(18.0f, 0.5f);
  }
  if (g_scene.has_mirror) {
    solid->position[solid->amount-1] = V2(0.0f, MIRROR_Y);
    solid->size[solid->amount-1] = V2(2.0f, 1.0f);
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
  if (!g_scene.transition) {
    if (window_is_key_press(K_RESTART)) {
      global_init();
      (void)scene_load(0);
    }
    return;
  }
  auto player = entities_get_player_data();
  player->position.y = lerp(player->position.y, GAME_H * 0.5f + 1.5f, 8.0f * dt);
  g_scene.offset.y   = lerp(g_scene.offset.y, GAME_H, 8.0f * dt);
  if (g_scene.offset.y >= GAME_H - 0.05f) {
    g_scene.offset.y = 0.0f;
    (void)scene_load(g_scene.next_map);
    g_scene.transition = false;
  }
  renderer_set_offset(g_scene.offset);
}

static void
rope_render(float y) {
  struct v2 pos = V2(-GAME_W * 0.5f + 1.5f, y);
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

void
scene_render(void) {
  auto map = &g_maps_data[g_scene.current_map];
  renderer_request_tileset(MAP_TILES_AMOUNT, map->tileset, V2S(0.0f), map->tiles_sprite_position, map->tiles_position, 0);
  if (g_scene.transition) {
    map = &g_maps_data[g_scene.next_map];
    renderer_request_tileset(MAP_TILES_AMOUNT, map->tileset, V2(0.0f, -GAME_H), map->tiles_sprite_position, map->tiles_position, 0);
  }
  if (g_scene.transition) {
    auto door = entities_get_door_data();
    if (global.next_room_layout == ROOM_LOCK) {
      renderer_request_sprite(SPR_DOOR_LOCKED, v2_add(door->position, V2(0.0f, GAME_H)), door->origin, 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
    } else {
      if (global.next_room_layout == ROOM_BOX) {
        renderer_request_sprite(SPR_BOX_TAPED, V2(0.0f, BOX_Y + GAME_H), V2(0.0f, 0.5f), 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
      } else if (global.next_room_layout == ROOM_ROPE) {
        rope_render(ROPE_Y + GAME_H);
      } else if (global.next_room_layout == ROOM_MIRROR) {
        renderer_request_sprite(SPR_MIRROR_ROOM, V2(0.0f, MIRROR_Y + GAME_H), V2(0.0f, 1.5f), 0.0f, V2S(1.0f), WHITE, 1.0f, 0.1f, 0.0f);
      }
      renderer_request_sprite(SPR_DOOR, v2_add(door->position, V2(-1.0f, GAME_H)), door->origin, 0.0f, V2(+1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
      renderer_request_sprite(SPR_DOOR, v2_add(door->position, V2(+1.0f, GAME_H)), door->origin, 0.0f, V2(-1.0f, 1.0f), WHITE, 1.0f, 0.1f, 0.0f);
    }
  } else if (g_scene.has_rope) {
    rope_render(ROPE_Y);
  } else if (g_scene.has_box) {
    auto player = entities_get_player_data();
    renderer_request_sprite(SPR_BOX_TAPED, V2(0.0f, BOX_Y), V2(0.0f, 0.5f), 0.0f, V2S(1.0f), WHITE, 1.0f,
                            player->position.y > BOX_Y ? player->depth-10.0f : player->depth+0.1f, 0.0f);
  } else if (g_scene.has_mirror) {
    auto player = entities_get_player_data();
    renderer_request_sprite(SPR_MIRROR_ROOM, V2(0.0f, MIRROR_Y), V2(0.0f, 1.5f), 0.0f, V2S(1.0f), WHITE, 1.0f,
                            player->position.y > MIRROR_Y ? player->depth-10.0f : player->depth+0.1f, 0.0f);
  }
}
