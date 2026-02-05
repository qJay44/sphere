#pragma once

struct RBO {
public:
  RBO() {
    gen();
  }

  static void unbind() {
    glBindFramebuffer(GL_RENDERBUFFER, 0);
  }

  void gen() {
    glGenRenderbuffers(size, &id);
  }

  void bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, id);
  }

  void storage(GLenum internalFormat, ivec2 size) const {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, id);
    unbind();
  }

  void clear() {
    if (id) glDeleteRenderbuffers(size, &id);
    id = 0;
  }

private:
  static constexpr GLsizei size = 1;
  GLuint id = 0;
};

