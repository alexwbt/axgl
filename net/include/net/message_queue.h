#pragma once

#include <mutex>
#include <queue>
#include <flatbuffers/flatbuffers.h>

namespace net
{

  template <typename element_t>
  class message_queue
  {
  private:
    std::mutex queue_mutex_;
    std::queue<element_t> queue_;

  public:
    void push(element_t element)
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      queue_.push(std::move(element));
    }

    /**
     * @returns pointer to first element of queue, nullptr if queue is empty.
     */
    element_t* front()
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      if (queue_.empty())
        return nullptr;
      return &queue_.front();
    }

    void pop()
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      queue_.pop();
    }

    void handle_all(std::function<void(element_t*)> handler)
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      while (!queue_.empty())
      {
        handler(&queue_.front());
        queue_.pop();
      }
    }
  };

}
