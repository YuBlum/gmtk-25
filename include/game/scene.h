#ifndef __SCENE_H__
#define __SCENE_H__

#include "game/maps.h"

bool scene_load(enum map map);
void scene_transition_to(enum map map);
bool scene_is_in_transition(void);
void scene_update(float dt);
void scene_render(void);

#endif/*__SCENE_H__*/
