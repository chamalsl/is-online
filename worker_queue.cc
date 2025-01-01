#include "worker_queue.h"
#include <iostream>
#include <curl/curl.h>

WorkerQueue::WorkerQueue(Glib::Dispatcher* p_dispatcher){
  m_dispatcher = p_dispatcher;
}

WorkerQueue::~WorkerQueue() {
  for(int i=0; i < m_threads.size(); i++) {
    std::thread* thread_tmp = m_threads[i];
    if (thread_tmp->joinable()){
      thread_tmp->join();
      delete thread_tmp;
    }
  }
  m_threads.clear();

}

static size_t getResponseFromCurl(void* contents, size_t size, size_t nmemb, void* user_data){
  //Do nothing, because response is not necessary.
  //std::cout << "Curl write data " << size << "\n";
  return size*nmemb;
}

static long checkStatus(std::string p_url){
  
  const char* url = p_url.c_str();
  //std::cout << "URL to check " << url << " - " << strlen(url) << "\n";
  char *url_encoded;
  CURLU *url_obj = curl_url();
  
  int rc = curl_url_set(url_obj, CURLUPART_URL, url, CURLU_URLENCODE);
  if (rc != 0){
    curl_url_cleanup(url_obj);
    return -1;
  }

  rc = curl_url_get(url_obj,CURLUPART_URL, &url_encoded, 0);
  if (rc != 0){
    curl_url_cleanup(url_obj);
    return -1;
  }
  //std::cout << "URL to check encoded " << url_encoded << "\n";
  CURL* curl_conn = curl_easy_init();
  curl_easy_setopt(curl_conn, CURLOPT_URL, url_encoded);
  curl_easy_setopt(curl_conn, CURLOPT_HTTPGET, 1L);
  curl_easy_setopt(curl_conn, CURLOPT_TIMEOUT, 3);
  curl_easy_setopt(curl_conn, CURLOPT_WRITEFUNCTION, getResponseFromCurl);
  std::string data;
  curl_easy_setopt(curl_conn, CURLOPT_WRITEDATA, (void *)&data);
  
  CURLcode code = curl_easy_perform(curl_conn);
  long status = -1;
  curl_easy_getinfo(curl_conn, CURLINFO_RESPONSE_CODE ,&status);
  //std::cout << "Is online " << status << " " << code << "\n";
  curl_url_cleanup(url_obj);
  curl_free(url_encoded);
  curl_easy_cleanup(curl_conn);
  return status;
}

void WorkerQueue::processTasks() {
  m_tasks.setEndWait(false);
  while(running){
    if (m_threads.size() == m_maxThreads) {
      for(int i=0; i < m_threads.size(); i++) {
        std::thread* thread_tmp = m_threads[i];
        if (thread_tmp->joinable()){
          thread_tmp->join();
          delete thread_tmp;
        }
      }
      m_threads.clear();
    }
  
    std::shared_ptr<Task> task = m_tasks.wait_and_pop();
    if (task.get() == nullptr) {
      continue;
    }
    //std::cout << "Task received " << url->m_url << "\n";
    std::thread* worker_thread = new std::thread(
      [this](size_t task_id, unsigned int url_id, std::string url){
        std::shared_ptr<Task> resultStart = std::make_shared<Task>();
        resultStart->task_id = task_id;
        resultStart->url_data = std::make_shared<URLData>();
        resultStart->url_data->id = url_id;
        resultStart->m_status = TaskStatus::STARTED;
        addResult(resultStart);
        m_dispatcher->emit();

        long status = checkStatus(url);

        std::shared_ptr<Task> result = std::make_shared<Task>();;
        result->task_id = task_id;
        result->url_data = std::make_shared<URLData>();
        result->url_data->id = url_id;
        result->url_data->m_url = url;
        result->url_data->m_status = status;
        result->m_status = TaskStatus::FINISHED;
        addResult(result);
        m_dispatcher->emit();
      }, task->task_id,task->url_data->id, task->url_data->m_url);
    
    m_threads.push_back(worker_thread); 
  }
}

void WorkerQueue::addTask(std::shared_ptr<Task> p_task) {
  m_tasks.push(p_task);
}

void WorkerQueue::stopAllTasks() {
  m_tasks.clear();
  m_tasks.setEndWait(true);
}

size_t WorkerQueue::size(){
  return m_tasks.size();
}

void WorkerQueue::addResult(std::shared_ptr<Task> p_task) {
  m_results.push(p_task);
}

std::shared_ptr<Task> WorkerQueue::getResult() {
  return m_results.pop();
}


