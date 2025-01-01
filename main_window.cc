#include <iostream>
#include <string>
#include "main_window.h"

MainWindow::MainWindow()
:m_mainContainer(Gtk::Orientation::ORIENTATION_VERTICAL, 5),
 m_addForm(Gtk::Orientation::ORIENTATION_HORIZONTAL, 5),
 m_urlAddBtn("Add"),
 m_checkAllBtn("Check All")
{
  set_title("Is Online");
  set_default_size(600,600);

  std::string css_data = R"(
    #status_label {
      background-color:white;
      border: solid;
      border-width:1px;
      border-color: black;
    }

    #status_label_error {
      background-color:white;
      border: solid;
      border-width:1px;
      border-color: red;
    }

    #status_label_success {
      background-color:white;
      border: solid;
      border-width:1px;
      border-color: green;
    }
  )";
  auto css_provider = Gtk::CssProvider::create();
  css_provider->load_from_data(css_data);
  Gtk::StyleContext::add_provider_for_screen(Gdk::Screen::get_default(),css_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
  //URL Add Form
  m_urlAddBtn.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::createNewURL));
  m_checkAllBtn.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::checkAllURLs));
  m_helpBtn.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::showAbout));
  signal_delete_event().connect(sigc::mem_fun(*this, &MainWindow::cleanUp));
  
  m_mainContainer.set_homogeneous(false);
  m_urlText.set_width_chars(40);
  m_helpBtn.set_image_from_icon_name("help-about");
  m_addForm.pack_start(m_urlText, false, false, 5);
  m_addForm.add(m_urlAddBtn);
  m_addForm.add(m_checkAllBtn);
  m_addForm.add(m_helpBtn);
  m_mainContainer.pack_start(m_addForm, false, false, 5);
  m_addForm.set_valign(Gtk::ALIGN_BASELINE);
  m_mainContainer.set_valign(Gtk::ALIGN_FILL);

  m_aboutDialog.set_transient_for(*this);
  //m_aboutDialog.set_logo(Gdk::Pixbuf::create_from_resource("/images/logo.svg"));
  m_aboutDialog.set_logo_default();
  m_aboutDialog.set_program_name("Is Online");
  m_aboutDialog.set_copyright("Chamal De Silva");
  m_aboutDialog.set_license_type(Gtk::License::LICENSE_MIT_X11);

  m_urlListGrid.set_halign(Gtk::Align::ALIGN_CENTER);
  m_scrolledList.add(m_urlListGrid);
  m_scrolledList.set_policy(Gtk::PolicyType::POLICY_NEVER, Gtk::PolicyType::POLICY_AUTOMATIC);
  m_scrolledList.set_valign(Gtk::ALIGN_FILL);
  m_mainContainer.pack_end(m_scrolledList, true, true, 5);
  add(m_mainContainer);
  set_position(Gtk::WIN_POS_CENTER);
  //set_resizable(false);
  show_all_children();
  
  //Load data
  m_dataManager = std::make_unique<DataManager>();

  if (!m_dataManager->ensureDatabase()){
    showError("Could not create database!");
    exit(1);
  }

  std::vector<std::shared_ptr<URLData>>* url_list = m_dataManager->retriveURLs();
  if (url_list->empty()){
    m_checkAllBtn.set_sensitive(false);
  }
  for (std::shared_ptr<URLData> url: *url_list) {
    addURL(url);
  }
  
  m_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::updateURLCheckResult)); 
  m_workerQueue = std::make_unique<WorkerQueue>(&m_dispatcher);  
}

 
MainWindow::~MainWindow(){

}

void MainWindow::updateURLCheckResult(){
  //std::cout << "Result received \n"; 
  std::shared_ptr<Task> task = m_workerQueue->getResult();
  if (!task || !task->url_data || !task->url_data.get()){
    return;
  }

  if (m_taskStatusMap[task->task_id] == TaskStatus::CANCELLED){
    if (task->m_status == TaskStatus::FINISHED){
      m_taskStatusMap.erase(task->task_id);
    }
    return;
  }

  if (task->m_status == TaskStatus::STARTED){
    auto it = m_urlItemWidgets.find(task->url_data->id);
    if (it != m_urlItemWidgets.end()) {
      URLItemBox* url_item_box = it->second; 
      url_item_box->m_spinner.start();
    }
    return;
  }
  
  auto it = m_urlItemWidgets.find(task->url_data->id);
  if (it != m_urlItemWidgets.end()) {
    URLItemBox* url_item_box = it->second;
    url_item_box->m_spinner.stop(); 

    if (task->url_data->m_status > 0){
      url_item_box->m_statusLabel.set_label(std::to_string(task->url_data->m_status));
    }
    else{
      url_item_box->m_statusLabel.set_label("Error");
    }
    
    if (task->url_data->m_status != 200) {
      url_item_box->m_statusLabel.set_name("status_label_error");
    }else{
      url_item_box->m_statusLabel.set_name("status_label_success");
    }
  }
  m_taskStatusMap.erase(task->task_id);

  
  if (m_checkingAll) {
    if (m_workerQueue->size() == 0){
      enableURLItemButtons(true);
      m_checkAllBtn.set_label("Check All");
      m_checkingAll = false;
    }
  } 
  
}

void MainWindow::startWorkerThread()
{
  if (m_workerQueue->running == true) {
    return;
  }
  m_workerQueue->running = true;
  m_workersThread = new std::thread(
    [this](){
      m_workerQueue->processTasks();
  }); 
}

void MainWindow::enableURLItemButtons(bool enable)
{
  for (auto& it: m_urlItemWidgets) {
    URLItemBox* item_box = it.second;
    item_box->enableButtons(enable);
  }
}

