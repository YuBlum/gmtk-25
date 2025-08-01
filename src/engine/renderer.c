#if DEV
#  include <assert.h>
#endif
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine/core.h"
#include "engine/string.h"
#include "engine/shaders.h"
#include "engine/renderer.h"
#include "engine/atlas.h"
#include "game/maps.h"

struct vertex {
  struct v2    position;
  struct v2    texcoord;
  struct v2    origin;
  struct v2    angle;
  struct color color;
  float        opacity;
  float        flash;
};

#define QUAD_CAPACITY 10000
#define INDEX_CAPACITY (QUAD_CAPACITY*6)
#define VERTEX_CAPACITY (QUAD_CAPACITY*4)

struct quad_vertices {
  struct vertex v[4];
};

struct quad_indices {
  uint32_t i[6];
};

struct index_sort {
  uint32_t start;
  float depth;
};

struct renderer {
  struct quad_vertices vertices[QUAD_CAPACITY];
  struct quad_indices indices[QUAD_CAPACITY];
  struct index_sort indices_to_sort[QUAD_CAPACITY];
#if DEV
  struct quad_vertices vertices_circle[QUAD_CAPACITY];
  struct quad_indices indices_circle[QUAD_CAPACITY];
  uint32_t circles_amount;
#endif
  struct v2 offset;
  uint32_t quads_amount;
  uint32_t sh_default;
  int32_t  sh_default_proj;
  uint32_t texture_atlas;
#if DEV
  uint32_t sh_circle;
  int32_t  sh_circle_proj;
#endif
};

static struct renderer g_renderer;
static float g_projection[3*3] = {
  +2.0f/(GAME_RIGHT-GAME_LEFT)                  , +0.0f                                         , +0.0f,
  +0.0f                                         , +2.0f/(GAME_TOP-GAME_BOTTOM)                  , +0.0f,
  -(GAME_RIGHT+GAME_LEFT)/(GAME_RIGHT-GAME_LEFT), -(GAME_TOP+GAME_BOTTOM)/(GAME_TOP-GAME_BOTTOM), +1.0f,
};

#define SHADER_LOG_CAPACITY 512

static uint32_t
shader_make(GLenum type, const struct str_view *src) {
#if DEV
  const char *shader_str = 0;
  switch (type) {
    case GL_VERTEX_SHADER: {
      shader_str = "vertex";
    } break;
    case GL_FRAGMENT_SHADER: {
      shader_str = "fragment";
    } break;
    default: {
      log_errorlf("%s: unknown shader type", __func__);
      return 0;
    } break;
  }
#endif
  uint32_t shader = glCreateShader(type);
  if (!shader) {
    log_errorlf("%s: couldn't make shader", __func__);
    return 0;
  }
  const int len = src->length;
  glShaderSource(shader, 1, &src->data, &len);
  glCompileShader(shader);
#if DEV
  int status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (!status) {
    int log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > SHADER_LOG_CAPACITY) {
      log_warnlf("%s: shader compile error will not be fully displayed. increase SHADER_LOG_CAPACITY", __func__);
    }
    char log[SHADER_LOG_CAPACITY];
    glGetShaderInfoLog(shader, SHADER_LOG_CAPACITY, 0, log);
    log_errorf("%s shader: %s", shader_str, log);
    glDeleteShader(shader);
    return 0;
  }
#endif
  return shader;
}

static uint32_t
shader_program_make(const struct str_view *vert_src, const struct str_view *frag_src) {
  log_infolf("%s: starting shader program creation...", __func__);
  uint32_t program = glCreateProgram();
  if (!program) {
    log_errorlf("%s: couldn't make shader program", __func__);
    return 0;
  }
  uint32_t vert = shader_make(GL_VERTEX_SHADER, vert_src);
  if (!vert) return 0;
  log_infolf("%s: compiled vertex shader", __func__);
  uint32_t frag = shader_make(GL_FRAGMENT_SHADER, frag_src);
  if (!frag) return 0;
  log_infolf("%s: compiled fragment shader", __func__);
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);
  glDeleteShader(vert);
  glDeleteShader(frag);
