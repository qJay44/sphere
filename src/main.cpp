#include <cstdio>
#include <cstdlib>
#include <format>
#include <windows.h>

#include "engine/Camera.hpp"
#include "GLFW/glfw3.h"
#include "engine/CameraStorage.hpp"
#include "engine/Shader.hpp"
#include "engine/InputsHandler.hpp"
#include "global.hpp"
#include "gui.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "objects/Airplane.hpp"
#include "objects/AirplaneCamera.hpp"
#include "objects/Light.hpp"

using global::window;

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
  window = glfwCreateWindow(1200, 720, "Sphere", NULL, NULL);
  ivec2 winSize = getWinSize();
  dvec2 winCenter = winSize / 2;

  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, winCenter.x, winSize.y * 0.5f);

  // GLAD init
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, winSize.x, winSize.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  Light light({3.5f, 1.5f, 1.2f}, 0.1f);

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("shaders");
  Shader planetShader("planet.vert", "planet.frag");
  Shader linesShader("lines.vert", "lines.frag", "lines.geom");
  Shader colorShader("default/color.vert", "default/color.frag");
  Shader normalsShader("default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader textureShader("default/texture.vert", "default/texture.frag");
  Shader airplaneShader("airplane.vert", "airplane.frag");

  planetShader.setUniform3f("lightPos", light.getPosition());
  planetShader.setUniform4f("lightColor", light.getColor());
  airplaneShader.setUniform3f("lightPos", light.getPosition());
  airplaneShader.setUniform4f("lightColor", light.getColor());

  // ===== Planet =============================================== //

  /* Planet planet(720, global::planetRadius, "res/geo/textures/wem21600.png"); */
  Planet planet(128u, 256u, 20.f, "res/geo/textures/wem2560.png");

  // ===== Airplane ============================================= //

  vec3 airplanePosInit(0.f);
  float airplaneFlyHeight = 3.f;
  airplanePosInit.z = planet.getRadius() + airplaneFlyHeight;
  Airplane airplane(planet, airplanePosInit, PI / 200.f, airplaneFlyHeight, 0.1f);
  Texture airplaneTexture("res/tex/Aircraft_Texture.png", GL_TEXTURE_2D, "diffuse0");
  airplane.add(&airplaneTexture);

  // ===== Cameras ============================================== //

  Camera cameraFree({0.f, 0.f, planet.getRadius()}, {0.f, 0.f, -1.f}, 100.f);
  AirplaneCamera cameraAirplane(airplane, 8.f, 200.f);
  CameraStorage::cameraFreePtr = &cameraFree;
  CameraStorage::cameraAirplanePtr= &cameraAirplane;

  // ===== Inputs Handler ======================================= //

  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);

  // ============================================================ //

  gui::link(&planet);
  gui::link(&cameraAirplane);
  gui::link(&cameraFree);
  gui::link(&airplane);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static Camera* camera = &cameraAirplane;
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer;
    static double currTime = prevTime;

    constexpr double fpsLimit = 1. / 90.;
    currTime = glfwGetTime();
    global::dt = currTime - prevTime;

    // FPS cap
    if (global::dt < fpsLimit) continue;
    else prevTime = currTime;

    camera = global::camIsArcball ? &cameraAirplane : &cameraFree;

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
      InputsHandler::process(camera);
    else
      glfwSetCursorPos(window, winCenter.x, winCenter.y);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      u16 fps = static_cast<u16>(1.f / global::dt);
      glfwSetWindowTitle(window, std::format("FPS: {} / {:.5f} ms", fps, global::dt).c_str());
      titleTimer = currTime;
    }

    airplane.update();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planet.draw(camera, planetShader);
    if (global::drawWireframe) planet.draw(camera, linesShader);
    if (global::drawNormals)   planet.draw(camera, normalsShader);

    airplane.draw(camera, airplaneShader);
    if (global::drawWireframe)  airplane.draw(camera, linesShader);
    if (global::drawNormals)    airplane.draw(camera, normalsShader);
    if (global::drawDirections) airplane.draw(camera, colorShader, AIRPLANE_FLAG_DRAW_DIRECTIONS);

    glDisable(GL_CULL_FACE);

    light.draw(camera, colorShader);
    camera->draw(cameraAirplane, colorShader, CAMERA_FLAG_DRAW_FRUSTUM);

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

