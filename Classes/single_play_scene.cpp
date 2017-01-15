#include "single_play_scene.h"
#include "SimpleAudioEngine.h"
#include "pre_defined.h"
#include "utils.h"
#include "json11.h"

Scene* single_play_scene::createScene()
{
  // 'scene' is an autorelease object
  auto scene = Scene::create();
    
  // 'layer' is an autorelease object
  auto layer = single_play_scene::create();

  // add layer as a child to scene
  scene->addChild(layer);

  // return the scene
  return scene;
}

// on "init" you need to initialize your instance
bool single_play_scene::init()
{
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
      return false;
  }

  // 스테이지 인포 가져오기
  single_play_status_ = SINGLE_PLAY_STATUS::LOADING;

  //save_user_info("string_test", std::string("testing"));
  save_user_info("current_stage", 1);

  auto current_stage = get_user_info<int>("current_stage");
  CCLOG("string test: %d \n", current_stage);

  download_count_ = 0;

  // http request for gathering stage information
  CCLOG("1\n");
  http_request("http://127.0.0.1:3000/stage_info/0", "stage_info");
  CCLOG("3\n");  
    
  auto visibleSize = Director::getInstance()->getVisibleSize();
  origin_ = Director::getInstance()->getVisibleOrigin();

  /////////////////////////////
  // 2. add a menu item with "X" image, which is clicked to quit the program
  //    you may modify it.

  // add a "close" icon to exit the progress. it's an autorelease object
  auto closeItem = MenuItemImage::create(
					 "CloseNormal.png",
					 "CloseSelected.png",
					 CC_CALLBACK_1(single_play_scene::menuCloseCallback, this));
    
  closeItem->setPosition(Vec2(origin_.x + visibleSize.width - closeItem->getContentSize().width/2 ,
			      origin_.y + closeItem->getContentSize().height/2));

  // create menu, it's an autorelease object
  auto menu = Menu::create(closeItem, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  /////////////////////////////
  // 3. add your codes below...

  // add a label shows "Hello World"
  // create and initialize a label  
  label_ = Label::createWithTTF("Waitting For Images", "fonts/Marker Felt.ttf", 24);
  label_->setPosition(Vec2(origin_.x + visibleSize.width/2,
			  origin_.y + visibleSize.height - label_->getContentSize().height));
  this->addChild(label_, 1);

  // bg
  /*
  auto bg = Sprite::create("res/bbbgg.jpg");
  bg->setPosition(Vec2(visibleSize.width/2 + origin_.x, visibleSize.height/2 + origin_.y + (iphone6_height/2) - offset_y));
  this->addChild(bg, 1);
  */

  /*
  CCLOG("visible width: %f, height: %f\n", visibleSize.width, visibleSize.height);

  http_request("https://images.pristineauction.com/50/501420/main_7-Hank-Greenberg-Signed-Charles-Fazzino-Custom-Hand-Painted-3D-Pop-Art-Baseball-with-Swarovski-Crystals-JSA-LOA-PristineAuction.com.jpg", "left_img");
  http_request("https://images.pristineauction.com/50/501420/main_7-Hank-Greenberg-Signed-Charles-Fazzino-Custom-Hand-Painted-3D-Pop-Art-Baseball-with-Swarovski-Crystals-JSA-LOA-PristineAuction.com.jpg", "right_img");
    */


  curtain_left_img_ = Sprite::create("res/curtain.png");
  curtain_left_img_->setPosition(Vec2((visibleSize.width/2)/2 + origin_.x - offset_x, (visibleSize.height/2 + origin_.y) - offset_y));
  this->addChild(curtain_left_img_, 2);

  curtain_right_img_ = Sprite::create("res/curtain.png");
  curtain_right_img_->setPosition(Vec2( (visibleSize.width/2)+(visibleSize.width/2/2) + origin_.x + offset_x, (visibleSize.height/2 + origin_.y)  - offset_y));
  this->addChild(curtain_right_img_, 2);
  
  // handle input
  auto touch_listener = EventListenerTouchOneByOne::create();
  touch_listener->setSwallowTouches(true);
 
  touch_listener->onTouchBegan = CC_CALLBACK_2(single_play_scene::onTouchBegan, this);
  touch_listener->onTouchMoved = CC_CALLBACK_2(single_play_scene::onTouchMoved, this);
  touch_listener->onTouchCancelled = CC_CALLBACK_2(single_play_scene::onTouchCancelled, this);
  touch_listener->onTouchEnded = CC_CALLBACK_2(single_play_scene::onTouchEnded, this);

  EventDispatcher* _event_dispatcher = Director::getInstance()->getEventDispatcher();
  _event_dispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

  //  this->scheduleUpdate();

  return true;
}

