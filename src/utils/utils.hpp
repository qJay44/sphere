#pragma once

#include <format>
#include <string>

#include "clrp.hpp"
#include "status.hpp"

struct RunOnce {
  template<typename T> RunOnce(T&& f) { f(); }
};


std::string readFile(const fspath& path);

void printTabs(u8 n);
void error(const std::string& msg);
void warning(const std::string& msg);

inline ivec2 getWinSize() {
  ivec2 winSize;
  glfwGetWindowSize(global::window, &winSize.x, &winSize.y);
  return winSize;
}

inline void print(const vec3& v, const std::string& name = "vec") {
  printf("%s\n", std::format("{}: {}, {}, {}", name, v.x, v.y, v.z).c_str());
}

inline void print(const vec2& v, const std::string& name = "vec") {
  printf("%s\n", std::format("{}: {}, {}", name, v.x, v.y).c_str());
}

