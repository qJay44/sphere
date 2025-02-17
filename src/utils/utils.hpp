#pragma once

#include <string>

#include "ThreadPool.hpp"
#include "clrp.hpp"

std::string readFile(const fspath& path);

void printTabs(u8 n);
void error(const std::string& msg);
void warning(const std::string& msg);
