#include "utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <filesystem>

std::string Utils::getHomeDirectory()
{
    std::string home_dir_str("");
    char* home_dir = getenv("HOME");
    if (home_dir == NULL) {
        long buf_size = sysconf(_SC_GETPW_R_SIZE_MAX);
        char* buf;
        struct passwd pw;
        struct passwd* result;
        if (buf_size == -1){
            buf_size = 20000;
        }
        buf = (char*)malloc(buf_size);
        if (!buf){
            return std::string();
        }
        int rc = getpwuid_r(getuid(), &pw, buf, buf_size, &result);
        if (result == NULL){
            return std::string();
        }
        
        home_dir_str.append(pw.pw_dir);
        free(buf);
    } else {
        home_dir_str.append(home_dir);
    }
    
    return home_dir_str;
}

std::string Utils::getDataDirectory()
{
    std::string home_dir = Utils::getHomeDirectory();
    if (home_dir.empty()){
        return std::string();
    }

    std::filesystem::path home_dir_path(home_dir);
    home_dir_path.append(".local/share/is-online");
    return home_dir_path.u8string();
}


