#include <cstdio>
#include <cstdlib>
#include <format>

#include "engine/Light.hpp"
#include "engine/Shader.hpp"
#include "engine/gui.hpp"
#include "engine/mesh/meshes.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "inputs.hpp"

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
  glfwSwapInterval(1);

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

  Shader planetShader("planet.vert", "planet.frag");
  Shader linesShader("lines.vert", "lines.frag", "lines.geom");
  Shader colorShader("default/color.vert", "default/color.frag");
  Shader normalsShader("default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader textureShader("default/texture.vert", "default/texture.frag");

  Camera camera({-1.f, 1.f, 2.f}, {0.5f, -0.3f, -1.f}, 100.f);
  Light light({3.5f, 1.5f, 1.2f});
  light.scale(0.1f);

  Planet planet(720, 1.f, "res/geo/textures/wem21600.png");
  gui::link(&planet);

  Mesh plane = meshes::plane(vec3(0.f), vec2(20.f, 10.f));

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;

  glEnable(GL_DEPTH_TEST);

  planetShader.setUniform3f("lightPos", light.getPosition());
  planetShader.setUniform4f("lightColor", light.getColor());

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
    } else {
      glfwSetCursorPos(window, _gcfg.winWidth * 0.5f, _gcfg.winHeight * 0.5f);
    }

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planet.draw(camera, planetShader);
    if (_gcfg.drawWireframe) planet.draw(camera, linesShader);
    if (_gcfg.drawNormals) planet.draw(camera, normalsShader);
    /* plane.draw(camera, textureShader); */
    /* if (_gcfg.drawWireframe) plane.draw(camera, linesShader); */
    /* if (_gcfg.drawNormals) plane.draw(camera, normalsShader); */

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
