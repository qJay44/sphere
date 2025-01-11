#pragma once

#include <string>

class Shader {
public:
  Shader(const fspath& vsName, const fspath& fsName, const fspath& gsName = "");

  const fspath& getVertexShaderName() const;
  const fspath& getFragmentShaderName() const;
  const fspath& getGeometryShaderName() const;

  void setUniform3f(const std::string& name, const vec3& v) const;
  void setUniform4f(const std::string& name, const vec4& v) const;
  void setUniformMatrix4f(const std::string& name, const mat4& m) const;

private:
  GLint program;
  const fspath& vsName;
  const fspath& fsName;
  const fspath& gsName;
};