#if DEV
  int status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (!status) {
    int log_length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    if (log_length > SHADER_LOG_CAPACITY) {
      log_warnlf("%s: shader linking error will not be fully displayed. increase SHADER_LOG_CAPACITY", __func__);
    }
    char log[SHADER_LOG_CAPACITY];
    glGetProgramInfoLog(program, SHADER_LOG_CAPACITY, 0, log);
    log_errorf("shader program linking: %s", log);
    glDeleteProgram(program);
    return 0;
  }
#endif
  return program;
}

bool
renderer_make(void) {
  log_infol("making renderer...");
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  log_infol("loaded opengl functions");
  g_renderer.sh_default = shader_program_make(&SH_DEFAULT_VERT, &SH_DEFAULT_FRAG);
  if (!g_renderer.sh_default) return false;
  g_renderer.sh_default_proj = glGetUniformLocation(g_renderer.sh_default, "u_proj");
#if DEV
  if (g_renderer.sh_default_proj < 0) {
    log_errorl("couldn't get 'u_proj' location from default shader");
    return false;
  }
#endif
  glUseProgram(g_renderer.sh_default);
  glUniformMatrix3fv(g_renderer.sh_default_proj, 1, false, g_projection);
  log_infol("created default shader");
#if DEV
  g_renderer.sh_circle = shader_program_make(&SH_CIRCLE_VERT, &SH_CIRCLE_FRAG);
  if (!g_renderer.sh_circle) return false;
  g_renderer.sh_circle_proj = glGetUniformLocation(g_renderer.sh_circle, "u_proj");
  if (g_renderer.sh_circle_proj < 0) {
    log_errorl("couldn't get 'u_proj' location from circle shader");
    return false;
  }
  glUseProgram(g_renderer.sh_circle);
  glUniformMatrix3fv(g_renderer.sh_circle_proj, 1, false, g_projection);
  for (uint32_t i = 0; i < QUAD_CAPACITY; i++) {
    g_renderer.indices_circle[i].i[0] = (i * 4) + 0;
    g_renderer.indices_circle[i].i[1] = (i * 4) + 1;
    g_renderer.indices_circle[i].i[2] = (i * 4) + 2;
    g_renderer.indices_circle[i].i[3] = (i * 4) + 2;
    g_renderer.indices_circle[i].i[4] = (i * 4) + 3;
    g_renderer.indices_circle[i].i[5] = (i * 4) + 0;
  }
  log_infol("created circle shader");
#endif
  glGenTextures(1, &g_renderer.texture_atlas);
  glBindTexture(GL_TEXTURE_2D, g_renderer.texture_atlas);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ATLAS_WIDTH, ATLAS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, g_atlas_data);
  log_infol("loaded texture atlas");
  uint32_t vao, vbo, ibo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof (g_renderer.vertices), 0, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (g_renderer.indices), 0, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, position));
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, texcoord));
  glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, origin));
  glVertexAttribPointer(3, 2, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, angle));
  glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, color));
  glVertexAttribPointer(5, 1, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, opacity));
  glVertexAttribPointer(6, 1, GL_FLOAT, false, sizeof (struct vertex), (void *)offsetof (struct vertex, flash));
  log_infol("vao, vbo and ibo created successfully");
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  g_renderer.offset = V2S(0.0f);
  log_infol("renderer creation complete!");
  return true;
}

struct v2
renderer_get_offset(void) {
  return g_renderer.offset;
}

void
renderer_set_offset(struct v2 offset) {
  g_renderer.offset = offset;
}

static int
compare_indices(const void *r0, const void *r1) {
  float d0 = ((const struct index_sort *)r0)->depth;
  float d1 = ((const struct index_sort *)r1)->depth;
  if (d1 < d0) return -1;
  if (d1 > d0) return +1;
  return 0;
}

