#include "thread_safe_queue.h"

ThreadSafeQueue::ThreadSafeQueue(){
}


ThreadSafeQueue::~ThreadSafeQueue(){
}
    
void ThreadSafeQueue::push(std::shared_ptr<Task> p_task) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_items.push(p_task);
  m_condition.notify_one();
}

void ThreadSafeQueue::setEndWait(bool wait) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_end_wait = wait;
  m_condition.notify_one();
}

std::shared_ptr<Task> ThreadSafeQueue::pop() {
  std::lock_guard<std::mutex> lock(m_mutex);
  if (m_items.empty()){
    return nullptr;
  }
  
  std::shared_ptr<Task> task = m_items.front();
  m_items.pop();
  return task;
}

size_t ThreadSafeQueue::size() {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_items.size();
}

void ThreadSafeQueue::clear(){
  while(pop() != nullptr){}
}

std::shared_ptr<Task> ThreadSafeQueue::wait_and_pop() {
  std::unique_lock lock(m_mutex);
  m_condition.wait(lock, [this] {return (!m_items.empty() || m_end_wait);});

  if (m_items.empty()){
    return nullptr;
  }
  std::shared_ptr<Task> task = m_items.front();
  m_items.pop();
  return task;
}
