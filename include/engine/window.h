#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "engine/core.h"

enum key : uint8_t {
  K_LEFT = 0,
  K_RIGHT,
  K_UP,
  K_DOWN,
  K_A,
  K_B,
  K_EXIT,
  KEY_AMOUNT,
};

bool window_make(uint32_t width, uint32_t height);
void window_destroy(void);
void window_close();
bool window_is_running();
bool window_is_key_press(enum key key);
bool window_is_key_down(enum key key);
bool window_is_key_up(enum key key);
float window_get_delta_time(void);

#endif/*__WINDOW_H__*/
