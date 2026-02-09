#include "Trail.hpp"

#include <vector>

Trail::Trail() : Mesh({}, GL_TRIANGLE_STRIP, GL_DYNAMIC_DRAW) {}

void Trail::add(const vec3& pos) {
  points.push_back({pos, global::time});
}

void Trail::update(const Camera* cam) {
  while (!points.empty() && (global::time - points.front().timestamp > duration))
    points.erase(points.begin());

  std::vector<Vertex4> vertices;

  for (size_t i = 0; i < points.size(); i++) {
    const vec3& pos = points[i].position;
    vec3 dir = global::up;

    if (i < points.size() - 1)
      dir = normalize(points[i + 1].position - pos);
    else if (i)
      dir = normalize(pos - points[i - 1].position);

    vec3 toCam = normalize(pos - cam->getPosition());
    vec3 side = normalize(cross(dir, toCam)) * width;

    float age = (global::time - points[i].timestamp) / duration;
    float alpha = glm::clamp(1.f - age, 0.f, 1.f);

    vertices.push_back({pos + side, vec3(alpha)});
    vertices.push_back({pos - side, vec3(alpha)});
  }

  updateData(vertices, GL_DYNAMIC_DRAW);
}

