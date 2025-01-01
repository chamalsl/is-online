#ifndef URL_DATA_H
#define URL_DATA_H

#include <string>
class URLData{

  public:
    
    URLData();
    URLData(std::string p_url, unsigned int p_order);
    virtual ~URLData();

    
    unsigned int id;
    std::string m_url;
    unsigned int m_order;
    unsigned int m_status;

};

#endif //URL_DATA_H
