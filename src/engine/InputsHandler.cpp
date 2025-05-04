#include "InputsHandler.hpp"

#include "../gui.hpp"
#include <cassert>

using global::window;

bool guiWasFocused;

AirplaneCamera* InputsHandler::airplaneCameraPtr = nullptr;

void InputsHandler::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (key) {
    case GLFW_KEY_G:
      if (action == GLFW_PRESS) {
        global::guiFocused = !global::guiFocused;
        guiWasFocused = true;
      }
      break;
    case GLFW_KEY_I:
      if (action == GLFW_PRESS) gui::toggle();
      break;
    case GLFW_KEY_1:
      if (action == GLFW_PRESS) global::drawWireframe = !global::drawWireframe;
      break;
    case GLFW_KEY_2:
      if (action == GLFW_PRESS) global::drawNormals = !global::drawNormals;
      break;
    case GLFW_KEY_3:
      if (action == GLFW_PRESS) global::drawDirections = !global::drawDirections;
      break;
    case GLFW_KEY_F:
      if (action == GLFW_PRESS) global::camIsArcball = !global::camIsArcball;
      break;
  }
}

void InputsHandler::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  assert(airplaneCameraPtr != nullptr);
  airplaneCameraPtr->zoom(static_cast<float>(yoffset));
}

void InputsHandler::process(Camera* camera) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

  glm::ivec2 winSize;
  glfwGetWindowSize(window, &winSize.x, &winSize.y);
  glm::dvec2 winCenter = winSize / 2;

  if (!global::guiFocused) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (guiWasFocused) {
      glfwSetCursorPos(window, winCenter.x, winCenter.y);
      guiWasFocused = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera->moveForward();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera->moveLeft();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera->moveBack();
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera->moveRight();

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera->moveUp();
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera->moveDown();

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera->setIncreasedSpeed();
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) camera->setNormalSpeed();

    camera->update();

    glfwSetCursorPos(window, winCenter.x, winCenter.y);
  } else {
    camera->update(true);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

}
