#ifndef __SCENES_H__
#define __SCENES_H__

#include "engine/maps.h"

bool scene_load(enum map map);
void scene_transition_to(enum map map);
bool scene_is_in_transition(void);
void scene_update(float dt);
void scene_render(void);

#endif/*__SCENES_H__*/
