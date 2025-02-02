#pragma once

#include <string>

#include "types.hpp"
#include "ThreadPool.hpp"
#include "clrp.hpp"

std::string readFile(const fspath& path);

void printTabs(u8 n);
