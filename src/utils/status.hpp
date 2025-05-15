#pragma once

struct status {
public:
  status() = delete;
  static void start(const std::string& title, const std::string& what);
  static void end(bool success);
};

