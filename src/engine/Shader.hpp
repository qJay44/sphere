#pragma once

#include <string>

class Shader {
public:
  Shader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath = "");

  const std::string& getVertexShaderPath() const;
  const std::string& getFragmentShaderPath() const;
  const std::string& getGeometryShaderPath() const;

  void activate() const;

  void setUniform3f(const std::string& name, const vec3& v) const;
  void setUniformMatrix4f(const std::string& name, const mat4& m) const;

private:
  GLint program;
  const std::string& vsPath;
  const std::string& fsPath;
  const std::string& gsPath;
};
