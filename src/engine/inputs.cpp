#include "inputs.hpp"

#include "../gui.hpp"

bool guiWasFocused;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (key) {
    case GLFW_KEY_G:
      if (action == GLFW_PRESS) {
        _gcfg.guiFocused = !_gcfg.guiFocused;
        guiWasFocused = true;
      }
      break;
    case GLFW_KEY_I:
      if (action == GLFW_PRESS) gui::toggle();
      break;
    case GLFW_KEY_1:
      if (action == GLFW_PRESS) _gcfg.drawWireframe = !_gcfg.drawWireframe;
      break;
    case GLFW_KEY_2:
      if (action == GLFW_PRESS) _gcfg.drawNormals = !_gcfg.drawNormals;
      break;
  }
}

void processInput(GLFWwindow* window, Camera& camera) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (!_gcfg.guiFocused) {
    static double mouseX, mouseY;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (guiWasFocused) {
      glfwSetCursorPos(window, _gcfg.winWidth * 0.5f, _gcfg.winHeight * 0.5f);
      guiWasFocused = false;
    }

    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, _gcfg.winWidth * 0.5f, _gcfg.winHeight * 0.5f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.moveForward();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.moveLeft();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.moveBack();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.moveRight();

    /* if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.moveUp(); */
    /* if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.moveDown(); */

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.setIncreasedSpeed();
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) camera.setNormalSpeed();

    /* camera.move(mouseX, mouseY); */
    camera.update();
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}