void
renderer_submit(void) {
  // NOTE: maybe instead of sorting and resetting the batch every frame you can have 'permanent' quads
  qsort(g_renderer.indices_to_sort, g_renderer.quads_amount, sizeof (struct index_sort), compare_indices);
  for (uint32_t i = 0; i < g_renderer.quads_amount; i++) {
    g_renderer.indices[i].i[0] = g_renderer.indices_to_sort[i].start + 0;
    g_renderer.indices[i].i[1] = g_renderer.indices_to_sort[i].start + 1;
    g_renderer.indices[i].i[2] = g_renderer.indices_to_sort[i].start + 2;
    g_renderer.indices[i].i[3] = g_renderer.indices_to_sort[i].start + 2;
    g_renderer.indices[i].i[4] = g_renderer.indices_to_sort[i].start + 3;
    g_renderer.indices[i].i[5] = g_renderer.indices_to_sort[i].start + 0;
  }
  for (uint32_t i = 0; i < g_renderer.quads_amount; i++) {
    g_renderer.vertices[i].v[0].position = v2_sub(g_renderer.vertices[i].v[0].position, g_renderer.offset);
    g_renderer.vertices[i].v[1].position = v2_sub(g_renderer.vertices[i].v[1].position, g_renderer.offset);
    g_renderer.vertices[i].v[2].position = v2_sub(g_renderer.vertices[i].v[2].position, g_renderer.offset);
    g_renderer.vertices[i].v[3].position = v2_sub(g_renderer.vertices[i].v[3].position, g_renderer.offset);
  }
  glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(g_renderer.sh_default);
  glBufferSubData(GL_ARRAY_BUFFER, 0, g_renderer.quads_amount * sizeof (struct quad_vertices), g_renderer.vertices);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, g_renderer.quads_amount * sizeof (struct quad_indices), g_renderer.indices);
  glDrawElements(GL_TRIANGLES, g_renderer.quads_amount * 6, GL_UNSIGNED_INT, 0);
  g_renderer.quads_amount = 0;
#if DEV
  glUseProgram(g_renderer.sh_circle);
  glBufferSubData(GL_ARRAY_BUFFER, 0, g_renderer.circles_amount * sizeof (struct quad_vertices), g_renderer.vertices_circle);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, g_renderer.circles_amount * sizeof (struct quad_indices), g_renderer.indices_circle);
  glDrawElements(GL_TRIANGLES, g_renderer.circles_amount * 6, GL_UNSIGNED_INT, 0);
  g_renderer.circles_amount = 0;
#endif
}

void
renderer_request_sprites(uint32_t amount, const enum sprite sprites[amount], const struct v2 positions[amount], const struct v2 origins[amount], const float angles[amount], const struct v2 scales[amount], const struct color colors[amount], const float opacities[amount], const float depths[amount], const float flashes[amount]) {
#if DEV
  if (g_renderer.quads_amount + amount >= QUAD_CAPACITY) {
    log_warnlf("%s: trying to request to much quads for rendering. increase QUAD_CAPACITY", __func__);
    return;
  }
#endif
  struct v2 hsiz, tpos, tsiz, cos_sin;
  static_assert(sizeof (struct vertex) == sizeof (float) * 13);
#if DEV
  if (!sprites) {
    log_errorlf("%s: 'sprites' argument cannot be NULL", __func__);
    return;
  }
  for (uint32_t i = 0; i < amount; i++) {
    if (sprites[i] < SPRITES_AMOUNT) continue;
    log_warnlf("%s: passed sprite at index '%d', has invalid sprite number '%d' exists", __func__, i, sprites[i]);
    return;
  }
#endif
  for (uint32_t i = 0; i < amount; i++) {
    hsiz = g_atlas_sprite_half_sizes[sprites[i]];
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].origin = V2(-hsiz.x, -hsiz.y);
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].origin = V2(+hsiz.x, -hsiz.y);
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].origin = V2(+hsiz.x, +hsiz.y);
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].origin = V2(-hsiz.x, +hsiz.y);
  }
  if (origins) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount+i].v[0].origin = v2_add(origins[i], g_renderer.vertices[g_renderer.quads_amount+i].v[0].origin);
      g_renderer.vertices[g_renderer.quads_amount+i].v[1].origin = v2_add(origins[i], g_renderer.vertices[g_renderer.quads_amount+i].v[1].origin);
      g_renderer.vertices[g_renderer.quads_amount+i].v[2].origin = v2_add(origins[i], g_renderer.vertices[g_renderer.quads_amount+i].v[2].origin);
      g_renderer.vertices[g_renderer.quads_amount+i].v[3].origin = v2_add(origins[i], g_renderer.vertices[g_renderer.quads_amount+i].v[3].origin);
    }
  }
  if (scales) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount+i].v[0].origin = v2_mul(scales[i], g_renderer.vertices[g_renderer.quads_amount+i].v[0].origin);
      g_renderer.vertices[g_renderer.quads_amount+i].v[1].origin = v2_mul(scales[i], g_renderer.vertices[g_renderer.quads_amount+i].v[1].origin);
      g_renderer.vertices[g_renderer.quads_amount+i].v[2].origin = v2_mul(scales[i], g_renderer.vertices[g_renderer.quads_amount+i].v[2].origin);
      g_renderer.vertices[g_renderer.quads_amount+i].v[3].origin = v2_mul(scales[i], g_renderer.vertices[g_renderer.quads_amount+i].v[3].origin);
    }
  }
