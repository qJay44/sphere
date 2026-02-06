#include "Camera.hpp"

#include <cassert>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"

#include "mesh/Mesh.hpp"
#include "mesh/meshes.hpp"

Camera::Camera(vec3 pos, float yaw, float pitch) : Moveable(pos, yaw, pitch) {
  update();
};

const float& Camera::getNearPlane() const { return nearPlane; }
const float& Camera::getFarPlane()  const { return farPlane;  }
const float& Camera::getFov()       const { return fov;       }
const mat4&  Camera::getProj()      const { return proj;      }
const mat4&  Camera::getView()      const { return view;      }
const mat4&  Camera::getProjView()  const { return pv;        }

mat4 Camera::getProjViewInv() const {
  return glm::inverse(pv);
}

void Camera::setNearPlane(float p) { nearPlane = p; }
void Camera::setFarPlane(float p) { farPlane = p; }

void Camera::update() {
  vec2 winSize = global::getWinSize();

  aspectRatio = winSize.x / winSize.y;
  proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
  view = glm::lookAt(position, position + orientation, up);
  pv = proj * view;

  dvec2 winCenter = global::getWinCenter();
  if (!global::guiFocused)
    glfwSetCursorPos(global::window, winCenter.x, winCenter.y);
}

void Camera::draw(Camera& camToDraw, Shader& shader, u32 flags) const {
  if (&camToDraw != this) {
    static Mesh camMesh = meshes::cube({1.f, 0.f, 1.f});

    camToDraw.update();

    const vec3& camToDrawPos = camToDraw.position;

    camMesh.translate(camToDrawPos);

    Mesh frustumMesh = meshes::frustum(camToDraw);

    if (flags & CAMERA_FLAG_DRAW_RIGHT)
      meshes::line(camToDrawPos, camToDrawPos + camToDraw.getRight(), {1.f, 0.f, 0.f}).draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_UP)
      meshes::line(camToDrawPos, camToDrawPos + camToDraw.up, {0.f, 1.f, 0.f}).draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_FORWARD)
      meshes::line(camToDrawPos, camToDrawPos + camToDraw.getForward(), {0.f, 0.f, 1.f}).draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_MESH)
      camMesh.draw(this, shader);

    if (flags & CAMERA_FLAG_DRAW_FRUSTUM)
      frustumMesh.draw(this, shader);
  }
}

