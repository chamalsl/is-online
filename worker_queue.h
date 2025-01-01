#include "task.h"
#include "thread_safe_queue.h"
#include <gtkmm.h>
#include <vector>
#include <thread>

class WorkerQueue{
  public:
    WorkerQueue(Glib::Dispatcher* p_dispatcher);
    virtual ~WorkerQueue();
    void processTasks();
    void addTask(std::shared_ptr<Task> p_url_data);
    void stopAllTasks();
    size_t size();
    void addResult(std::shared_ptr<Task> p_url_data);
    std::shared_ptr<Task> getResult();

    bool running = false;
  
  private:
    short m_maxThreads = 3;
    Glib::Dispatcher* m_dispatcher;
    ThreadSafeQueue m_tasks;
    ThreadSafeQueue m_results;
    std::vector<std::thread*> m_threads;
    
};
