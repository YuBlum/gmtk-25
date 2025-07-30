#include "engine/arena.h"
#include "game/entities.h"
#include "game/some_entity.h"
#include "game/player.h"

struct entities {
  struct arena *arena;
  struct some_entity_data some_entity_data;
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
  if (layout->some_entity_amount) {
    g_entities.some_entity_data.amount = layout->some_entity_amount;
    g_entities.some_entity_data.position = arena_push_array(g_entities.arena, false, struct v2, layout->some_entity_amount);
    if (!g_entities.some_entity_data.position) {
      log_errorl("couldn't allocate some_entity position data");
      return false;
    }
    g_entities.some_entity_data.scale = arena_push_array(g_entities.arena, false, struct v2, layout->some_entity_amount);
    if (!g_entities.some_entity_data.scale) {
      log_errorl("couldn't allocate some_entity scale data");
      return false;
    }
    g_entities.some_entity_data.texture_position = arena_push_array(g_entities.arena, false, struct v2u, layout->some_entity_amount);
    if (!g_entities.some_entity_data.texture_position) {
      log_errorl("couldn't allocate some_entity texture_position data");
      return false;
    }
    g_entities.some_entity_data.texture_size = arena_push_array(g_entities.arena, false, struct v2u, layout->some_entity_amount);
    if (!g_entities.some_entity_data.texture_size) {
      log_errorl("couldn't allocate some_entity texture_size data");
      return false;
    }
    g_entities.some_entity_data.pivot = arena_push_array(g_entities.arena, false, struct v2, layout->some_entity_amount);
    if (!g_entities.some_entity_data.pivot) {
      log_errorl("couldn't allocate some_entity pivot data");
      return false;
    }
    g_entities.some_entity_data.angle = arena_push_array(g_entities.arena, false, float, layout->some_entity_amount);
    if (!g_entities.some_entity_data.angle) {
      log_errorl("couldn't allocate some_entity angle data");
      return false;
    }
    g_entities.some_entity_data.color = arena_push_array(g_entities.arena, false, struct color, layout->some_entity_amount);
    if (!g_entities.some_entity_data.color) {
      log_errorl("couldn't allocate some_entity color data");
      return false;
    }
    g_entities.some_entity_data.opacity = arena_push_array(g_entities.arena, false, float, layout->some_entity_amount);
    if (!g_entities.some_entity_data.opacity) {
      log_errorl("couldn't allocate some_entity opacity data");
      return false;
    }
    g_entities.some_entity_data.depth = arena_push_array(g_entities.arena, false, float, layout->some_entity_amount);
    if (!g_entities.some_entity_data.depth) {
      log_errorl("couldn't allocate some_entity depth data");
      return false;
    }
    some_entity_init(&g_entities.some_entity_data);
  } else {
    g_entities.some_entity_data.amount = 0;
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
  if (g_entities.some_entity_data.amount) some_entity_update(&g_entities.some_entity_data, dt);
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
  if (g_entities.some_entity_data.amount) some_entity_render(&g_entities.some_entity_data);
  if (g_entities.player_data) player_render(g_entities.player_data);
}
