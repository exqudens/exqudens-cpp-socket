#include "TestThreadPool.hpp"

TestThreadPool::TestThreadPool(): TestThreadPool(
    std::thread::hardware_concurrency()
) {
}

TestThreadPool::TestThreadPool(size_t queueSize): TestThreadPool(
    queueSize,
    queueSize
) {
}

TestThreadPool::TestThreadPool(size_t queueSize, size_t threadSize):
    queueSize(queueSize),
    stop(false)
{
  try {
    if (queueSize == 0) {
      throw std::invalid_argument(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): 'queueSize' is zero");
    }
    if (threadSize == 0) {
      throw std::invalid_argument(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + "): 'threadSize' is zero");
    }
    for(size_t i = 0; i < threadSize; i++) {
      threads.emplace_back([this]{
        while(true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->condition.wait(lock, [this]{ return this->stop || !this->queue.empty(); });
            if(this->stop && this->queue.empty()) {
              return;
            }
            task = std::move(this->queue.front());
            this->queue.pop();
          }
          task();
        }
      });
    }
  } catch (...) {
    std::throw_with_nested(std::runtime_error(std::string(__FUNCTION__) + "(" + __FILE__ + ":" + std::to_string(__LINE__) + ")"));
  }
}

TestThreadPool::~TestThreadPool() {
  {
    std::unique_lock<std::mutex> lock(mutex);
    stop = true;
  }
  condition.notify_all();
  for(std::thread& thread : threads) {
    thread.join();
  }
}
