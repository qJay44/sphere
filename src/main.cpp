#include <cstdio>
#include <cstdlib>
#include <format>
#include <windows.h>

#include "engine/Camera.hpp"
#include "GLFW/glfw3.h"
#include "engine/CameraStorage.hpp"
#include "engine/Shader.hpp"
#include "engine/InputsHandler.hpp"
#include "engine/mesh/meshes.hpp"
#include "global.hpp"
#include "gui.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "objects/Airplane.hpp"
#include "objects/AirplaneCamera.hpp"
#include "objects/Earth.hpp"
#include "objects/Light.hpp"
#include "utils/clrp.hpp"

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
  exit(1);
}

int main() {
  // Assuming the executable is launching from its own directory
  SetCurrentDirectory("../../../src");

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, TRUE);

  // Window init
  window = glfwCreateWindow(1200, 720, "Sphere", NULL, NULL);
  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
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

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("shaders");
  Shader::setDefaultShader(SHADER_DEFAULT_TYPE_COLOR_SHADER, "default/color.vert", "default/color.frag");
  Shader::setDefaultShader(SHADER_DEFAULT_TYPE_NORMALS_SHADER, "default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader::setDefaultShader(SHADER_DEFAULT_TYPE_TEXTURE_SHADER, "default/texture.vert", "default/texture.frag");

  const Shader& colorShader = Shader::getDefaultShader(SHADER_DEFAULT_TYPE_COLOR_SHADER);
  Shader earthShader("earth.vert", "earth.frag", "earth.geom");
  Shader airplaneShader("airplane.vert", "airplane.frag");
  Shader planetBordersShader("borders.vert", "borders.frag");

  const GLint earthShaderLightPosLoc      = earthShader.getUniformLoc("u_lightPos");
  const GLint earthShaderLightColorLoc    = earthShader.getUniformLoc("u_lightColor");
  const GLint earthShaderTimeLoc          = earthShader.getUniformLoc("u_time");
  const GLint airplaneShaderLightPosLoc   = airplaneShader.getUniformLoc("u_lightPos");
  const GLint airplaneShaderLightColorLoc = airplaneShader.getUniformLoc("u_lightColor");

  // ===== Earth =============================================== //

  Earth::loadTextures();
  Earth earth(512u, 256u, 80.f);

  // ===== Light ================================================ //

  Light light(earth.getRadius() + vec3{16.3f, 24.f, 26.6f}, 1.5f);

  // ===== Airplane ============================================= //

  Airplane::loadTextures();
  vec3 airplanePosInit(0.f);
  float airplaneFlyHeight = 10.f;
  airplanePosInit.z = earth.getRadius() + airplaneFlyHeight;
  Airplane airplane(earth, airplanePosInit, PI / 100.f, airplaneFlyHeight, PI / 10.f, 0.001f);

  // ===== Cameras ============================================== //

  Camera cameraFree({0.f, 0.f, earth.getRadius() + 3.f}, {0.f, 0.f, -1.f}, 100.f);
  cameraFree.setFarPlane(300.f);
  AirplaneCamera cameraAirplane(airplane, 8.f, 200.f);
  CameraStorage::cameraFreePtr = &cameraFree;
  CameraStorage::cameraAirplanePtr= &cameraAirplane;
  cameraFree.setSpeed(earth.getRadius() * 0.1f);

  // ===== Inputs Handler ======================================= //

  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);

  // ============================================================ //

  gui::link(&earth);
  gui::link(&cameraAirplane);
  gui::link(&cameraFree);
  gui::link(&airplane);
  gui::link(&light);

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
    global::time += global::dt;

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

    earthShader.setUniform3f(earthShaderLightPosLoc, light.getPosition());
    earthShader.setUniform3f(earthShaderLightColorLoc, light.getColor());
    earthShader.setUniform1f(earthShaderTimeLoc, global::time);
    airplaneShader.setUniform3f(airplaneShaderLightPosLoc, light.getPosition());
    airplaneShader.setUniform3f(airplaneShaderLightColorLoc, light.getColor());

    airplane.update();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    earth.draw(camera, earthShader);

    airplane.draw(camera, airplaneShader);

    glDisable(GL_CULL_FACE);

    light.draw(camera, colorShader);
    camera->draw(cameraAirplane, colorShader, CAMERA_FLAG_DRAW_FRUSTUM);

    if (global::drawGlobalAxis)
      meshes::axis(earth.getRadius() * 2.f).draw(camera, colorShader);

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

