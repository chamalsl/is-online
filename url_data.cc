#include "url_data.h"

URLData::URLData(){
}

URLData::URLData(std::string p_url, unsigned int p_order){
    m_url = p_url;
    m_order = p_order;
}

URLData::~URLData(){
}