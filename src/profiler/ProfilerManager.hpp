#pragma once

#include <cassert>
#include <vector>

#undef IM_NEW
#undef IM_FREE

#include "ImGuiProfilerRenderer.h"
#include "ProfilerTask.h"
#include "utils/types.hpp"

class ProfilerManager {
public:
  ProfilerManager(size_t framesCount) : graph(framesCount) {}

  // NOTE: Call this every frame
  void clearTasks() {
    tasks.clear();
  }

  [[nodiscard]]
  size_t startTask(const std::string& name, const u32* color = nullptr) {
    assert(tasks.size() < 20);
    size_t i = tasks.size();

    legit::ProfilerTask task;
    task.name = name;
    task.color = color ? *color : getColorBright(tasks.size());
    task.startTime = 0.f;
    addTask(task);

    return i;
  }

  void endTask(size_t i, float durationMs) {
    assert(i < tasks.size());
    tasks[i].endTime = durationMs;
  }

  void addTask(const legit::ProfilerTask& task) {
    tasks.push_back(task);
  }

  [[nodiscard]]
  const legit::ProfilerTask& getTask(size_t i) const {
    assert(i < tasks.size());
    return tasks[i];
  }

  void render(int graphWidth, int legendWidth, int height, int frameIndexOffset = 1) {
    graph.LoadFrameData(tasks.data(), tasks.size());
    graph.RenderTimings(graphWidth, legendWidth, height, frameIndexOffset, 1.f / 60.f);
  }

private:
  std::vector<legit::ProfilerTask> tasks;
  ImGuiUtils::ProfilerGraph graph;

private:
  static const u32& getColorBright(size_t i) {
    using namespace legit::Colors;

    static constexpr u32 colors[8] = {
      turqoise, emerald, peterRiver, amethyst, sunFlower, carrot, alizarin, clouds
    };

    return colors[i % 8];
  }

  static const u32& getColorDim(size_t i) {
    using namespace legit::Colors;

    static constexpr u32 colors[8] = {
      greenSea, nephritis, belizeHole, wisteria, orange, pumpkin, pomegranate, silver
    };

    return colors[i % 8];
  }
};

#undef IM_NEW
#undef IM_FREE

