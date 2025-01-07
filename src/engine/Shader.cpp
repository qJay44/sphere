#include "Shader.hpp"

#include <string>

#include "glm/gtc/type_ptr.hpp"

static GLuint load(const std::string& path, int type) {
  std::string shaderStr = readFile(path);
  const char* shaderStrPtr = shaderStr.c_str();
  GLuint shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &shaderStrPtr, NULL);

  return shaderId;
}

static GLuint compile(const std::string& path, int type) {
  GLuint shaderId = load(path, type);
  GLint hasCompiled;
  char infoLog[1024];

  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &hasCompiled);

  // if GL_FALSE
  if (!hasCompiled) {
    glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
    printf("\nShader: %s\n", path.c_str());
    printf("\n===== Shader compilation error =====\n\n%s", infoLog);
    printf("====================================\n\n");
  }

  return shaderId;
}

static void link(GLuint program) {
  GLint hasLinked;
  char infoLog[1024];

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &hasLinked);

  // if GL_FALSE
  if (!hasLinked) {
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    printf("\n===== Shader link error =====\n\n%s", infoLog);
    printf("\n=============================\n\n");
  }
}

Shader::Shader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath)
  : vsPath(vsPath), fsPath(fsPath), gsPath(gsPath) {
  program = glCreateProgram();
  GLuint shaders[3];
  u8 idx = 0;

  shaders[idx++] = compile(vsPath, GL_VERTEX_SHADER);
  shaders[idx++] = compile(fsPath, GL_FRAGMENT_SHADER);

  if (gsPath.size())
    shaders[idx++] = compile(gsPath, GL_GEOMETRY_SHADER);

  for (int i = 0; i < idx; i++)
    glAttachShader(program, shaders[i]);

  link(program);

  for (int i = 0; i < idx; i++)
    glDeleteShader(shaders[i]);
}

const std::string& Shader::getVertexShaderPath() const {return vsPath;}
const std::string& Shader::getFragmentShaderPath() const {return vsPath;}
const std::string& Shader::getGeometryShaderPath() const {return vsPath;}

void Shader::activate() const {
  glUseProgram(program);
}

void Shader::setUniform3f(const std::string& name, const vec3& v) const {
  glUniform3f(glGetUniformLocation(program, name.c_str()), v.x, v.y, v.z);
}

void Shader::setUniformMatrix4f(const std::string& name, const mat4& m) const {
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, value_ptr(m));
}

