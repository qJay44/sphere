#pragma once

#include <chrono>

struct ScopedProfilerTask {
  size_t taskIdx;
  std::chrono::steady_clock::time_point start;
  bool ended = false;

  ScopedProfilerTask(const std::string& taskName, u32 color = 0) : start(std::chrono::steady_clock::now()) {
    assert(global::profiler);
    taskIdx = global::profiler->startTask(taskName, color);
  }

  ~ScopedProfilerTask() {
    end();
  }

  void end() {
    if (ended)
      return;

    using namespace std::chrono;

    auto end = steady_clock::now();
    auto dur = end - start;
    float durationSec = duration_cast<duration<float>>(dur).count();

    assert(global::profiler);
    global::profiler->endTask(taskIdx, durationSec);
    ended = true;
  }
};

