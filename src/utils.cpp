#include "utils.hpp"

#include <fstream>
#include <sstream>

std::string readFile(const std::string& path) {
  std::ifstream ifs(path);
  std::ostringstream oss;
  oss << ifs.rdbuf();
  std::string entireFile = oss.str();

  return entireFile;
}
