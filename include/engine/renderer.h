#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "engine/core.h"
#include "engine/arena.h"
#include "engine/math.h"
#include "engine/sprites.h"

struct color { float r, g, b; };
#define RGB(r, g, b) ((struct color) { r, g, b })
#define WHITE RGB(1.0f, 1.0f, 1.0f)
#define RED   RGB(1.0f, 0.0f, 0.0f)
#define GREEN RGB(0.0f, 1.0f, 0.0f)
#define BLUE  RGB(0.0f, 0.0f, 1.0f)
#define BLACK RGB(0.0f, 0.0f, 0.0f)

bool renderer_make(void);
void renderer_submit(void);

void renderer_request_sprites(uint32_t amount, const enum sprite sprites[amount], const struct v2 positions[amount], const struct v2 origins[amount], const float angles[amount], const struct v2 scales[amount], const struct color colors[amount], const float opacities[amount], const float depths[amount], const float flashes[amount]);
void renderer_request_tileset(uint32_t amount, enum sprite tileset, const struct v2u tileset_offset[amount], const struct v2 positions[amount], const float depths[amount]);

void renderer_request_sprite(enum sprite sprite, struct v2 position, struct v2 origin, float angle, struct v2 scale, struct color color, float opacity, float depth, float flash);
void renderer_request_sprite_slice(enum sprite sprite, struct v2u top_left, struct v2u size, struct v2 position, struct v2 origin, float angle, struct v2 scale, struct color color, float opacity, float depth, float flash);

#if DEV
void renderer_request_circle(struct v2 position, float radius, struct color color, float opacity);
void renderer_request_rect(struct v2 position, struct v2 size, struct color color, float opacity, float depth);
#endif

#endif/*__RENDERER_H__*/
