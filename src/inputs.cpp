#include "inputs.hpp"

void processInput(GLFWwindow* window, Camera* camera) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->moveForward();
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->moveLeft();
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->moveBack();
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->moveRight();

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera->moveUp();
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->moveDown();

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera->setIncreasedSpeed();
  else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) camera->setNormalSpeed();
}