void single_play_scene::menuCloseCallback(Ref* pSender) {
  //Close the cocos2d-x game scene and quit the application
  Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
    
  /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
  //EventCustom customEndEvent("game_scene_close_event");
  //_eventDispatcher->dispatchEvent(&customEndEvent);
}

void single_play_scene::http_request(std::string url, std::string tag) {
  
  auto request = new HttpRequest();

  CCLOG("request url: %s", url.c_str());
  request->setUrl(url.c_str());

  request->setRequestType(HttpRequest::Type::GET);
  request->setResponseCallback( CC_CALLBACK_2(single_play_scene::on_http_request_completed, this) );
  request->setTag(tag.c_str());

  cocos2d::network::HttpClient::getInstance()->send(request);
  request->release();

}

void single_play_scene::on_http_request_completed(HttpClient *sender, HttpResponse *response) {

  if(!response) {
    CCLOG("response is not true\n");  
    return;
  }

  auto response_code = response->getResponseCode();
  
  if(!response->isSucceed()) {
    //std::cout << "error code: " << response_code << std::endl;
    CCLOG("response is not success, response code: %ld\n", response_code);
    return;
  }

  if(response_code != 200) {
    CCLOG("response code is not 200\n");
    return;
  }

  if (0 != strlen(response->getHttpRequest()->getTag())) {
      CCLOG("%s completed", response->getHttpRequest()->getTag());
  }

  CCLOG("http request complete");

  auto visibleSize = Director::getInstance()->getVisibleSize();

  std::vector<char>* buffer = response->getResponseData();


  if(response->getHttpRequest()->getTag() == std::string("left_img") || 
     response->getHttpRequest()->getTag() == std::string("right_img")) {
    Image* image = new Image();
    image->initWithImageData ( reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
  
    if(response->getHttpRequest()->getTag() == std::string("left_img")) {
      left_texture.initWithImage(image);
 
      left_img = Sprite::createWithTexture(&left_texture);
      left_img->setPosition(Vec2((visibleSize.width/2)/2 + origin_.x - offset_x, (visibleSize.height/2 + origin_.y) - offset_y));

      this->addChild(left_img, 0);
    } else {
      right_texture.initWithImage(image);
 
      right_img = Sprite::createWithTexture(&right_texture);

      right_img->setPosition(Vec2( (visibleSize.width/2)+(visibleSize.width/2/2) + origin_.x + offset_x, (visibleSize.height/2 + origin_.y)  - offset_y));

      this->addChild(right_img, 0);
    }

    if (image) {
      delete image;
    }

    ++download_count_;
    if (download_count_ >= 2) {
      label_->setString("Loading Textures Done");
      start_game();
    }
  }
  else if (response->getHttpRequest()->getTag() == std::string("stage_info")) {
    CCLOG("2\n");
    CCLOG("http request complete for stage_info");
    char * concatenated = (char *) malloc(buffer->size() + 1);
    std::string _data(buffer->begin(), buffer->end());
    strcpy(concatenated, _data.c_str());

    if(parsing_stage_info(std::move(_data))) {
      CCLOG("stage_count: %d", stage_info_->stage_count);
      CCLOG("spot size: %d", stage_info_->spots.size());
      CCLOG("rects size: %d", stage_info_->spot_rects.size());
      CCLOG("left img: %s", stage_info_->left_img.c_str());
      CCLOG("right img: %s", stage_info_->right_img.c_str());
    }
   
  } else {
    CCLOG("http request complete but no tag");    
  }

  //http://legacy.tistory.com/88
}

bool single_play_scene::onTouchBegan(Touch* touch, Event* unused_event) {

  Point location = touch->getLocation();

  CCLOG("touched position %f, %f\n", location.x, location.y);


  if(single_play_status_ != SINGLE_PLAY_STATUS::PLAYING) return true;
  auto r = check_find_answer(location);
  if(!r) {
    incorrect_effect();
  }

  return true;
}
 
void single_play_scene::onTouchMoved(Touch* touch, Event* unused_event) {}
 
void single_play_scene::onTouchCancelled(Touch* touch, Event* unused_event) {}
 
void single_play_scene::onTouchEnded(Touch* touch, Event* unused_event) {}

void single_play_scene::update(float delta_time)
{
  CCLOG("aaa");
}

bool single_play_scene::parsing_stage_info(std::string&& payload) {
    using namespace json11;
    std::string err;
    auto res = Json::parse(payload, err);

    if(!err.empty()) {
      CCLOG("fail to parise json");
      return false;
    }

    stage_info_ = std::make_shared<stage_info>();
    stage_info_->stage_count = res["stage_count"].int_value();

    auto spots = res["spots"].array_items();    
    for(auto& d : spots) {
      auto x = d["x"].int_value();
      auto y = d["y"].int_value();
      stage_info_->spots.push_back(spot(x, y));
      //spots_.push_back(Rect(x,y))
    }

    auto rects = res["spot_rects"].array_items();    
    for(auto& d : rects) {
      auto x = d["x"].int_value();
      auto y = d["y"].int_value();
      stage_info_->spot_rects.push_back(spot_rect(x, y));
    }

    stage_info_->left_img = res["left_img"].string_value().c_str(); 
    stage_info_->right_img = res["right_img"].string_value().c_str(); 

    http_request(stage_info_->left_img, "left_img");
    http_request(stage_info_->right_img, "right_img");

    generate_rects();

    return true;
}

void single_play_scene::start_game() {
  single_play_status_ = SINGLE_PLAY_STATUS::PLAYING;
  curtain_left_img_->runAction(Sequence::create(Show::create(), FadeOut::create(2.0), NULL));
  curtain_right_img_->runAction(Sequence::create(Show::create(), FadeOut::create(2.0), NULL));
}

void single_play_scene::pause_game() {
  single_play_status_ = SINGLE_PLAY_STATUS::PAUSE;
  curtain_left_img_->runAction(Sequence::create(Show::create(), FadeIn::create(1.0), NULL));
  curtain_right_img_->runAction(Sequence::create(Show::create(), FadeIn::create(1.0), NULL));
}

void single_play_scene::generate_rects() {

  spots_info_ = std::make_shared<spots_info>();

  const float added_right_x = offset_x + (iphone6_width / 2);

  for(size_t i=0; i<stage_info_->spots.size(); ++i) {

    // left image rects
    auto _x = static_cast<float>(stage_info_->spots[i].x - (stage_info_->spot_rects[i].x * 0.5f));
    auto _y = static_cast<float>(stage_info_->spots[i].y - stage_info_->spot_rects[i].y * 0.5f);

    auto _width = static_cast<float>(stage_info_->spot_rects[i].x);
    auto _height = static_cast<float>(stage_info_->spot_rects[i].y);
    Rect lr(_x, _y, _width, _height);
    spots_info_->left_rects.push_back(lr);

    // right image rects
    _x = static_cast<float>(stage_info_->spots[i].x + added_right_x - stage_info_->spot_rects[i].x * 0.5f);

    _width = static_cast<float>(stage_info_->spot_rects[i].x);
    Rect rr(_x, _y, _width, _height);
    spots_info_->right_rects.push_back(rr);

    spots_info_->answer_container.push_back(false);
  }
}

bool single_play_scene::check_find_answer(const Point& point) {

  if(single_play_status_ != SINGLE_PLAY_STATUS::PLAYING) return false;

  auto is_find = false;

  for(auto i=0; i<spots_info_->answer_container.size(); ++i) {
    if(spots_info_->answer_container[i])
      continue;

    auto& left_rect = spots_info_->left_rects[i];
    auto& right_rect = spots_info_->right_rects[i];

    if (left_rect.containsPoint(point) || right_rect.containsPoint(point)) {      
      correct_effect(i);
      return true;
    }
  }

  return is_find;
}

void single_play_scene::correct_effect(int index) {
  CCLOG("@@ correct answer @@");
  spots_info_->answer_container[index] = true;
}

void single_play_scene::incorrect_effect() {
  CCLOG("XX incorrect answer XX");
}

//http://stackoverflow.com/questions/38887808/how-to-add-child-nested-element-inside-json-using-json11-library
