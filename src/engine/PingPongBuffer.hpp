#pragma once

#include <cassert>

#include "mesh/BufferObject.hpp"

class PingPongBuffer {
public:
  PingPongBuffer() = default;

  PingPongBuffer(GLenum target, GLsizeiptr maxBufferSize, GLenum usage) {
    for (BufferObject& pbo : pp) {
      pbo = BufferObject{target};
      pbo.allocate(nullptr, maxBufferSize, usage);
    }
  }

  ~PingPongBuffer() {
    for (BufferObject& pbo : pp)
      pbo.clear();
  }

  void bind() const {
    currPBO->bind();
  }

  void unbind() const {
    currPBO->unbind(); // Target related
  }

  // bind -> map -> unmap -> unbind
  [[nodiscard]]
  void* map(size_t size) const {
    void* pboPtr = glMapBufferRange(currPBO->target, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    assert(pboPtr);

    return pboPtr;
  }

  void unmap() const {
    glUnmapBuffer(currPBO->target);
  }

  void next() {
    idx = 1 - idx;
    currPBO = &pp[idx];
  }

private:
  BufferObject pp[2];
  size_t idx = 0;

  BufferObject* currPBO = &pp[idx];
};

