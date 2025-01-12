#include "engine/Light.hpp"
#include "engine/Mesh.hpp"
#include "engine/Shader.hpp"
#include "engine/meshes.hpp"
#include "engine/planet/Planet.hpp"
#include "inputs.hpp"

#include <cstdio>
#include <cstdlib>
#include <windows.h>

int main() {
  // Change cwd to where "src" directory located (since launching the executable always from the directory where its
  // located)
  SetCurrentDirectory("../../../src");

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  GLFWwindow* window = glfwCreateWindow(_gState.winWidth, _gState.winHeight, "Sphere", NULL, NULL);
  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, _gState.winWidth * 0.5f, _gState.winHeight * 0.5f);

  // GLAD init
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, _gState.winWidth, _gState.winHeight);

  Shader mainShader("main.vert", "main.frag");
  Shader linesShader("lines.vert", "lines.frag", "lines.geom");
  Shader lightShader("light.vert", "light.frag");

  Camera camera({-1.f, 1.f, 2.f}, {0.5f, -0.3f, -1.f}, 100.f);
  Light light({3.5f, 1.5f, 1.2f});
  Planet planet = Planet(100);
  Mesh sphere = meshes::sphere(2.f, 100, {1.f, 0.f, 1.f});

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;
  double mouseX, mouseY;

  glEnable(GL_DEPTH_TEST);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, _gState.winWidth * 0.5f, _gState.winHeight * 0.5f);

    currTime = glfwGetTime();
    dt = currTime - prevTime;
    prevTime = currTime;
    _gState.time = currTime;

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput(window, &camera);

    camera.move(mouseX, mouseY);
    camera.update(dt);

    mainShader.setUniform3f("lightPos", camera.getPosition());
    mainShader.setUniform4f("lightColor", light.getColor());

    planet.draw(camera, mainShader);
    light.draw(camera, lightShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
