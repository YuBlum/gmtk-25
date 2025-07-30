#include <stdlib.h>
#include <time.h>
#include "engine/arena.h"
#include "engine/window.h"
#include "engine/mixer.h"
#include "engine/renderer.h"
#include "game/entities.h"
#include "game/player.h"

#define WINDOW_W (GAME_W_PIXEL * GAME_S)
#define WINDOW_H (GAME_H_PIXEL * GAME_S)

int
main(void) {
  srand(time(0));
  struct arena *tmp_arena = arena_make(0, 0);
  if (!tmp_arena) {
    log_errorl("couldn't make main temporary arena");
    return 1;
  }
  if (!window_make(WINDOW_W, WINDOW_H)) return 1;
  if (!mixer_make()) {
    window_destroy();
    return 1;
  }
  if (!renderer_make()) {
    mixer_destroy();
    window_destroy();
    return 1;
  }
  if (!entities_make()) {
    mixer_destroy();
    window_destroy();
    return 1;
  }
  if (!entities_layout_set(&(struct entities_layout) {
    .has_player = true,
  })) {
    mixer_destroy();
    window_destroy();
    return 1;
  } // TODO: remove this from here
  while (window_is_running()) {
    if (window_is_key_down(K_EXIT)) window_close();
    entities_update(window_get_delta_time());
    entities_render();
    renderer_submit();
    arena_clear(tmp_arena);
  }
  mixer_destroy();
  window_destroy();
  return 0;
}
