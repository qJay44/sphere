#include "engine/Camera.hpp"
#include "engine/FBO.hpp"
#include "engine/InputsHandler.hpp"
#include "engine/Shader.hpp"
#include "engine/gui/gui.hpp"
#include "engine/objects/Airplane.hpp"
#include "engine/objects/Earth.hpp"
#include "engine/texture/Texture2D.hpp"
#include "global.hpp"
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
  window = glfwCreateWindow(1600, 900, "MyProgram", NULL, NULL);
  global::profiler = new ProfilerManager(300);

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

  gui::init();

  if (VIPS_INIT("TileApp"))
    vips_error_exit("Unable to init libvips");

  // ===== Shaders ============================================== //

  Shader::setDirectoryLocation("src/engine/shaders");

  Shader earthShader("earth.vert", "earth.frag", "earth.tesc", "earth.tese");
  Shader airplaneShader("airplane.vert", "airplane.frag");
  Shader trailShader("trail.vert", "trail.frag");
  Shader atmosphereShader("atmosphere.vert", "atmosphere.frag");
  Shader sunShader("sun.vert", "sun.frag");
  Shader lightShader("light.vert", "light.frag");
  Shader directionShader("direction.vert", "direction.frag");
  Shader linesShader("lines.vert", "lines.frag");

  const float earthInitRadius = 300.f;

  // ===== Light ================================================ //

  Sun sun(800.f, 2.f, 0.f);
  sun.updateDir();

  // ===== Planet ============================================== //

  Earth earth(50, 9, earthInitRadius);

  // ===== Airplane ============================================= //

  vec3 airplanePosInit(0.f);
  float airplaneFlyHeight = 10.f;
  airplanePosInit.z = earthInitRadius + airplaneFlyHeight;
  Airplane airplane(airplanePosInit, airplaneFlyHeight, "res/obj/11804_Airplane_v2_l2.obj", 0.001f);
  airplane.setSpeedDefault(5.f);
  airplane.setTurnSpeed(PI / 5.f);
  // airplane.setFlags(AirplaneFlags::AirplaneFlags_DrawDirections);

  // ===== Cameras ============================================== //

  Camera cameraSpectate({earth.getRadius(), 0.f, earth.getRadius()}, PI);
  cameraSpectate.setFarPlane(1000.f);
  cameraSpectate.setSpeedDefault(earth.getRadius());

  Camera& cameraAirplane = airplane.getCamera();
  cameraAirplane.setFarPlane(100.f);
  cameraAirplane.setSensitivity(1.f);

  Camera* cameraATM = global::controllingAirplane ? &cameraAirplane : &cameraSpectate;

  // ===== Inputs Handler ======================================= //

  glfwSetScrollCallback(window, InputsHandler::scrollCallback);
  glfwSetKeyCallback(window, InputsHandler::keyCallback);
  glfwSetCursorPosCallback(window, InputsHandler::cursorPosCallback);

  InputsHandler::airplanePtr = &airplane;

  // ===== Framebuffers ========================================= //

  TextureDescriptor fboTexDesc{};
  fboTexDesc.internalFormat = GL_RGBA16F;
  fboTexDesc.minFilter = GL_NEAREST;
  fboTexDesc.magFilter = GL_NEAREST;

  FBO fboScreen;
  Texture2D screenColorTexture(winSize, fboTexDesc);

  fboTexDesc.internalFormat = GL_DEPTH_COMPONENT;
  fboTexDesc.format = GL_DEPTH_COMPONENT;

  Texture2D screenDepthTexture(winSize, fboTexDesc);

  fboScreen.attach2D(GL_COLOR_ATTACHMENT0, screenColorTexture);
  fboScreen.attach2D(GL_DEPTH_ATTACHMENT, screenDepthTexture);

  // ============================================================ //

  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glPatchParameteri(GL_PATCH_VERTICES, 4);

  gui::earthPtr = &earth;
  gui::camSpecatePtr = &cameraSpectate;
  gui::airplanePtr = &airplane;
  gui::sunPtr = &sun;

  earth.update();
  earth.bakeOpticalDepth();

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static double titleTimer = glfwGetTime();
    static double prevTime = titleTimer;
    static double currTime = prevTime;

    constexpr float fpsLimit = 1.f / 90.f;
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

    // Update window title every 0.3 seconds
    if (currTime - titleTimer >= 0.3) {
      gui::fps = static_cast<u16>(1.f / global::dt);
      titleTimer = currTime;
    }

    global::profiler->clearTasks();

    earth.update();
    airplane.update(earth);

    earthShader.setUniform1f("u_time", global::time);

    sun.setUniforms(earthShader);
    sun.setUniforms(airplaneShader);
    sun.setUniforms(atmosphereShader);

    // ===== Main scene =========================================== //

    fboScreen.bind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE + !global::wireframeMode);

    earth.draw(cameraATM, frustum::Frustum(cameraAirplane), earthShader);

    airplane.draw(cameraATM, airplaneShader);

    // ===== Post Processing ====================================== //

    FBO::unbind();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    screenColorTexture.bind(0);
    screenDepthTexture.bind(1);

    atmosphereShader.setUniform1f("u_gamma", global::gamma);
    earth.drawAtmosphere(cameraATM, atmosphereShader);

    screenColorTexture.unbind();
    screenDepthTexture.unbind();

    airplane.drawTrails(cameraATM, trailShader);
    airplane.drawLights(cameraATM, lightShader);
    airplane.drawDirections(cameraATM, directionShader);

    cameraAirplane.draw(cameraATM, linesShader);

    if (global::drawGlobalAxis) {
      Mesh::drawDirectionLine(cameraATM, directionShader, {}, {1e6f, 0.f, 0.f}, global::red);
      Mesh::drawDirectionLine(cameraATM, directionShader, {}, {0.f, 1e6f, 0.f}, global::green);
      Mesh::drawDirectionLine(cameraATM, directionShader, {}, {0.f, 0.f, 1e6f}, global::blue);
    }
    // ============================================================ //

    glDisable(GL_FRAMEBUFFER_SRGB);
    gui::draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  gui::shutdown();
  glfwTerminate();
  delete global::profiler;

  return 0;
}

