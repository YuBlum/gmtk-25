#include <stdio.h>
#include <GLFW/glfw3.h>

int
main(void) {
  glfwInit();
  glfwWindowHint(GLFW_RESIZABLE, false);
  GLFWwindow *window = glfwCreateWindow(640, 480, "game", 0, 0);
  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}
