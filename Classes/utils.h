#ifndef __UTILS_H__
#define __UTILS_H__

#include "cocos2d.h"
#include <string>

using namespace cocos2d;

/*
void add_s2(cocos2d::Layer* scene) 
{
  auto visibleSize = Director::getInstance()->getVisibleSize();
  auto origin = Director::getInstance()->getVisibleOrigin();

  auto sprite = Sprite::create("HelloWorld.png");
  sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
  scene->addChild(sprite, 0);
}
*/

void save_user_info(std::string key, bool value);
void save_user_info(std::string key, int value);
void save_user_info(std::string key, float value);
void save_user_info(std::string key, double value);
void save_user_info(std::string key, std::string value);
void save_user_info(std::string key, const char* value);

template<typename T>
T get_user_info(const std::string& key);

template<>
bool get_user_info<bool>(const std::string& key);

template<>
int get_user_info<int>(const std::string& key);

template<>
float get_user_info<float>(const std::string& key);

template<>
double get_user_info<double>(const std::string& key);

template<>
std::string get_user_info<std::string>(const std::string& key);

#endif

