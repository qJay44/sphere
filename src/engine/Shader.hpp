#pragma once

#include <string>

class Shader {
public:
  Shader(const fspath& vsName, const fspath& fsName, const fspath& gsName = "");

  const fspath& getVertexShaderName() const;
  const fspath& getFragmentShaderName() const;
  const fspath& getGeometryShaderName() const;

  GLint getUniformLoc(const std::string& name) const;

  void use() const;

  void setUniform1f(const GLint& loc, const float& n) const;
  void setUniform3f(const GLint& loc, const vec3& v) const;
  void setUniform4f(const GLint& loc, const vec4& v) const;
  void setUniformMatrix4f(const GLint& loc, const mat4& m) const;
  void setUniformTexture(const GLint& loc, const GLuint& unit) const;

  void setUniform1f(const std::string& name, const float& n) const;
  void setUniform3f(const std::string& name, const vec3& v) const;
  void setUniform4f(const std::string& name, const vec4& v) const;
  void setUniformMatrix4f(const std::string& name, const mat4& m) const;
  void setUniformTexture(const std::string& name, const GLuint& unit) const;

private:
  GLuint program;
  const fspath& vsName;
  const fspath& fsName;
  const fspath& gsName;
};
