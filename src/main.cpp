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

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("shaders");
  Shader planetShader("planet.vert", "planet.frag");
  Shader linesShader("lines.vert", "lines.frag", "lines.geom");
  Shader colorShader("default/color.vert", "default/color.frag");
  Shader normalsShader("default/normal.vert", "default/normal.frag", "default/normal.geom");
  Shader textureShader("default/texture.vert", "default/texture.frag");
  Shader airplaneShader("airplane.vert", "airplane.frag");

  GLint planetShaderLightPosLoc = planetShader.getUniformLoc("lightPos");
  GLint planetShaderLightColorLoc = planetShader.getUniformLoc("lightColor");
  GLint airplaneShaderLightPosLoc = airplaneShader.getUniformLoc("lightPos");
  GLint airplaneShaderLightColorLoc = airplaneShader.getUniformLoc("lightColor");

  // ===== Textures ============================================= //

  Texture normalheightmap0("res/tex/planet/normalheightmap0.png", GL_TEXTURE_2D, "normalheightmap0", 0, 4);
  Texture normalheightmap1("res/tex/planet/normalheightmap1.png", GL_TEXTURE_2D, "normalheightmap1", 1, 4);

  Texture world0;
  Texture world1;
  {
    image2D img("res/tex/planet/world21600.jpg");
    image2D img_w2 = img;
    img_w2.width /= 2;

    glPixelStorei(GL_UNPACK_ROW_LENGTH, img.width);
    world0 = Texture(img_w2, GL_TEXTURE_2D, "world0", 2, 3);
    img_w2.pixels += img_w2.width * img_w2.channels;
    world1 = Texture(img_w2, GL_TEXTURE_2D, "world1", 3, 3);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    img_w2.pixels = nullptr;
  }


  // ===== Planet =============================================== //

  Planet::addTexNormalheightmaps(&normalheightmap0, &normalheightmap1);
  Planet::addTexWorld(&world0, &world1);
  Planet planet(1024u, 256u, 45.f);

  // ===== Light ================================================ //

  Light light(planet.getRadius() + vec3{16.3f, 24.f, 26.6f}, 0.5f);

  // ===== Airplane ============================================= //

  vec3 airplanePosInit(0.f);
  float airplaneFlyHeight = 10.f;
  airplanePosInit.z = planet.getRadius() + airplaneFlyHeight;
  Airplane airplane(planet, airplanePosInit, PI / 100.f, airplaneFlyHeight, PI / 10.f, 0.1f);
  Texture airplaneTexture("res/tex/Aircraft_Texture.png", GL_TEXTURE_2D, "diffuse0");
  airplane.add(&airplaneTexture);

  // ===== Cameras ============================================== //

  Camera cameraFree({0.f, 0.f, planet.getRadius() + 3.f}, {0.f, 0.f, -1.f}, 100.f);
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

    planetShader.setUniform3f(planetShaderLightPosLoc, light.getPosition());
    planetShader.setUniform3f(planetShaderLightColorLoc, light.getColor());
    airplaneShader.setUniform3f(airplaneShaderLightPosLoc, light.getPosition());
    airplaneShader.setUniform3f(airplaneShaderLightColorLoc, light.getColor());

    airplane.update();

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    planet.draw(camera, planetShader);
    airplane.draw(camera, airplaneShader);

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

