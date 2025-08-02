#include "engine/arena.h"
#include "game/entities.h"
#include "game/box.h"
#include "game/item.h"
#include "game/solid.h"
#include "game/player.h"
#include "game/door.h"

struct entities {
  struct arena *arena;
  struct box_data box_data;
  struct item_data item_data;
  struct solid_data solid_data;
  struct player_data *player_data;
  struct door_data *door_data;
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
  if (layout->box_capacity) {
    g_entities.box_data.capacity = layout->box_capacity;
    g_entities.box_data.amount   = 0;
    g_entities.box_data.position = arena_push_array(g_entities.arena, false, struct v2, layout->box_capacity);
    if (!g_entities.box_data.position) {
      log_errorl("couldn't allocate box position data");
      return false;
    }
    g_entities.box_data.sprite = arena_push_array(g_entities.arena, false, enum sprite, layout->box_capacity);
    if (!g_entities.box_data.sprite) {
      log_errorl("couldn't allocate box sprite data");
      return false;
    }
    g_entities.box_data.depth = arena_push_array(g_entities.arena, false, float, layout->box_capacity);
    if (!g_entities.box_data.depth) {
      log_errorl("couldn't allocate box depth data");
      return false;
    }
    g_entities.box_data.flash = arena_push_array(g_entities.arena, false, float, layout->box_capacity);
    if (!g_entities.box_data.flash) {
      log_errorl("couldn't allocate box flash data");
      return false;
    }
    g_entities.box_data.size = arena_push_array(g_entities.arena, false, struct v2, layout->box_capacity);
    if (!g_entities.box_data.size) {
      log_errorl("couldn't allocate box size data");
      return false;
    }
    g_entities.box_data.flash_target = arena_push_array(g_entities.arena, false, float, layout->box_capacity);
    if (!g_entities.box_data.flash_target) {
      log_errorl("couldn't allocate box flash_target data");
      return false;
    }
    g_entities.box_data.type = arena_push_array(g_entities.arena, false, enum box_type, layout->box_capacity);
    if (!g_entities.box_data.type) {
      log_errorl("couldn't allocate box type data");
      return false;
    }
    g_entities.box_data.item_drop_type = arena_push_array(g_entities.arena, false, enum item_type, layout->box_capacity);
    if (!g_entities.box_data.item_drop_type) {
      log_errorl("couldn't allocate box item_drop_type data");
      return false;
    }
    g_entities.box_data.can_drop = arena_push_array(g_entities.arena, false, bool, layout->box_capacity);
    if (!g_entities.box_data.can_drop) {
      log_errorl("couldn't allocate box can_drop data");
      return false;
    }
    box_init(&g_entities.box_data);
  } else {
    g_entities.box_data.capacity = 0;
  }
  if (layout->item_capacity) {
    g_entities.item_data.capacity = layout->item_capacity;
    g_entities.item_data.amount   = 0;
    g_entities.item_data.position = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.position) {
      log_errorl("couldn't allocate item position data");
      return false;
    }
    g_entities.item_data.scale = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.scale) {
      log_errorl("couldn't allocate item scale data");
      return false;
    }
    g_entities.item_data.sprite = arena_push_array(g_entities.arena, false, enum sprite, layout->item_capacity);
    if (!g_entities.item_data.sprite) {
      log_errorl("couldn't allocate item sprite data");
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
    g_entities.item_data.flash_target = arena_push_array(g_entities.arena, false, float, layout->item_capacity);
    if (!g_entities.item_data.flash_target) {
      log_errorl("couldn't allocate item flash_target data");
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
    g_entities.item_data.next_position = arena_push_array(g_entities.arena, false, struct v2, layout->item_capacity);
    if (!g_entities.item_data.next_position) {
      log_errorl("couldn't allocate item next_position data");
      return false;
    }
    g_entities.item_data.type = arena_push_array(g_entities.arena, false, enum item_type, layout->item_capacity);
    if (!g_entities.item_data.type) {
      log_errorl("couldn't allocate item type data");
      return false;
    }
    g_entities.item_data.timer_to_die = arena_push_array(g_entities.arena, false, float, layout->item_capacity);
    if (!g_entities.item_data.timer_to_die) {
      log_errorl("couldn't allocate item timer_to_die data");
      return false;
    }
    g_entities.item_data.box_index = arena_push_array(g_entities.arena, false, int32_t, layout->item_capacity);
    if (!g_entities.item_data.box_index) {
      log_errorl("couldn't allocate item box_index data");
      return false;
    }
  } else {
    g_entities.item_data.capacity = 0;
  }
  if (layout->solid_capacity) {
    g_entities.solid_data.capacity = layout->solid_capacity;
    g_entities.solid_data.amount   = 0;
    g_entities.solid_data.position = arena_push_array(g_entities.arena, false, struct v2, layout->solid_capacity);
    if (!g_entities.solid_data.position) {
      log_errorl("couldn't allocate solid position data");
      return false;
    }
    g_entities.solid_data.size = arena_push_array(g_entities.arena, false, struct v2, layout->solid_capacity);
    if (!g_entities.solid_data.size) {
      log_errorl("couldn't allocate solid size data");
      return false;
    }
  } else {
    g_entities.solid_data.capacity = 0;
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
  if (layout->has_door) {
    g_entities.door_data = arena_push_type(g_entities.arena, false, struct door_data);
    if (!g_entities.door_data) {
      log_errorl("couldn't allocate door data");
      return false;
    }
    door_init(g_entities.door_data);
  } else {
    g_entities.door_data = 0;
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
  if (g_entities.box_data.capacity) box_update(&g_entities.box_data, dt);
  if (g_entities.item_data.capacity) item_update(&g_entities.item_data, dt);
  if (g_entities.player_data) player_update(g_entities.player_data, dt);
  if (g_entities.door_data) door_update(g_entities.door_data, dt);
}

void
entities_render(void) {
#if DEV
  if (!g_entities.arena) {
    log_errorlf("%s: arena is null", __func__);
    return;
  }
#endif
  if (g_entities.box_data.capacity) box_render(&g_entities.box_data);
  if (g_entities.item_data.capacity) item_render(&g_entities.item_data);
  if (g_entities.solid_data.capacity) solid_render(&g_entities.solid_data);
  if (g_entities.player_data) player_render(g_entities.player_data);
  if (g_entities.door_data) door_render(g_entities.door_data);
}

struct player_data *entities_get_player_data(void) {
  return g_entities.player_data;
}

struct door_data *entities_get_door_data(void) {
  return g_entities.door_data;
}

struct box_data *entities_get_box_data(void) {
  return &g_entities.box_data;
}

struct item_data *entities_get_item_data(void) {
  return &g_entities.item_data;
}

struct solid_data *entities_get_solid_data(void) {
  return &g_entities.solid_data;
}