#if DEV
  if (!positions) {
    log_errorlf("%s: 'positions' argument cannot be NULL", __func__);
    return;
  }
#endif
  for (uint32_t i = 0; i < amount; i++) {
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].position = positions[i];
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].position = positions[i];
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].position = positions[i];
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].position = positions[i];
  }
  for (uint32_t i = 0; i < amount; i++) {
    tpos = g_atlas_sprite_positions[sprites[i]];
    tsiz = g_atlas_sprite_sizes[sprites[i]];
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].texcoord = v2_add(tpos, V2(0.0f  , tsiz.y));
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].texcoord = v2_add(tpos, V2(tsiz.x, tsiz.y));
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].texcoord = v2_add(tpos, V2(tsiz.x, 0.0f  ));
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].texcoord = v2_add(tpos, V2(0.0f  , 0.0f  ));
  }
  if (angles) {
    for (uint32_t i = 0; i < amount; i++) {
      cos_sin = V2(cosf(angles[i]), sinf(angles[i]));
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].angle = cos_sin;
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].angle = cos_sin;
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].angle = cos_sin;
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].angle = cos_sin;
    }
  } else {
    cos_sin = V2(cosf(0.0f), sinf(0.0f));
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].angle = cos_sin;
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].angle = cos_sin;
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].angle = cos_sin;
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].angle = cos_sin;
    }
  }
  if (colors) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].color = colors[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].color = colors[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].color = colors[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].color = colors[i];
    }
  } else {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].color = WHITE;
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].color = WHITE;
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].color = WHITE;
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].color = WHITE;
    }
  }
  if (opacities) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].opacity = opacities[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].opacity = opacities[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].opacity = opacities[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].opacity = opacities[i];
    }
  } else {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].opacity = 1.0f;
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].opacity = 1.0f;
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].opacity = 1.0f;
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].opacity = 1.0f;
    }
  }
  if (flashes) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].flash = flashes[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].flash = flashes[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].flash = flashes[i];
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].flash = flashes[i];
    }
  } else {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.vertices[g_renderer.quads_amount + i].v[0].flash = 0.0f;
      g_renderer.vertices[g_renderer.quads_amount + i].v[1].flash = 0.0f;
      g_renderer.vertices[g_renderer.quads_amount + i].v[2].flash = 0.0f;
      g_renderer.vertices[g_renderer.quads_amount + i].v[3].flash = 0.0f;
    }
  }
  if (depths) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].depth = depths[i];
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].start = (g_renderer.quads_amount + i) * 4;
    }
  } else {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].depth = 0.0f;
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].start = (g_renderer.quads_amount + i) * 4;
    }
  }
  g_renderer.quads_amount += amount;
}

