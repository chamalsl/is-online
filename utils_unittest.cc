#include "utils.h"
#include <gtest/gtest.h>
#include <unistd.h>
#include <string>

TEST(Utils, getHomeDirectory){
  char* home_dir = Utils::getHomeDirectory();
  char* user = getlogin();
  std::string expected_path("/home/");
  expected_path.append(user);
  std::string actual(home_dir);
  ASSERT_EQ(expected_path, actual);
}

TEST(Utils, getHomeDirectory_getpwuid){
  char* home_env= getenv("HOME");
  unsetenv("HOME");
  char* home_dir = Utils::getHomeDirectory();
  char* user = getlogin();
  std::string expected_path("/home/");
  expected_path.append(user);
  std::string actual(home_dir);
  setenv("HOME",home_env,0);
  ASSERT_EQ(expected_path, actual);
}

TEST(Utils, getDataDirectory){
  std::string actual = Utils::getDataDirectory();
  char* user = getlogin();
  std::string expected_path("/home/");
  expected_path.append(user);
  expected_path.append("/.local/share/is-online");
  ASSERT_EQ(expected_path, actual);
}