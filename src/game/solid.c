#include "engine/window.h"
#include "game/entities.h"

#if DEV
extern bool show_colliders;
#endif

void
solid_render(struct solid_data *self) {
  (void)self;
#if DEV
  for (uint32_t i = 0; i < self->amount; i++)
    if (show_colliders) renderer_request_rect(self->position[i], self->size[i], GREEN, 0.4f, -100.0f);
#endif
}
