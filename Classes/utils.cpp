#include "utils.h"

void save_user_info(std::string key, bool value) {
  UserDefault::getInstance()->setBoolForKey(key.c_str(), value);
  UserDefault::getInstance()->flush();
}

void save_user_info(std::string key, int value) {
  UserDefault::getInstance()->setIntegerForKey(key.c_str(), value);
  UserDefault::getInstance()->flush();
}

void save_user_info(std::string key, float value) {
  UserDefault::getInstance()->setFloatForKey(key.c_str(), value);
  UserDefault::getInstance()->flush();
}

void save_user_info(std::string key, double value) {
  UserDefault::getInstance()->setDoubleForKey(key.c_str(), value);
  UserDefault::getInstance()->flush();
}

void save_user_info(std::string key, std::string value) {
  UserDefault::getInstance()->setStringForKey(key.c_str(), value.c_str());
  UserDefault::getInstance()->flush();
}

void save_user_info(std::string key, const char* value) {
  UserDefault::getInstance()->setStringForKey(key.c_str(), value);
  UserDefault::getInstance()->flush();
}


template<typename T>
T get_user_info(const std::string& key) {
  //return T;
}

template<>
bool get_user_info<bool>(const std::string& key) {
  return UserDefault::getInstance()->getBoolForKey(key.c_str());
}

template<>
int get_user_info<int>(const std::string& key) {
  return UserDefault::getInstance()->getIntegerForKey(key.c_str(), 0);
}

template<>
float get_user_info<float>(const std::string& key) {
  return UserDefault::getInstance()->getFloatForKey(key.c_str());
}

template<>
double get_user_info<double>(const std::string& key) {
  return UserDefault::getInstance()->getDoubleForKey(key.c_str());
}

template<>
std::string get_user_info<std::string>(const std::string& key) {
  return UserDefault::getInstance()->getStringForKey(key.c_str());
}

//auto user_default_save_path = UserDefault::getInstance()->getXMLFilePath();
//CCLOG("%s", user_default_save_path.c_str());
