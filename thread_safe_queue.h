#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include "task.h"

class ThreadSafeQueue {

  public:
    ThreadSafeQueue();
    virtual ~ThreadSafeQueue();
    
    void push(std::shared_ptr<Task> p_url_data);
    void setEndWait(bool wait);
    std::shared_ptr<Task> pop();
    size_t size();
    void clear();
    std::shared_ptr<Task> wait_and_pop();

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::queue<std::shared_ptr<Task>> m_items;
    bool m_end_wait = false;


};

#endif //THREAD_SAFE_QUEUE_H
