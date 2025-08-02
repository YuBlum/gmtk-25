#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "game/core.h"
#include "game/entities.h"

struct global {
  struct player_data player_state;
  struct v2 extra_item_position;
  enum item_type extra_item_type;
  enum sprite    extra_item_sprite; /* this is needed for the random trash */
  enum item_type next_item_type;
  enum room_layout next_room_layout;
  int32_t layout_box, content_box;
  bool going_out;
  bool end;
};

extern struct global global;

void global_init(void);

#endif/*__GLOBAL_H__*/
