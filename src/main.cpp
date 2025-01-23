#include <cstdio>
#include <cstdlib>
#include <format>

#include "engine/Light.hpp"
#include "engine/Shader.hpp"
#include "engine/planet/Planet.hpp"
#include "engine/texture/Texture.hpp"
#include "glad/glad.h"
#include "inputs.hpp"
#include "nc/GEBCO.hpp"

int main() {
  // Assuming the executable is launching from its own directory
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
  Shader colorShader("default/color.vert", "default/color.frag");
  Shader normalShader("default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader textureShader("default/texture.vert", "default/texture.frag");

  Texture earthTexture(R"(res\geo\textures\wem2560.png)", TEXTURE_DIFFUSE);
  GEBCO gebco(R"(res\geo\data\GEBCO_2024.nc)");

  Camera camera({-1.f, 1.f, 2.f}, {0.5f, -0.3f, -1.f}, 100.f);
  Light light({3.5f, 1.5f, 1.2f});

  Planet planet(80, &gebco);
  planet.add(earthTexture);

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

    // Update window title every 0.3 seconds
    if (glfwGetTime() - titleTimer >= 0.3) {
      u16 fps = 1. / dt;
      glfwSetWindowTitle(window, std::format("FPS: {} / {:.5f} ms", fps, dt).c_str());
      titleTimer = currTime;
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      processInput(window, &camera);
      camera.move(mouseX, mouseY);
      camera.update(dt);
    }

    mainShader.setUniform3f("lightPos", light.getPosition());
    mainShader.setUniform4f("lightColor", light.getColor());

    planet.draw(camera, mainShader);
    /* planet.draw(camera, normalShader); */
    /* planet.draw(camera, linesShader); */
    light.draw(camera, colorShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
