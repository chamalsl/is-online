#include "data_manager.h"
#include "utils.h"
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <stddef.h>
#include <filesystem>


DataManager::DataManager(){
}

DataManager::~DataManager(){

  if(m_db != NULL){
    sqlite3_close(m_db);
  }
}

sqlite3* DataManager::getDatabaseConnection(){
  if (m_db == NULL){
    int rc = sqlite3_open(database_path->c_str(), &m_db);
    if (rc){
      std::cout << "Could not open database " << sqlite3_errmsg(m_db);
    }
  }
  return m_db;
}

bool DataManager::ensureDatabase()
{
    std::string data_dir = Utils::getDataDirectory();
    if (data_dir.empty()){
      std::cout << "Could not find HOME directory.\n";
      return false;
    }
    std::filesystem::path data_path(data_dir);
    if (!std::filesystem::exists(data_path)){
       if (!std::filesystem::create_directories(data_path)){
        std::cout << "Could not created data directory - " << data_path.u8string() << "\n";
        return false;
       }
    }
    data_path.append("isonline_ram.db");
    database_path = std::make_unique<std::string>(data_path.u8string());
    if (!std::filesystem::exists(data_path)){
          sqlite3* db;
          int rc = sqlite3_open(data_path.u8string().c_str(), &db);
          if (rc){
            std::cout << "Could not open database " << sqlite3_errmsg(db);
            return false;
          }
          if (!createTable()){
            std::cout << "Could not create database tables.\n";
            return false;           
          }
          sqlite3_close(db);
    }
    std::cout << "Database path " << data_path.u8string() << "\n";
    return true;
}

bool DataManager::createTable(){

  sqlite3* db = getDatabaseConnection();
  
  char* err;
  sqlite3_stmt* stmt = NULL;
  std::string sql_create_table = "CREATE TABLE URLS(ID INTEGER PRIMARY KEY AUTOINCREMENT,URL VARCHAR(2000),POSITION INTEGER);";
  if (sqlite3_prepare_v2(db, sql_create_table.c_str(), -1, &stmt, NULL) != SQLITE_OK || stmt == NULL){
    return false;
  }
  
  int rc = sqlite3_step(stmt);
  
  if (rc != SQLITE_DONE){
    std::cout << "Could not create tables. - " << err;
    return false;
  }
  sqlite3_finalize(stmt);
  return true;
}


std::shared_ptr<URLData> DataManager::addURL(std::string p_url){
  sqlite3* db = getDatabaseConnection();
  
  char* err;
  sqlite3_stmt* stmt = NULL;
  std::string sql_insert = "INSERT INTO URLS (URL, POSITION) VALUES (?,?)";
  if (sqlite3_prepare_v2(db, sql_insert.c_str(), -1, &stmt, NULL) != SQLITE_OK || stmt == NULL){
    return nullptr;
  }
  
  sqlite3_bind_text(stmt, 1, p_url.c_str(), p_url.length(), SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, 0);
  int rc = sqlite3_step(stmt);
  
  if (rc != SQLITE_DONE){
    std::cout << "Could not insert. -  " << err;
    return nullptr;
  }
  sqlite3_finalize(stmt);
  //Retrive Last Insterted URL
  sqlite3_int64 id = sqlite3_last_insert_rowid(db);
  sqlite3_stmt* stmt_get = NULL;
  std::string sql_getall = "SELECT ID, URL, POSITION FROM URLS WHERE rowid=?";
  if (sqlite3_prepare_v2(db, sql_getall.c_str(), -1, &stmt_get, nullptr) != SQLITE_OK || stmt_get == NULL){
    return nullptr;
  }
  sqlite3_bind_int(stmt_get, 1, id);
  
  if ((rc = sqlite3_step(stmt_get)) != SQLITE_ROW){
    return nullptr;
  }
  
  std::shared_ptr<URLData> url (new URLData());
  url->id = sqlite3_column_int(stmt_get, 0);
  url->m_url = (const char*)sqlite3_column_text(stmt_get, 1);
  sqlite3_finalize(stmt_get);
  m_urlList->push_back(url);
  return url;
}

