#include "Trail.hpp"

#include "../mesh/Mesh.hpp"

Trail::Trail(vec3 color)
  : color(color) {}

void Trail::add(const vec3& pos, const float& duration) {
  trailPoints.push_back({pos, global::time, duration});
}

void Trail::update() {
  while (!trailPoints.empty() && (global::time - trailPoints.front().timestamp > trailPoints.front().duration))
    trailPoints.erase(trailPoints.begin());
}

void Trail::draw(const Camera* cam, Shader& shader) const {
  const size_t count = trailPoints.size();

  if (count > 1) {
    std::vector<VertexPC> vertices(count);
    std::vector<GLuint> indices((count - 1) * 2);
    size_t indicesIdx = 0;

    for (size_t i = 0; i < count; i++) {
      vertices[i] = {trailPoints[i].position, color};
      if (i != count - 1) {
        indices[indicesIdx++] = i;
        indices[indicesIdx++] = i + 1;
      }
    }

    shader.setUniform3f("u_closestPos", trailPoints.back().position);

    Mesh(vertices, indices, GL_LINES, true).draw(cam, shader);
  }
}

