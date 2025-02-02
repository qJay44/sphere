#include <cstdio>
#include <cstdlib>
#include <format>

#include "engine/Light.hpp"
#include "engine/Shader.hpp"
#include "engine/gui.hpp"
#include "engine/mesh/meshes.hpp"
#include "engine/mesh/texture/Texture.hpp"
#include "engine/planet/Planet.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "inputs.hpp"
#include "engine/mesh/texture/Tif.hpp"

int main() {
  // Assuming the executable is launching from its own directory
  SetCurrentDirectory("../../../src");

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  GLFWwindow* window = glfwCreateWindow(_gcfg.winWidth, _gcfg.winHeight, "Sphere", NULL, NULL);
  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, _gcfg.winWidth * 0.5f, _gcfg.winHeight * 0.5f);
  glfwSetKeyCallback(window, keyCallback);

  // GLAD init
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, _gcfg.winWidth, _gcfg.winHeight);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  Shader mainShader("main.vert", "main.frag");
  Shader linesShader("lines.vert", "lines.frag", "lines.geom");
  Shader colorShader("default/color.vert", "default/color.frag");
  Shader normalsShader("default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader textureShader("default/texture.vert", "default/texture.frag");

  Texture earthTexture(R"(res\geo\textures\wem2560.png)", TEXTURE_DIFFUSE);

  Camera camera({-1.f, 1.f, 2.f}, {0.5f, -0.3f, -1.f}, 100.f);
  Light light({3.5f, 1.5f, 1.2f});

  Tif tif(R"(C:\Users\q44\Downloads\GRAY_HR_SR_OB_DR\GRAY_HR_SR_OB_DR.tif)");
  tif.printInfo();

  Planet planet(20, 1.f);
  planet.add(earthTexture);
  gui::link(&planet);
  /* Mesh plane = meshes::plane({}, {10.f, 20.f}); */

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;

  glEnable(GL_DEPTH_TEST);

  mainShader.setUniform3f("lightPos", light.getPosition());
  mainShader.setUniform4f("lightColor", light.getColor());

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    currTime = glfwGetTime();
    dt = currTime - prevTime;
    prevTime = currTime;
    _gcfg.dt = dt;

    // Update window title every 0.3 seconds
    if (glfwGetTime() - titleTimer >= 0.3) {
      u16 fps = static_cast<u16>(1. / dt);
      glfwSetWindowTitle(window, std::format("FPS: {} / {:.5f} ms", fps, dt).c_str());
      titleTimer = currTime;
    }

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      processInput(window, camera);
    } else{
      glfwSetCursorPos(window, _gcfg.winWidth * 0.5f, _gcfg.winHeight * 0.5f);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planet.draw(camera, mainShader);
    if (_gcfg.drawWireframe) planet.draw(camera, linesShader);
    if (_gcfg.drawNormals) planet.draw(camera, normalsShader);

    light.draw(camera, colorShader);

    gui::draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
