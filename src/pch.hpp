#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::uvec2;
using glm::uvec3;
using glm::uvec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::dvec2;
using glm::dvec3;
using glm::dvec4;

using glm::mat4;

#ifdef _WIN32
  #include <direct.h>
  #define CHDIR(p) _chdir(p);
  #include "glad/glad.h" // Windows version?
#else
  #include "glad/gl.h"
  #include <unistd.h>
  #define CHDIR(p) chdir(p);
#endif

#include "GLFW/glfw3.h"
#include "defines.hpp"

#define UTILS_ENABLE_GLM
#include "utils/utils.hpp"
#include "utils/types.hpp"

// STL
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <format>
#include <functional>
#include <future>
#include <list>
#include <span>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <vips/vips8>
