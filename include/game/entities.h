#ifndef __ENTITIES_H__
#define __ENTITIES_H__

struct entities_layout {
  uint32_t some_entity_amount;
  bool has_player;
};

bool entities_make(void);
bool entities_layout_set(const struct entities_layout *layout);
void entities_update(float dt);
void entities_render(void);

#endif/*__ENTITIES_H__*/
