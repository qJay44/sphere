#include "Shader.hpp"

#include <cstdio>
#include <string>

#include "glm/gtc/type_ptr.hpp"

static GLuint load(const fspath& filename, int type) {
  std::string shaderStr = readFile(R"(engine\shaders)" / filename);
  const char* shaderStrPtr = shaderStr.c_str();
  GLuint shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &shaderStrPtr, NULL);

  return shaderId;
}

static GLuint compile(const fspath& filename, int type) {
  GLuint shaderId = load(filename, type);
  GLint hasCompiled;
  char infoLog[1'024];

  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &hasCompiled);

  // if GL_FALSE
  if (!hasCompiled) {
    glGetShaderInfoLog(shaderId, 1'024, NULL, infoLog);
    std::string fmt = clrp::prepare(clrp::ATTRIBUTE::BOLD, clrp::FG::RED);
    std::string head = std::format("\n===== Shader compilation error ({}) =====\n\n", filename.string().c_str());
    printf(fmt.c_str(), head.c_str());
    puts(infoLog);
    printf(fmt.c_str(), "====================================\n");
  }

  return shaderId;
}

static void link(GLuint program) {
  GLint hasLinked;
  char infoLog[1'024];

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &hasLinked);

  // if GL_FALSE
  if (!hasLinked) {
    glGetProgramInfoLog(program, 1'024, NULL, infoLog);
    std::string fmt = clrp::prepare(clrp::ATTRIBUTE::BOLD, clrp::FG::RED);
    printf(fmt.c_str(), "\n===== Shader link error =====\n\n");
    puts(infoLog);
    printf(fmt.c_str(), "=============================\n\n");
  }
}

Shader::Shader(const fspath& vsName, const fspath& fsName, const fspath& gsName)
  : vsName(vsName),
    fsName(fsName),
    gsName(gsName) {
  program = glCreateProgram();
  GLuint shaders[3];
  u8 idx = 0;

  shaders[idx++] = compile(vsName, GL_VERTEX_SHADER);
  shaders[idx++] = compile(fsName, GL_FRAGMENT_SHADER);

  if (!gsName.empty()) shaders[idx++] = compile(gsName, GL_GEOMETRY_SHADER);

  for (int i = 0; i < idx; i++)
    glAttachShader(program, shaders[i]);

  link(program);

  for (int i = 0; i < idx; i++)
    glDeleteShader(shaders[i]);
}

const fspath& Shader::getVertexShaderName() const { return vsName; }
const fspath& Shader::getFragmentShaderName() const { return vsName; }
const fspath& Shader::getGeometryShaderName() const { return vsName; }

void Shader::use() const { glUseProgram(program); }

void Shader::setUniform3f(const std::string& name, const vec3& v) const {
  use();
  glUniform3f(glGetUniformLocation(program, name.c_str()), v.x, v.y, v.z);
}

void Shader::setUniform4f(const std::string& name, const vec4& v) const {
  use();
  glUniform4f(glGetUniformLocation(program, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setUniformMatrix4f(const std::string& name, const mat4& m) const {
  use();
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, value_ptr(m));
}

void Shader::setUniformTexture(const std::string& name, const GLuint& unit) const {
  use();
  glUniform1i(glGetUniformLocation(program, name.c_str()), unit);
}
