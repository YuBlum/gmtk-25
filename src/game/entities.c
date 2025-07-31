#include "engine/arena.h"
#include "game/entities.h"
#include "game/item.h"
#include "game/player.h"

struct entities {
  struct arena *arena;
  struct item_data item_data;
  struct player_data *player_data;
};

static struct entities g_entities;

bool
entities_make(void) {
  g_entities.arena = arena_make(0, 0);
  if (!g_entities.arena) {
    log_errorl("couldn't make entities arena");
    return false;
  }
  log_infol("entities manager creation completed!");
  return true;
}

bool
entities_layout_set(const struct entities_layout *layout) {
  if (!arena_clear(g_entities.arena)) { log_errorl("couldn't clear entities arena"); return false; }
  if (layout->item_capacity) {
    g_entities.item_data.capacity = layout->item_capacity;
    g_entities.item_data.amount   = 0;
    g_entities.item_data.position = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.position) {
      log_errorl("couldn't allocate item position data");
      return false;
    }
    g_entities.item_data.texture_position = arena_push_array(g_entities.arena, false, struct v2u, layout->item_capacity);
    if (!g_entities.item_data.texture_position) {
      log_errorl("couldn't allocate item texture_position data");
      return false;
    }
    g_entities.item_data.texture_size = arena_push_array(g_entities.arena, false, struct v2u, layout->item_capacity);
    if (!g_entities.item_data.texture_size) {
      log_errorl("couldn't allocate item texture_size data");
      return false;
    }
    g_entities.item_data.depth = arena_push_array(g_entities.arena, false, float, layout->item_capacity);
    if (!g_entities.item_data.depth) {
      log_errorl("couldn't allocate item depth data");
      return false;
    }
    g_entities.item_data.flash = arena_push_array(g_entities.arena, false, float, layout->item_capacity);
    if (!g_entities.item_data.flash) {
      log_errorl("couldn't allocate item flash data");
      return false;
    }
    g_entities.item_data.size = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.size) {
      log_errorl("couldn't allocate item size data");
      return false;
    }
    g_entities.item_data.position_target = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.position_target) {
      log_errorl("couldn't allocate item position_target data");
      return false;
    }
    g_entities.item_data.launch_velocity = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.launch_velocity) {
      log_errorl("couldn't allocate item launch_velocity data");
      return false;
    }
    item_init(&g_entities.item_data);
  } else {
    g_entities.item_data.capacity = 0;
  }
  if (layout->has_player) {
    g_entities.player_data = arena_push_type(g_entities.arena, false, struct player_data);
    if (!g_entities.player_data) {
      log_errorl("couldn't allocate player data");
      return false;
    }
    player_init(g_entities.player_data);
  } else {
    g_entities.player_data = 0;
  }
  return true;
}

void
entities_update(float dt) {
#if DEV
  if (!g_entities.arena) {
    log_errorlf("%s: arena is null", __func__);
    return;
  }
#endif
  if (g_entities.item_data.capacity) item_update(&g_entities.item_data, dt);
  if (g_entities.player_data) player_update(g_entities.player_data, dt);
}

void
entities_render(void) {
#if DEV
  if (!g_entities.arena) {
    log_errorlf("%s: arena is null", __func__);
    return;
  }
#endif
  if (g_entities.item_data.capacity) item_render(&g_entities.item_data);
  if (g_entities.player_data) player_render(g_entities.player_data);
}

struct player_data *entities_get_player_data(void) {
  return g_entities.player_data;
}
