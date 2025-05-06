#pragma once

#include "../Camera.hpp"
#include "Plane.hpp"

namespace frustum {

struct Frustum {
  Plane topFace;
  Plane bottomFace;

  Plane rightFace;
  Plane leftFace;

  Plane farFace;
  Plane nearFace;

  Frustum(const Camera& cam) {
    ivec2 winSize = getWinSize();
    float aspectRatio = static_cast<float>(winSize.x) / winSize.y;

    const float& camNear = cam.getNearPlane();
    const float& camFar = cam.getFarPlane();
    const vec3& camPos = cam.getPosition();
    vec3 camCrossUp = normalize(cross(cam.getForward(), cam.getLeft()));

    float halfVSide = camFar * tanf(cam.getFov() * 0.5f);
    float halfHSide = halfVSide * aspectRatio;
    vec3 frontMultFar = camFar * cam.getForward();

    vec3 planeRight = normalize(cross(frontMultFar + cam.getRight() * halfHSide, camCrossUp));
    vec3 planeLeft = normalize(cross(camCrossUp, frontMultFar + cam.getLeft() * halfHSide));
    vec3 planeUp = normalize(cross(frontMultFar + camCrossUp * halfVSide, cam.getLeft()));
    vec3 planeDown = normalize(cross(frontMultFar + camCrossUp * halfVSide, cam.getRight()));

    vec3 right = normalize(cross(frontMultFar + cam.getRight() * halfHSide,  camCrossUp    ));
    vec3 up    = normalize(cross(frontMultFar + camCrossUp     * halfVSide,  cam.getLeft() ));
    vec3 left  = normalize(cross(frontMultFar + cam.getLeft()  * halfHSide, -camCrossUp    ));
    vec3 down  = normalize(cross(frontMultFar - camCrossUp     * halfVSide,  cam.getRight()));

    nearFace   = {cam.getForward(), dot(camPos + camNear * cam.getForward(), cam.getForward())};
    farFace    = {cam.getBack(), dot(camPos + frontMultFar, cam.getBack())};
    rightFace  = {right, dot(camPos, right)};
    leftFace   = {left , dot(camPos, left)};
    topFace    = {up   , dot(camPos, up)};
    bottomFace = {down , dot(camPos, down)};
  }
};

}

