#include "inputs.hpp"

void processInput(GLFWwindow* window, Camera* camera) {
  static bool escPressed;

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (!_gState.guiFocused) {
    static double mouseX, mouseY;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, _gState.winWidth * 0.5f, _gState.winHeight * 0.5f);

    camera->move(mouseX, mouseY);
    camera->update(_gState.dt);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->moveForward();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->moveLeft();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->moveBack();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->moveRight();

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera->moveUp();
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->moveDown();

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera->setIncreasedSpeed();
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) camera->setNormalSpeed();
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  bool escPressedCurrently = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
  if (!escPressed && escPressedCurrently) {
    _gState.guiFocused = !_gState.guiFocused;
    if (!_gState.guiFocused) glfwSetCursorPos(window, _gState.winWidth * 0.5f, _gState.winHeight * 0.5f);
  }
  escPressed = escPressedCurrently;
}