size_t MainWindow::getNextTaskId()
{
    return ++next_task_id;
}

void MainWindow::showAbout()
{
  m_aboutDialog.present();
}

void MainWindow::showError(std::string error_msg){
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget* dialog = gtk_message_dialog_new(NULL, flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", error_msg.c_str());
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

std::string MainWindow::validateURL(std::string p_url){
  std::string error_msg = "";
  if (p_url.empty()){
    return "URL is empty!";
  }

  if (p_url.length() > 500) {
    return "URL contains more than 500 characters";
  }

  if (p_url.find("://") == std::string::npos){
    return "Scheme is not provided! (Ex: http://)";
  }

  //std::cout << scheme << "\n";
  if (p_url.find("http://") != 0 && p_url.find("https://") != 0){
    return "Only http and https URLs are supported!";
  }

  return "";
}

void MainWindow::createNewURL(){

  std::string url(m_urlText.get_text());
  std::string error_msg = validateURL(url);

  if (!error_msg.empty()){
    showError(error_msg);
    return;
  }

  std::shared_ptr<URLData> url_data = m_dataManager->addURL(url);
  addURL(url_data);
  m_urlText.set_text("");
  if (!m_checkAllBtn.get_sensitive()){
    m_checkAllBtn.set_sensitive(true);
  }
}

void MainWindow::checkAllURLs(){
  if (!m_checkingAll) 
  { 
    m_checkingAll = true;
    for (auto& it: m_urlItemWidgets) {
      URLItemBox* item_box = it.second;
      item_box->clearResult();
    }
    enableURLItemButtons(false);
    m_checkAllBtn.set_label("Cancel");
    startWorkerThread();
    std::vector<std::shared_ptr<URLData>>* url_list = m_dataManager->retriveURLs();
    for (std::shared_ptr<URLData> url: *url_list) {
      std::shared_ptr<Task> task =  std::make_shared<Task>();
      task->task_id = getNextTaskId();
      task->url_data = url;
      m_taskStatusMap[task->task_id] = TaskStatus::STARTED;
      m_workerQueue->addTask(task);
    }
  }else {
    m_workerQueue->stopAllTasks();
    for (auto& task_status: m_taskStatusMap){
      task_status.second = TaskStatus::CANCELLED;
    }
    m_checkAllBtn.set_label("Check All");
    m_checkingAll = false;
    enableURLItemButtons(true);
  }
}

bool MainWindow::cleanUp(GdkEventAny* any_event)
{
  m_workerQueue->stopAllTasks();
  for (auto& task_status: m_taskStatusMap){
    task_status.second = TaskStatus::CANCELLED;
  }
  m_workerQueue->running = false;
  if (m_workersThread && m_workersThread->joinable()){
    m_workersThread->join();
    delete m_workersThread;
    m_workersThread = nullptr;
  }
  return false;
}

void MainWindow::isOnline(uint32_t id){
  std::shared_ptr<URLData> url_data = m_dataManager->findURLDataById(id);
  if (url_data == NULL) {
    showError("System Error: Url data corrupted!");
    return;
  }
  URLItemBox* url_item_box = m_urlItemWidgets[id];
  url_item_box->clearResult();
  startWorkerThread();
  std::shared_ptr<Task> task =  std::make_shared<Task>();
  task->task_id = getNextTaskId();
  task->url_data = url_data;
  m_taskStatusMap[task->task_id] = TaskStatus::STARTED;
  m_workerQueue->addTask(task);
 
}

void MainWindow::updateURL(uint32_t id){
  //std::cout << "Update URL \n";
  if (m_urlItemWidgets.find(id) == m_urlItemWidgets.end()) {
    return;
  }
  
  URLItemBox* url_item_box = m_urlItemWidgets[id];
  std::string new_url(url_item_box->m_urlText.get_text());

  std::string error_msg = validateURL(new_url);

  if (!error_msg.empty()){
    showError(error_msg);
    return;
  }

  if (!m_dataManager->updateURL(id, new_url)){
    showError("Could not update!");
    return;
  }
}

void MainWindow::deleteURL(uint32_t id){
  //std::cout << "Delete URL \n";
  
  if (!m_dataManager->deleteURL(id)){
    showError("Could not delete!");
    return;
  }
  
  if (m_urlItemWidgets.find(id) != m_urlItemWidgets.end()) {
    URLItemBox* url_item_box = m_urlItemWidgets[id]; 
    m_urlItemWidgets.erase(id);
    m_urlListGrid.remove(*url_item_box);
  }
}

void MainWindow::addURL(std::shared_ptr<URLData> p_url){
  //std::cout << "Adding URL \n";
  auto url_item_box = new URLItemBox();
  url_item_box->m_urlText.set_text(p_url->m_url);  
  url_item_box->m_editBtn.signal_clicked().connect(sigc::bind<uint32_t>(sigc::mem_fun(*this, &MainWindow::MainWindow::updateURL),p_url->id));
  url_item_box->m_deleteBtn.signal_clicked().connect(sigc::bind<uint32_t>(sigc::mem_fun(*this, &MainWindow::MainWindow::deleteURL),p_url->id));
  url_item_box->m_checkBtn.signal_clicked().connect(sigc::bind<uint32_t>(sigc::mem_fun(*this, &MainWindow::MainWindow::isOnline),p_url->id));
  m_urlListGrid.attach(*Gtk::manage(url_item_box),1,m_nextRow,1,1);
  m_urlListGrid.show_all_children();
  m_urlItemWidgets[p_url->id] = url_item_box;
  m_nextRow++;
}
