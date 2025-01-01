#include "url_data.h"
#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>
#include <cstdint>


class DataManager{

  public:
  
    DataManager();
    virtual ~DataManager();
    bool ensureDatabase();
    std::shared_ptr<URLData> addURL(std::string p_url);
    bool updateURL(uint32_t p_id, std::string p_url);
    bool deleteURL(uint32_t p_id);
    std::vector<std::shared_ptr<URLData>>* retriveURLs();
    std::shared_ptr<URLData> findURLDataById(uint32_t id);
    sqlite3* getDatabaseConnection();
    std::unique_ptr<std::vector<std::shared_ptr<URLData>>> m_urlList = nullptr;
  
  private:
    bool updateURLDataById(uint32_t id, std::string p_url);
    bool deleteURLDataById(uint32_t id);
    bool createTable();
    std::unique_ptr<std::string> database_path;
    sqlite3* m_db = NULL;
  
  
};
