#include <string.h>
#include <GLFW/glfw3.h>
#include "engine/core.h"
#include "engine/window.h"

struct window {
  GLFWwindow *handle;
  uint32_t width;
  uint32_t height;
  bool keys[KEY_AMOUNT];
  bool pkeys[KEY_AMOUNT];
  float time;
};

static struct window g_window;

static void
key_callback(GLFWwindow* _window, int key, int _scancode, int action, int _mods) {
  (void)_window; (void)_scancode; (void) _mods;
  if (action != GLFW_PRESS && action != GLFW_RELEASE) return;
  switch (key) {
    case GLFW_KEY_ESCAPE: g_window.keys[K_EXIT]      = action == GLFW_PRESS; break;
    case GLFW_KEY_RIGHT:  g_window.keys[K_RIGHT]     = action == GLFW_PRESS; break;
    case GLFW_KEY_LEFT:   g_window.keys[K_LEFT]      = action == GLFW_PRESS; break;
    case GLFW_KEY_UP:     g_window.keys[K_UP]        = action == GLFW_PRESS; break;
    case GLFW_KEY_DOWN:   g_window.keys[K_DOWN]      = action == GLFW_PRESS; break;
    case GLFW_KEY_Z:      g_window.keys[K_INTERACT]  = action == GLFW_PRESS; break;
    case GLFW_KEY_R:      g_window.keys[K_RESTART]   = action == GLFW_PRESS; break;
    case GLFW_KEY_F1:     g_window.keys[K_COLLIDERS] = action == GLFW_PRESS; break;
  }
}

bool
window_make(uint32_t width, uint32_t height) {
  log_infol("making window...");
  if (!glfwInit()) {
    log_errorl("couldn't initialize glfw");
    return false;
  }
  log_infol("glfw initialized");
  glfwWindowHint(GLFW_RESIZABLE, false);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  g_window.handle = glfwCreateWindow(width, height, GAME_TITLE, 0, 0);
  if (!g_window.handle) {
    log_errorl("couldn't make window handle");
    return false;
  }
  glfwMakeContextCurrent(g_window.handle);
  glfwSwapInterval(1);
  log_infolf("window created with %d %d dimensions", width, height);
  const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  if (vidmode) {
    glfwSetWindowPos(g_window.handle, vidmode->width / 2 - width / 2, vidmode->height / 2 - height / 2);
    log_infol("window centered");
  } else {
    log_warnl("couldn't center window");
  }
  (void)memset(g_window.keys, false, sizeof (bool) * KEY_AMOUNT);
  (void)glfwSetKeyCallback(g_window.handle, key_callback);
  log_infol("window input setup");
  log_infol("window creation complete!");
  return true;
}

void
window_destroy(void) {
  glfwTerminate();
}

void
window_close(void) {
  (void)glfwSetWindowShouldClose(g_window.handle, true);
}

bool
window_is_running(void) {
  memcpy(g_window.pkeys, g_window.keys, sizeof (bool) * KEY_AMOUNT);
  glfwPollEvents();
  glfwSwapBuffers(g_window.handle);
  return !glfwWindowShouldClose(g_window.handle);
}

bool
window_is_key_press(enum key key) {
#if DEV
  if (key >= KEY_AMOUNT) {
    log_errorlf("%s: passing invalid key", __func__);
    return false;
  }
#endif
  return !g_window.pkeys[key] && g_window.keys[key];
}

bool
window_is_key_down(enum key key) {
#if DEV
  if (key >= KEY_AMOUNT) {
    log_errorlf("%s: passing invalid key", __func__);
    return false;
  }
#endif
  return g_window.keys[key];
}

bool
window_is_key_up(enum key key) {
#if DEV
  if (key >= KEY_AMOUNT) {
    log_errorlf("%s: passing invalid key", __func__);
    return false;
  }
#endif
  return !g_window.keys[key];
}

bool
window_is_key_release(enum key key) {
#if DEV
  if (key >= KEY_AMOUNT) {
    log_errorlf("%s: passing invalid key", __func__);
    return false;
  }
#endif
  return g_window.pkeys[key] && !g_window.keys[key];
}

float
window_get_delta_time(void) {
  float prv_time = g_window.time;
  g_window.time = glfwGetTime();
  return g_window.time - prv_time;
}