void
renderer_request_tileset(uint32_t amount, enum sprite tileset, struct v2 offset, const struct v2u tileset_offset[amount], const struct v2 positions[amount], const float depths[amount]) {
#if DEV
  if (g_renderer.quads_amount + amount >= QUAD_CAPACITY) {
    log_warnlf("%s: trying to request to much quads for rendering. increase QUAD_CAPACITY", __func__);
    return;
  }
#endif
  struct v2 hsiz, tpos, tsiz, cos_sin, pos;
  static_assert(sizeof (struct vertex) == sizeof (float) * 13);
#if DEV
  if (tileset >= SPRITES_AMOUNT) {
    log_warnlf("%s: passed tileset has invalid sprite number '%d' exists", __func__, tileset);
    return;
  }
#endif
  hsiz = V2(TILE_WIDTH * 0.5f * UNIT_ONE_PIXEL, TILE_HEIGHT * 0.5f * UNIT_ONE_PIXEL);
  for (uint32_t i = 0; i < amount; i++) {
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].origin = V2(-hsiz.x, -hsiz.y);
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].origin = V2(+hsiz.x, -hsiz.y);
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].origin = V2(+hsiz.x, +hsiz.y);
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].origin = V2(-hsiz.x, +hsiz.y);
  }
#if DEV
  if (!positions) {
    log_errorlf("%s: 'positions' argument cannot be NULL", __func__);
    return;
  }
#endif
  for (uint32_t i = 0; i < amount; i++) {
    pos = v2_sub(positions[i], offset);
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].position = pos;
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].position = pos;
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].position = pos;
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].position = pos;
  }
  tsiz = V2(TILE_WIDTH * ATLAS_PIXEL_W, TILE_HEIGHT * ATLAS_PIXEL_H);
  for (uint32_t i = 0; i < amount; i++) {
    tpos = v2_add(g_atlas_sprite_positions[tileset], V2(tileset_offset[i].x * ATLAS_PIXEL_W, tileset_offset[i].y * ATLAS_PIXEL_H));
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].texcoord = v2_add(tpos, V2(0.0f  , tsiz.y));
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].texcoord = v2_add(tpos, V2(tsiz.x, tsiz.y));
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].texcoord = v2_add(tpos, V2(tsiz.x, 0.0f  ));
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].texcoord = v2_add(tpos, V2(0.0f  , 0.0f  ));
  }
  cos_sin = V2(cosf(0.0f), sinf(0.0f));
  for (uint32_t i = 0; i < amount; i++) {
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].angle = cos_sin;
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].angle = cos_sin;
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].angle = cos_sin;
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].angle = cos_sin;
  }
  for (uint32_t i = 0; i < amount; i++) {
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].color = WHITE;
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].color = WHITE;
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].color = WHITE;
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].color = WHITE;
  }
  for (uint32_t i = 0; i < amount; i++) {
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].opacity = 1.0f;
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].opacity = 1.0f;
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].opacity = 1.0f;
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].opacity = 1.0f;
  }
  for (uint32_t i = 0; i < amount; i++) {
    g_renderer.vertices[g_renderer.quads_amount + i].v[0].flash = 0.0f;
    g_renderer.vertices[g_renderer.quads_amount + i].v[1].flash = 0.0f;
    g_renderer.vertices[g_renderer.quads_amount + i].v[2].flash = 0.0f;
    g_renderer.vertices[g_renderer.quads_amount + i].v[3].flash = 0.0f;
  }
  if (depths) {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].depth = depths[i];
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].start = (g_renderer.quads_amount + i) * 4;
    }
  } else {
    for (uint32_t i = 0; i < amount; i++) {
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].depth = 1000.0f;
      g_renderer.indices_to_sort[g_renderer.quads_amount + i].start = (g_renderer.quads_amount + i) * 4;
    }
  }
  g_renderer.quads_amount += amount;
}

