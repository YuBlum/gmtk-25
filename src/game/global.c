#include "game/core.h"
#include "game/global.h"

struct global global;

void
global_init(void) {
  //static_assert(sizeof (struct global) == 20, "added global variable but didn't set it up");
  global.player_state.sprite        = SPR_PLAYER;
  global.player_state.position      = V2(0.00f, 0.00f);
  global.player_state.size          = V2(1.00f, 0.75f);
  global.player_state.scale         = V2(1.00f, 1.00f);
  global.player_state.origin        = V2(0.00f, 0.45f);
  global.player_state.interact_pos  = v2_add(global.player_state.position, global.player_state.origin);
  global.player_state.interact_rad  = 1.5f;
  global.player_state.angle         = 0.0f;
  global.player_state.wiggle_cur    = 0.0f;
  global.player_state.wiggle_target = 0.0f;
  global.player_state.depth         = 0.0f;
  global.player_state.item_held     = -1;
}
