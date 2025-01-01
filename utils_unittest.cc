#include "utils.h"
#include <gtest/gtest.h>
#include <unistd.h>
#include <string>

TEST(Utils, getHomeDirectory){
  std::string home_dir = Utils::getHomeDirectory();
  char* user = getlogin();
  std::string expected_path("/home/");
  expected_path.append(user);
  ASSERT_EQ(expected_path, home_dir);
}

TEST(Utils, getHomeDirectory_getpwuid){
  char* home_env= getenv("HOME");
  unsetenv("HOME");
  std::string home_dir = Utils::getHomeDirectory();
  char* user = getlogin();
  std::string expected_path("/home/");
  expected_path.append(user);
  setenv("HOME",home_env,0);
  ASSERT_EQ(expected_path, home_dir);
}

TEST(Utils, getDataDirectory){
  std::string actual = Utils::getDataDirectory();
  char* user = getlogin();
  std::string expected_path("/home/");
  expected_path.append(user);
  expected_path.append("/.local/share/is-online");
  ASSERT_EQ(expected_path, actual);
}