bool DataManager::updateURL(uint32_t p_id, std::string p_url){
  sqlite3* db = getDatabaseConnection();
  
  char* err;
  sqlite3_stmt* stmt = NULL;
  std::string sql_update = "UPDATE URLS SET URL=? WHERE ID=?";
  if (sqlite3_prepare_v2(db, sql_update.c_str(), -1, &stmt, NULL) != SQLITE_OK || stmt == NULL){
    return false;
  }
  
  sqlite3_bind_text(stmt, 1, p_url.c_str(), p_url.length(), SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, p_id);
  int rc = sqlite3_step(stmt);
  
  if (rc != SQLITE_DONE){
    std::cout << "Could update. -  " << err;
    return false;
  }
  sqlite3_finalize(stmt);
  updateURLDataById(p_id, p_url);
  return true;
}

bool DataManager::deleteURL(uint32_t p_id){

  sqlite3* db = getDatabaseConnection();
  
  char* err;
  sqlite3_stmt* stmt = NULL;
  std::string sql_delete = "DELETE FROM URLS WHERE ID=?";
  if (sqlite3_prepare_v2(db, sql_delete.c_str(), -1, &stmt, NULL) != SQLITE_OK || stmt == NULL){
    return false;
  }
  
  sqlite3_bind_int(stmt, 1, p_id);
  int rc = sqlite3_step(stmt);
  
  if (rc != SQLITE_DONE){
    std::cout << "Could delete. - " << err;
    return false;
  }
  sqlite3_finalize(stmt);
  deleteURLDataById(p_id);
  return true;
}

std::shared_ptr<URLData> DataManager::findURLDataById(uint32_t id) {
  for (std::shared_ptr<URLData> url: *m_urlList) {
    if (url->id == id){
      return url;
    }
  }
  
  return nullptr;
}

bool DataManager::updateURLDataById(uint32_t id, std::string p_url) {

  int i = 0;
  bool found = false;
  for (int i=0; i < m_urlList->size(); i++){
    std::shared_ptr<URLData> url = m_urlList->at(i);
    if (url->id == id){
      url->m_url = p_url;
      found = true;
      break;
    }
  }  
  
  return found;
}

bool DataManager::deleteURLDataById(uint32_t id) {

  int i = 0;
  bool found = false;
  for (int i=0; i < m_urlList->size(); i++){
    std::shared_ptr<URLData> url = m_urlList->at(i);
    if (url->id == id){
      found = true;
      break;
    }
  }  
  
  if (found){
    m_urlList->erase(m_urlList->begin() + i);
  }
  
  return found;
}

std::vector<std::shared_ptr<URLData>>* DataManager::retriveURLs(){
  sqlite3* db = getDatabaseConnection();
  sqlite3_stmt* stmt = NULL;
  std::string sql_getall = "SELECT ID, URL, POSITION FROM URLS";
  int rc = SQLITE_ERROR;
  
  if (m_urlList == nullptr){
    m_urlList = std::make_unique<std::vector<std::shared_ptr<URLData>>>();
  }
  m_urlList->clear();
  
  if (sqlite3_prepare_v2(db, sql_getall.c_str(), sql_getall.length(), &stmt, NULL) != SQLITE_OK){
    return nullptr;
  }
  
  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    //std::cout << sqlite3_column_text(stmt, 1) << "\n";
    std::shared_ptr<URLData> url (new URLData());
    url->id = sqlite3_column_int(stmt, 0);
    url->m_url = (const char*)sqlite3_column_text(stmt, 1);
    m_urlList->push_back(url);
  }
  if (rc != SQLITE_DONE){
    std::cout << "Could not retrieve data ";
    return nullptr;
  }
  //std::cout << "Retreived results\n";
  return m_urlList.get();
}