void
renderer_request_sprite(enum sprite sprite, struct v2 position, struct v2 origin, float angle, struct v2 scale, struct color color, float opacity, float depth, float flash) {
#if DEV
  if (g_renderer.quads_amount + 1 >= QUAD_CAPACITY) {
    log_warnlf("%s: trying to request to much quads for rendering. increase QUAD_CAPACITY", __func__);
    return;
  }
  if (sprite >= SPRITES_AMOUNT) {
    log_warnlf("%s: sprite with number '%d' doesn't exists", __func__, sprite);
    return;
  }
#endif
  static_assert(sizeof (struct vertex) == sizeof (float) * 13);
  struct v2 hsiz = g_atlas_sprite_half_sizes[sprite],
            tpos = g_atlas_sprite_positions[sprite],
            tsiz = g_atlas_sprite_sizes[sprite],
            cos_sin = { cosf(angle), sinf(angle) };
  struct vertex *vertices = g_renderer.vertices[g_renderer.quads_amount].v;
  vertices[0].position = position;
  vertices[1].position = position;
  vertices[2].position = position;
  vertices[3].position = position;
  vertices[0].texcoord = v2_add(tpos, V2(0.0f  , tsiz.y));
  vertices[1].texcoord = v2_add(tpos, V2(tsiz.x, tsiz.y));
  vertices[2].texcoord = v2_add(tpos, V2(tsiz.x, 0.0f  ));
  vertices[3].texcoord = v2_add(tpos, V2(0.0f  , 0.0f  ));
  vertices[0].origin = v2_mul(v2_add(origin, V2(-hsiz.x, -hsiz.y)), scale);
  vertices[1].origin = v2_mul(v2_add(origin, V2(+hsiz.x, -hsiz.y)), scale);
  vertices[2].origin = v2_mul(v2_add(origin, V2(+hsiz.x, +hsiz.y)), scale);
  vertices[3].origin = v2_mul(v2_add(origin, V2(-hsiz.x, +hsiz.y)), scale);
  vertices[0].angle = cos_sin;
  vertices[1].angle = cos_sin;
  vertices[2].angle = cos_sin;
  vertices[3].angle = cos_sin;
  vertices[0].color = color;
  vertices[1].color = color;
  vertices[2].color = color;
  vertices[3].color = color;
  vertices[0].opacity = opacity;
  vertices[1].opacity = opacity;
  vertices[2].opacity = opacity;
  vertices[3].opacity = opacity;
  vertices[0].flash = flash;
  vertices[1].flash = flash;
  vertices[2].flash = flash;
  vertices[3].flash = flash;
  g_renderer.indices_to_sort[g_renderer.quads_amount].depth = depth;
  g_renderer.indices_to_sort[g_renderer.quads_amount].start = g_renderer.quads_amount * 4;
  g_renderer.quads_amount++;
}

