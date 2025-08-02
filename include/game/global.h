#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "game/core.h"
#include "game/entities.h"

struct global {
  struct player_data player_state;
};

extern struct global global;

void global_init(void);

#endif/*__GLOBAL_H__*/
