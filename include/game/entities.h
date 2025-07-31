#ifndef __ENTITIES_H__
#define __ENTITIES_H__

#include "game/player.h"

struct entities_layout {
  uint32_t item_capacity;
  bool has_player;
};

bool entities_make(void);
bool entities_layout_set(const struct entities_layout *layout);
void entities_update(float dt);
void entities_render(void);

struct player_data *entities_get_player_data(void);

#endif/*__ENTITIES_H__*/
