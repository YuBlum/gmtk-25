#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "engine/core.h"
#include "engine/arena.h"
#include "engine/math.h"

#define ATLAS_SIZE 512
#define ATLAS_PIXEL (1.0/ATLAS_SIZE)

#if DEV
#  include <assert.h>
static_assert(ATLAS_SIZE==512, "ATLAS_PIXEL isn't going to work. change ATLAS_SIZE and the actual pixel on the atlas position");
#endif
#define ATLAS_PIXEL_POS V2U(511, 511)

struct color { float r, g, b; };
#define RGB(r, g, b) ((struct color) { r, g, b })
#define WHITE RGB(1.0, 1.0, 1.0)
#define BLACK RGB(0.0, 0.0, 0.0)

bool renderer_make(void);
void renderer_submit(void);

void renderer_request_quads(uint32_t amount, const struct v2 positions[amount], const struct v2 sizes[amount], const struct v2u texture_positions[amount], const struct v2u texture_sizes[amount], const struct color colors[amount], const float opacities[amount], const float depths[amount]);
void renderer_request_quad(struct v2 position, struct v2 size, struct v2u texture_position, struct v2u texture_size, struct color color, float opacity, float depth);

#endif/*__RENDERER_H__*/
