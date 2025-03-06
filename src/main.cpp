#include <cstdio>
#include <cstdlib>
#include <format>

#include "engine/Shader.hpp"
#include "engine/inputs.hpp"
#include "gui.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "objects/Light.hpp"

void GLAPIENTRY MessageCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam
) {
  clrp::clrp_t clrpError;
  clrpError.attr = clrp::ATTRIBUTE::BOLD;
  clrpError.fg = clrp::FG::RED;
  fprintf(
    stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, clrp::format(message, clrpError).c_str()
  );
}

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
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  Shader::setDirectoryLocation("shaders");
  Shader planetShader("planet.vert", "planet.frag");
  Shader linesShader("lines.vert", "lines.frag", "lines.geom");
  Shader colorShader("default/color.vert", "default/color.frag");
  Shader normalsShader("default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader textureShader("default/texture.vert", "default/texture.frag");

  Camera camera({0.f, 0.f, 2.f}, {0.f, 0.f, -1.f}, 100.f);
  Light light({3.5f, 1.5f, 1.2f});
  light.scale(0.1f);
  planetShader.setUniform4f("lightColor", light.getColor());
  planetShader.setUniform3f("lightPos", light.getPosition());

  /* Planet planet(720, 1.f, "res/geo/textures/wem21600.png"); */
  Planet planet(720, 1.f, "res/geo/textures/wem2560.png");
  gui::link(&planet);
  gui::link(&camera);

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    constexpr double fpsLimit = 1. / 90.;
    currTime = glfwGetTime();
    _gcfg.dt = currTime - prevTime;

    // FPS lock
    if (_gcfg.dt < fpsLimit) continue;
    else prevTime = currTime;

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      processInput(window, camera);
    } else {
      glfwSetCursorPos(window, _gcfg.winWidth * 0.5f, _gcfg.winHeight * 0.5f);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      u16 fps = static_cast<u16>(1. / _gcfg.dt);
      glfwSetWindowTitle(window, std::format("FPS: {} / {:.5f} ms", fps, _gcfg.dt).c_str());
      titleTimer = currTime;
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planet.draw(camera, planetShader);
    if (_gcfg.drawWireframe) planet.draw(camera, linesShader);
    if (_gcfg.drawNormals) planet.draw(camera, normalsShader);

    glDisable(GL_CULL_FACE);
    light.draw(camera, colorShader);
    glEnable(GL_CULL_FACE);

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
