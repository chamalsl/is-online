#include "main_window.h"
#include "utils.h"
#include <iostream>
#include <gtkmm/application.h>
#include <curl/curl.h>


int main(int argc, char *argv[]){
  auto app = Gtk::Application::create(argc, argv, "com.rammini.WebSiteChecker");
  MainWindow main_window;
  curl_global_init(CURL_GLOBAL_ALL);

  return app->run(main_window);
}
