#include "Camera.hpp"

#include <cassert>

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/trigonometric.hpp"

#include "mesh/Mesh.hpp"
#include "mesh/meshes.hpp"

Camera::Camera(vec3 pos, vec3 orientation, float sensitivity)
  : position(pos),
    orientation(orientation),
    sensitivity(sensitivity) {
  calcView();
};

const float& Camera::getFov()        const { return fov;         }
const vec3& Camera::getOrientation() const { return orientation; }
const vec3& Camera::getPosition()    const { return position;    }
const vec3& Camera::getUp()          const { return up;          }
const mat4& Camera::getMatrix()      const { return mat;         }

vec3 Camera::getBack()    const { return  transpose(view)[2]; }
vec3 Camera::getLeft()    const { return -transpose(view)[0]; }
vec3 Camera::getRight()   const { return -getLeft(); }
vec3 Camera::getForward() const { return -getBack(); }
vec3 Camera::getBottom()  const { return -getUp(); }

void Camera::setIncreasedSpeed()   { speed = 12.f * global::dt; }
void Camera::setNormalSpeed()      { speed = 3.f  * global::dt; }

void Camera::update() {
  float aspectRatio = static_cast<float>(global::winWidth) / global::winHeight;
  mat4 proj = glm::perspective(glm::radians(fov), aspectRatio, global::nearPlane, global::farPlane);

  calcView();
  mat = proj * view;
}

void Camera::draw(Camera& camToDraw, const Shader& shader) const {
  if (&camToDraw != this) {
    camToDraw.moveByMouse(global::winWidth * 0.5f, global::winHeight * 0.5f);
    camToDraw.update();

    Mesh camMesh = meshes::cube(camToDraw.getPosition(), 0.1f, {1.f, 0.f, 1.f});
    Mesh frustum = meshes::frustum(camToDraw);
    camMesh.draw(this, shader);
    frustum.draw(this, shader);
  }
}

void Camera::moveForward() { position +=  orientation * speed; }
void Camera::moveBack()    { position += -orientation * speed; }
void Camera::moveLeft()    { position += -normalize(cross(orientation, up)) * speed; }
void Camera::moveRight()   { position +=  normalize(cross(orientation, up)) * speed; }

void Camera::moveUp()   { position +=  up * speed; }
void Camera::moveDown() { position += -up * speed; }

void Camera::moveByMouse(const double& x, const double& y) {
  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  double rotX = sensitivity * (y - global::winHeight * 0.5f) / global::winHeight;
  double rotY = sensitivity * (x - global::winWidth * 0.5f) / global::winWidth;

  float radRotX = static_cast<float>(glm::radians(-rotX));
  float radRotY = static_cast<float>(glm::radians(-rotY));

  calcOrientation(radRotX, radRotY);
}

void Camera::calcView() {
  view = lookAt(position, position + orientation, up);
}

void Camera::calcOrientation(const float& radRotX, const float& radRotY) {
  // Calculates upcoming vertical change in the Orientation
  vec3 newOrientation = rotate(orientation, radRotX, normalize(cross(orientation, up)));

  // Decides whether the next vertical Orientation is legal or not
  if (!(angle(newOrientation, up) <= glm::radians(5.f) || angle(newOrientation, -up) <= glm::radians(5.f)))
    orientation = newOrientation;

  // Rotates the Orientation left and right
  orientation = rotate(orientation, radRotY, up);
}

