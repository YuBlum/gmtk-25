#include "engine/window.h"
#include "engine/renderer.h"

struct helper_button {
  enum key key;
  enum sprite sprite;
  float opacity;
  float oscillate_timer;
  bool pressed;
};

#define BUTTONS_AMOUNT 5

struct helper {
  struct helper_button buttons[BUTTONS_AMOUNT];
  float timer_to_r;
  float r_opacity;
  bool r_pressed;
};

static struct helper g_helper = {
  .buttons = {
    { .key = K_LEFT,     .sprite = SPR_KEY_LEFT,  .opacity = 1.0f, .oscillate_timer = 0.0f, .pressed = false },
    { .key = K_UP,       .sprite = SPR_KEY_UP,    .opacity = 1.0f, .oscillate_timer = 0.5f, .pressed = false },
    { .key = K_RIGHT,    .sprite = SPR_KEY_RIGHT, .opacity = 1.0f, .oscillate_timer = 1.0f, .pressed = false },
    { .key = K_DOWN,     .sprite = SPR_KEY_DOWN,  .opacity = 1.0f, .oscillate_timer = 1.5f, .pressed = false },
    { .key = K_INTERACT, .sprite = SPR_KEY_Z,     .opacity = 1.0f, .oscillate_timer = 2.0f, .pressed = false },
  },
  .timer_to_r = 0.0f,
  .r_opacity = 0.0f,
  .r_pressed = false,
};


#define MAIN_POSITION V2(-GAME_W * 0.5f + 3.0f, -GAME_H * 0.5f + 3.0f)
#define R_POSITION    V2(+GAME_W * 0.5f - 3.0f, -GAME_H * 0.5f + 3.0f)
#define GAP V2(2.5f, 0.0f)

#define OSCILLATE_SPEED 3.0f
#define OSCILLATE_MAGNITUDE 0.25f

#define BUTTON_OPACITY_SPEED 5.0f

#define SECONDS_TO_R 1.0f

void
helper_update(float dt) {
  for (uint32_t i = 0; i < BUTTONS_AMOUNT; i++) {
    g_helper.buttons[i].oscillate_timer += dt;
    if (window_is_key_down(g_helper.buttons[i].key)) {
      g_helper.buttons[i].pressed = true;
      g_helper.timer_to_r = 0.0f;
      g_helper.r_pressed = true;
    }
    if (g_helper.buttons[i].pressed) {
      g_helper.buttons[i].opacity -= BUTTON_OPACITY_SPEED * dt;
      if (g_helper.buttons[i].opacity < 0.0f) g_helper.buttons[i].opacity = 0.0f;
    }
  }
  if (window_is_key_press(K_RESTART)) {
    g_helper.r_pressed = true;
    g_helper.timer_to_r = 0.0f;
  }
  if (g_helper.r_pressed) {
    g_helper.r_opacity -= BUTTON_OPACITY_SPEED * dt;
    if (g_helper.r_opacity < 0.0f) g_helper.r_opacity = 0.0f;
  }
  g_helper.timer_to_r += dt;
  if (g_helper.timer_to_r >= SECONDS_TO_R) {
    g_helper.r_opacity = 1.0f;
    g_helper.r_pressed = false;
  }
}

void
helper_render(void) {
  for (uint32_t i = 0; i < BUTTONS_AMOUNT; i++) {
    renderer_request_sprite(
      g_helper.buttons[i].sprite,
      v2_add(v2_add(MAIN_POSITION, v2_muls(GAP, i)), V2(0.0f, sinf(g_helper.buttons[i].oscillate_timer * OSCILLATE_SPEED) * OSCILLATE_MAGNITUDE)),
      V2S(0.0f),
      0.0f,
      V2S(1.0f),
      WHITE,
      g_helper.buttons[i].opacity,
      -10000.0f,
      0.0f
    );
  }
  renderer_request_sprite(
    SPR_KEY_R,
    R_POSITION,
    V2S(0.0f),
    0.0f,
    V2S(1.0f),
    WHITE,
    g_helper.r_opacity,
    -10000.0f,
    0.0f
  );
}
