#ifndef TASK_H
#define TASK_H

#include "url_data.h"
#include <memory>

enum TaskStatus{
  NEW,
  STARTED,
  FINISHED,
  CANCELLED,
};

class Task{
  public:
  size_t task_id = 0;
  TaskStatus m_status = TaskStatus::NEW;
  std::shared_ptr<URLData> url_data = nullptr;
};

#endif //#ifndef TASK_H