#ifndef __ENTITIES_H__
#define __ENTITIES_H__

#include "game/player.h"
#include "game/door.h"
#include "game/box.h"
#include "game/item.h"
#include "game/solid.h"

struct entities_layout {
  uint32_t box_capacity;
  uint32_t item_capacity;
  uint32_t solid_capacity;
  bool has_player;
  bool has_door;
};

bool entities_make(void);
bool entities_layout_set(const struct entities_layout *layout);
void entities_update(float dt);
void entities_render(void);

struct player_data *entities_get_player_data(void);
struct door_data *entities_get_door_data(void);
struct box_data *entities_get_box_data(void);
struct item_data *entities_get_item_data(void);
struct solid_data *entities_get_solid_data(void);

#endif/*__ENTITIES_H__*/
