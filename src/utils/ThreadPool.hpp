#pragma once

/* Sources:
 * https://stackoverflow.com/a/32593825/17694832
 * https://github.com/johnBuffer/VerletSFML-Multithread/blob/main/src/thread_pool/thread_pool.hpp
 */

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include <condition_variable>

class ThreadPool {
public:
  void start();
  void start(uint32_t nthreads);
  void queueJob(const std::function<void()>& job);
  void waitForCompletion() const;
  void stop();

  const int size() const;

private:
  bool shouldTerminate = false;           // Tells threads to stop looking for jobs
  std::mutex queueMutex;                  // Prevents data races to the job queue
  std::condition_variable mutexCondition; // Allows threads to wait on new jobs or termination
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> jobs;
  std::atomic<uint32_t> remainingTasks = 0;

private:
  static void wait();
  void threadLoop();
};