void
renderer_request_sprite_slice(enum sprite sprite, struct v2u top_left, struct v2u size, struct v2 position, struct v2 origin, float angle, struct v2 scale, struct color color, float opacity, float depth, float flash) {
#if DEV
  if (g_renderer.quads_amount + 1 >= QUAD_CAPACITY) {
    log_warnlf("%s: trying to request to much quads for rendering. increase QUAD_CAPACITY", __func__);
    return;
  }
  if (sprite >= SPRITES_AMOUNT) {
    log_warnlf("%s: sprite with number '%d' doesn't exists", __func__, sprite);
    return;
  }
#endif
  static_assert(sizeof (struct vertex) == sizeof (float) * 13);
  struct v2 hsiz = V2(size.x * 0.5f * UNIT_ONE_PIXEL, size.y * 0.5f * UNIT_ONE_PIXEL),
            tpos = v2_add(g_atlas_sprite_positions[sprite], V2(top_left.x * ATLAS_PIXEL_W, top_left.y * ATLAS_PIXEL_H)),
            tsiz = V2(size.x * ATLAS_PIXEL_W, size.y * ATLAS_PIXEL_H),
            cos_sin = { cosf(angle), sinf(angle) };
  struct vertex *vertices = g_renderer.vertices[g_renderer.quads_amount].v;
  vertices[0].position = position;
  vertices[1].position = position;
  vertices[2].position = position;
  vertices[3].position = position;
  vertices[0].texcoord = v2_add(tpos, V2(0.0f  , tsiz.y));
  vertices[1].texcoord = v2_add(tpos, V2(tsiz.x, tsiz.y));
  vertices[2].texcoord = v2_add(tpos, V2(tsiz.x, 0.0f  ));
  vertices[3].texcoord = v2_add(tpos, V2(0.0f  , 0.0f  ));
  vertices[0].origin = v2_mul(v2_add(origin, V2(-hsiz.x, -hsiz.y)), scale);
  vertices[1].origin = v2_mul(v2_add(origin, V2(+hsiz.x, -hsiz.y)), scale);
  vertices[2].origin = v2_mul(v2_add(origin, V2(+hsiz.x, +hsiz.y)), scale);
  vertices[3].origin = v2_mul(v2_add(origin, V2(-hsiz.x, +hsiz.y)), scale);
  vertices[0].angle = cos_sin;
  vertices[1].angle = cos_sin;
  vertices[2].angle = cos_sin;
  vertices[3].angle = cos_sin;
  vertices[0].color = color;
  vertices[1].color = color;
  vertices[2].color = color;
  vertices[3].color = color;
  vertices[0].opacity = opacity;
  vertices[1].opacity = opacity;
  vertices[2].opacity = opacity;
  vertices[3].opacity = opacity;
  vertices[0].flash = flash;
  vertices[1].flash = flash;
  vertices[2].flash = flash;
  vertices[3].flash = flash;
  g_renderer.indices_to_sort[g_renderer.quads_amount].depth = depth;
  g_renderer.indices_to_sort[g_renderer.quads_amount].start = g_renderer.quads_amount * 4;
  g_renderer.quads_amount++;
}

#if DEV
void
renderer_request_circle(struct v2 position, float radius, struct color color, float opacity) {
  if (g_renderer.circles_amount + 1 >= QUAD_CAPACITY) {
    log_warnlf("%s: trying to request to much circles for rendering. increase QUAD_CAPACITY", __func__);
    return;
  }
  static_assert(sizeof (struct vertex) == sizeof (float) * 13);
  struct vertex *vertices = g_renderer.vertices_circle[g_renderer.circles_amount].v;
  vertices[0].position = v2_add(position, V2(-radius, -radius));
  vertices[1].position = v2_add(position, V2(+radius, -radius));
  vertices[2].position = v2_add(position, V2(+radius, +radius));
  vertices[3].position = v2_add(position, V2(-radius, +radius));
  vertices[0].origin = V2(-1.0f, -1.0f); /* treating the 'origin' like normalized coordinates */
  vertices[1].origin = V2(+1.0f, -1.0f); /* treating the 'origin' like normalized coordinates */
  vertices[2].origin = V2(+1.0f, +1.0f); /* treating the 'origin' like normalized coordinates */
  vertices[3].origin = V2(-1.0f, +1.0f); /* treating the 'origin' like normalized coordinates */
  vertices[0].color = color;
  vertices[1].color = color;
  vertices[2].color = color;
  vertices[3].color = color;
  vertices[0].opacity = opacity;
  vertices[1].opacity = opacity;
  vertices[2].opacity = opacity;
  vertices[3].opacity = opacity;
  g_renderer.circles_amount++;
}

void
renderer_request_rect(struct v2 position, struct v2 size, struct color color, float opacity, float depth) {
  renderer_request_sprite(
    SPR_PIXEL,
    position,
    V2S(0.0f),
    0.0f,
    v2_mul(size, V2(UNIT_PER_PIXEL, UNIT_PER_PIXEL)),
    color,
    opacity,
    depth,
    0.0f
  );
}
#endif
