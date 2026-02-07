#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
  #include <direct.h>
  #define CHDIR(p) _chdir(p);
#else
  #include <unistd.h>
  #define CHDIR(p) chdir(p);
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include "global.hpp"
#include "engine/gui/gui.hpp"
#include "engine/Camera.hpp"
#include "engine/Shader.hpp"
#include "engine/InputsHandler.hpp"
#include "engine/FBO.hpp"
#include "engine/mesh/meshes.hpp"
#include "engine/texture/Texture.hpp"
#include "engine/objects/Airplane.hpp"
#include "engine/objects/Earth.hpp"
#include "engine/objects/Light.hpp"
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
  switch (source) {
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return; // Handled by the Shader class itself
    case GL_DEBUG_SOURCE_API:
      return; // "SIMD32 shader inefficient", skipping since occurs only on my laptop
  }

  clrp::clrp_t clrpError;
  clrpError.attr = clrp::ATTRIBUTE::BOLD;
  clrpError.fg = clrp::FG::RED;
  fprintf(
    stderr, "GL CALLBACK: %s source = 0x%x, id = 0x%x type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), source, id, type, severity, clrp::format(message, clrpError).c_str()
  );
  exit(1);
}

int main() {
  // Assuming the executable is launching from its own directory
  CHDIR("../../..");

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  // Window init
  window = glfwCreateWindow(1200, 720, "MyProgram", NULL, NULL);
  ivec2 winSize = global::getWinSize();
  dvec2 winCenter = dvec2(winSize) / 2.;

  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + 2 * !global::guiFocused);
  glfwSetCursorPos(window, winCenter.x, winCenter.y);

  // GLAD init
  if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, winSize.x, winSize.y);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("src/engine/shaders");

  Shader earthShader("earth.vert", "earth.frag", "earth.geom");
  Shader airplaneShader("airplane.vert", "airplane.frag");
  Shader trailShader("trail.vert", "trail.frag");
  Shader planetBordersShader("borders.vert", "borders.frag");
  Shader atmosphereShader("atmosphere.vert", "atmosphere.frag");
  Shader lightShader("light.vert", "light.frag");
  Shader linesShader("lines.vert", "lines.frag");

  const float earthInitRadius = 300.f;

  // ===== Light ================================================ //

  Light light(earthInitRadius + vec3{16.3f, 24.f, 26.6f}, 50.f);

  // ===== Planet ============================================== //

  Earth earth(512u, 256u, earthInitRadius);
  earth.loadTextures(earthShader);

  // ===== Airplane ============================================= //

  vec3 airplanePosInit(0.f);
  float airplaneFlyHeight = 10.f;
  airplanePosInit.z = earthInitRadius + airplaneFlyHeight;
  Airplane airplane(airplanePosInit, airplaneFlyHeight, "res/obj/11804_Airplane_v2_l2.obj", 0.001f);
  airplane.setSpeedDefault(PI / 100.f);
  airplane.setTurnSpeed(PI / 10.f);

  // ===== Cameras ============================================== //

  Camera cameraSpectate({earth.getRadius(), 0.f, earth.getRadius()}, PI);
  cameraSpectate.setFarPlane(1000.f);
  cameraSpectate.setSpeedDefault(earth.getRadius() * 0.1f);

  Camera& cameraAirplane = airplane.getCamera();
  cameraAirplane.setFarPlane(100.f);
  cameraAirplane.setSensitivity(50.f);

  Camera* cameraATM = global::controllingAirplane ? &cameraAirplane : &cameraSpectate;

  // ===== Inputs Handler ======================================= //

  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);
  glfwSetCursorPosCallback(window, InputsHandler::cursorPosCallback);

  InputsHandler::airplanePtr = &airplane;

  // ============================================================ //

  gui::earthPtr = &earth;
  gui::camSpecatePtr = &cameraSpectate;
  gui::airplanePtr = &airplane;
  gui::lightPtr = &light;

  TextureDescriptor fboTexDesc{};
  fboTexDesc.uniformName = "u_screenColorTex";
  fboTexDesc.unit = 0;
  fboTexDesc.minFilter = GL_NEAREST;
  fboTexDesc.magFilter = GL_NEAREST;
  fboTexDesc.wrapS = GL_CLAMP_TO_EDGE;
  fboTexDesc.wrapT = GL_CLAMP_TO_EDGE;
  fboTexDesc.genMipMap = false;

  FBO fboScreen;
  Texture screenColorTexture(winSize, fboTexDesc);

  fboTexDesc.uniformName = "u_screenDepthTex";
  fboTexDesc.unit = 1;
  fboTexDesc.internalFormat = GL_DEPTH_COMPONENT;
  fboTexDesc.format = GL_DEPTH_COMPONENT;

  Texture screenDepthTexture(winSize, fboTexDesc);

  fboScreen.attach2D(GL_COLOR_ATTACHMENT0, screenColorTexture);
  fboScreen.attach2D(GL_DEPTH_ATTACHMENT, screenDepthTexture);

  Mesh axis = meshes::axis(false);
  axis.scale(1e4f);

  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer;
    static double currTime = prevTime;

    constexpr double fpsLimit = 1. / 90.;
    currTime = glfwGetTime();
    global::dt = currTime - prevTime;

    // FPS cap
    if (global::dt < fpsLimit) continue;
    else prevTime = currTime;

    cameraATM = global::controllingAirplane ? &cameraAirplane : &cameraSpectate;
    global::time += global::dt;

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
      InputsHandler::process(global::controllingAirplane ? (Moveable&)airplane : cameraSpectate);
      cameraATM->update();
    } else
      glfwSetCursorPos(window, winCenter.x, winCenter.y);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      gui::fps = static_cast<u16>(1.f / global::dt);
      titleTimer = currTime;
    }

    light.update();
    earth.update(light);
    airplane.update(earth);

    earthShader.setUniform3f("u_lightPos", light.getPosition());
    earthShader.setUniform3f("u_lightColor", light.getColor());
    earthShader.setUniform1f("u_time", global::time);
    airplaneShader.setUniform3f("u_lightPos", light.getPosition());
    airplaneShader.setUniform3f("u_lightColor", light.getColor());

    fboScreen.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    earth.draw(cameraATM, frustum::Frustum(cameraAirplane), earthShader);

    airplane.draw(cameraATM, airplaneShader);

    FBO::unbind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    screenColorTexture.bind();
    screenDepthTexture.bind();

    atmosphereShader.setUniform3f("u_lightPos", light.getPosition());
    earth.drawAtmosphere(cameraATM, atmosphereShader);

    screenColorTexture.unbind();
    screenDepthTexture.unbind();

    airplane.drawTrail(cameraATM, trailShader);
    airplane.drawDirections(cameraATM, linesShader);

    light.draw(cameraATM, lightShader);

    cameraATM->draw(cameraAirplane, linesShader, CAMERA_FLAG_DRAW_FRUSTUM);

    if (global::drawGlobalAxis)
      axis.draw(cameraATM, linesShader);

    gui::draw();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}

