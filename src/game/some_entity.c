#include "game/some_entity.h"

void
some_entity_init(struct some_entity_data *self) {
  (void)self;
  log_warnlf("%s: not implemented", __func__);
}

void
some_entity_update(struct some_entity_data *self, float dt) {
  (void)self; (void)dt;
  log_warnlf("%s: not implemented", __func__);
}

void
some_entity_render(struct some_entity_data *self) {
  (void)self;
  log_warnlf("%s: not implemented", __func__);
}
