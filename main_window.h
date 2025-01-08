#include "task.h"
#include "data_manager.h"
#include "url_item_box.h"
#include "worker_queue.h"
#include <gtkmm.h>
#include <thread>
#include <map>
#include <memory>
#include <cstdint>



class MainWindow: public Gtk::Window {

public:
  MainWindow();
  virtual ~MainWindow();
  void addURL(std::shared_ptr<URLData> p_url);

protected:
  Gtk::Entry m_urlText;
  Gtk::Button m_urlAddBtn;
  Gtk::Button m_checkAllBtn;
  Gtk::Button m_helpBtn;
  Gtk::Box m_addForm;
  Gtk::Grid m_urlListGrid;
  Gtk::ScrolledWindow m_scrolledList;
  Gtk::VBox m_mainContainer;
  Gtk::AboutDialog m_aboutDialog;
  
private:
  void isOnline(uint32_t id);
  void updateURL(uint32_t id);
  void deleteURL(uint32_t id);
  void showError(std::string error_msg);
  std::string validateURL(std::string p_url);
  void createNewURL();
  void checkAllURLs();
  bool cleanUp(GdkEventAny* any_event);
  void updateURLCheckResult();
  void startWorkerThread();
  void enableURLItemButtons(bool enable);
  size_t getNextTaskId();
  void showAbout();
  
  bool m_checkingAll = false;
  uint32_t m_nextRow = 1;
  size_t next_task_id = 0;
  std::unique_ptr<std::string> m_version;
  std::unique_ptr<DataManager> m_dataManager = NULL;
  std::unique_ptr<WorkerQueue> m_workerQueue = NULL;
  std::thread* m_workersThread = NULL;
  std::map<uint32_t,URLItemBox*> m_urlItemWidgets;
  std::map<size_t, TaskStatus> m_taskStatusMap;
  Glib::Dispatcher m_dispatcher;
  
};
