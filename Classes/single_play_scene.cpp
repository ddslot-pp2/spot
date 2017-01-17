#include "single_play_scene.h"
#include "SimpleAudioEngine.h"
#include "pre_defined.h"
#include "utils.h"
#include "json11.h"

using namespace CocosDenshion;

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

  // input control
  enable_input_ = true;

  //save_user_info("string_test", std::string("testing"));
  save_user_info("current_stage", 1);

  auto current_stage = get_user_info<int>("current_stage");
  CCLOG("string test: %d \n", current_stage);

  download_count_ = 0;

  // http request for gathering stage information
  CCLOG("1\n");
  
  auto req_url = std::string(req_stage_info_url) + "0";
  http_request(req_url.c_str(), "stage_info");
  CCLOG("3\n");  
    
  auto visibleSize = Director::getInstance()->getVisibleSize();
  origin_ = Director::getInstance()->getVisibleOrigin();

  center_ = Vec2(origin_.x + visibleSize.width/2,
		 origin_.y + visibleSize.height/2);

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
  auto menu = Menu::create(closeItem, nullptr);
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


  // load ui
  create_timer();

  // bg  
  /*
  auto bg = Sprite::create("ui/bbg.jpg");
  bg->setPosition(Vec2(center_.x, center_.y + 500));
  this->addChild(bg, 2);
  */

  //draw_stage_info();

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
      CCLOG("stage_count: %d", stage_info_->current_stage_count);
      CCLOG("stage_count: %d", stage_info_->total_stage_count);
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

  if(!enable_input_) return true;

  Point location = touch->getLocation();

  CCLOG("touched position %f, %f\n", location.x, location.y);


  if(single_play_status_ != SINGLE_PLAY_STATUS::PLAYING) return true;
  auto r = check_find_answer(location);
  if(!r) {
    incorrect_effect(location);
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
    stage_info_->current_stage_count = res["current_stage_count"].int_value();
    stage_info_->total_stage_count = res["total_stage_count"].int_value();
    stage_info_->play_time = static_cast<float>(res["play_time"].int_value());

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

    draw_stage_info(stage_info_->current_stage_count, stage_info_->total_stage_count);
    update_spot_info(spots.size());

    http_request(stage_info_->left_img, "left_img");
    http_request(stage_info_->right_img, "right_img");

    generate_rects();

    return true;
}

void single_play_scene::start_game() {
  single_play_status_ = SINGLE_PLAY_STATUS::PLAYING;
  curtain_left_img_->runAction(Sequence::create(Show::create(), FadeOut::create(2.0), nullptr));
  curtain_right_img_->runAction(Sequence::create(Show::create(), FadeOut::create(2.0), nullptr));

  // 커튼같은거 치우고나서부터 게임이 시작
  this->scheduleOnce(SEL_SCHEDULE(&single_play_scene::on_start_game), 1.0f);

  auto ready = Sprite::create("ui/ready2.png");
  ready->setPosition(Vec2(center_.x, center_.y + 50));
  this->addChild(ready, 2);
  ready->runAction(Sequence::create(Show::create(), FadeOut::create(1.0), nullptr));
  auto audio = SimpleAudioEngine::getInstance();
  audio->playEffect("sound/ready.wav");

  auto go = Sprite::create("ui/go.png");
  go->setPosition(Vec2(center_.x, center_.y + 50));
  this->addChild(go, 2);
  go->setOpacity(0);
  go->runAction(Sequence::create(Show::create(), DelayTime::create(1.0), FadeIn::create(1.0), FadeOut::create(1.0), nullptr));
  this->runAction(
		  Sequence::create(
				   DelayTime::create(1.0),
				   CallFunc::create([]() {
				       auto audio = SimpleAudioEngine::getInstance();
				       audio->playEffect("sound/go.wav");
				     }),
				   nullptr
				   )
		  );
}

void single_play_scene::on_start_game() {
  this->schedule(SEL_SCHEDULE(&single_play_scene::on_update_timer), 1/10);
}

void single_play_scene::pause_game() {
  single_play_status_ = SINGLE_PLAY_STATUS::PAUSE;
  curtain_left_img_->runAction(Sequence::create(Show::create(), FadeIn::create(1.0), nullptr));
  curtain_right_img_->runAction(Sequence::create(Show::create(), FadeIn::create(1.0), nullptr));
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

  for(size_t i=0; i<spots_info_->answer_container.size(); ++i) {

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
  update_spot_info(spots_info_->answer_container.size());
}

void single_play_scene::incorrect_effect(Point point) {
  enable_input_ = false;
  CCLOG("XX incorrect answer XX");
  this->scheduleOnce(SEL_SCHEDULE(&single_play_scene::done_incorrect_effect), 0.8f);
  
  auto audio = SimpleAudioEngine::getInstance();
  audio->playEffect("sound/incorrect.wav");

 auto progress = progress_timebar_->getPercentage();
 progress_timebar_->setPercentage(progress - 10);

  auto incorrect = Sprite::create("ui/incorrect.png");
  incorrect->setScale(0.5f);
  incorrect->setPosition(Vec2(point.x, point.y));
  this->addChild(incorrect);
  auto fadeOut = FadeOut::create(0.8f);
  incorrect->runAction(fadeOut);
  
  auto moveBy0  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy1  = MoveBy::create(0.05f, Vec2(20, 0));
  auto moveBy2  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy3  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy4  = MoveBy::create(0.05f, Vec2(20, 0));
  auto moveBy5  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy6  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy7  = MoveBy::create(0.05f, Vec2(20, 0));
  auto moveBy8  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy9  = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy10 = MoveBy::create(0.05f, Vec2(20, 0));
  auto moveBy11 = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy12 = MoveBy::create(0.05f, Vec2(-10, 0));
  auto moveBy13 = MoveBy::create(0.05f, Vec2(20, 0));
  auto moveBy14 = MoveBy::create(0.05f, Vec2(-10, 0));

  auto seq = Sequence::create(moveBy0, moveBy1, moveBy2, moveBy3, moveBy4, moveBy5, moveBy6, moveBy7, moveBy8, moveBy9, moveBy10, moveBy11, moveBy12, moveBy13, moveBy14, nullptr);

  if(point.x <  Director::getInstance()->getVisibleSize().width / 2.0f) {
    left_img->runAction(seq);
  } else {
    right_img->runAction(seq);
  }
}

void single_play_scene::done_incorrect_effect() {
  enable_input_ = true; 
}

void single_play_scene::create_timer() {
   time_bar = Sprite::create("ui/timebar2.png");

  // 10초 동안 게이지 100% 동안 내려옴
   auto timeOutline = Sprite::create("ui/timeoutline2.png");

   auto timer_position = Vec2(time_bar->getContentSize().width/2 + 55, 
			     (center_.y + iphone6_height / 2) - timeOutline->getContentSize().height * 0.5f);
   
   timer_position.y = timer_position.y - 2.0f;
 
  timeOutline->setPosition(timer_position);

  //timeOutline->setScaleX(0.65f);
  //  timeOutline->setScaleY(0.7f);
  timeOutline->setVisible(true);
  this->addChild(timeOutline, 0);

  progress_timebar_ = ProgressTimer::create(time_bar);

  progress_timebar_->setPosition(timer_position);
  //progressTimeBar_->setScaleX(0.65f);
  //progressTimeBar_->setScaleY(0.7f);
  progress_timebar_->setMidpoint(Point(0.0f, 1.0f));
  progress_timebar_->setBarChangeRate(Point(1, 0));
  progress_timebar_->setType(ProgressTimer::Type::BAR);
  progress_timebar_->setPercentage(100);
  this->addChild(progress_timebar_, 0);
}

void single_play_scene::on_update_timer() {
  if(single_play_status_ == PAUSE) return;
  // 1분
  float timer_sec = stage_info_->play_time;
  float percentage = progress_timebar_->getPercentage();
  progress_timebar_->setPercentage(percentage - (100 / (60 * timer_sec)));
}

void single_play_scene::draw_stage_info(int current_stage, int end_stage) {
  auto stage_info_font = std::to_string(current_stage) + "/" + std::to_string(end_stage);
  auto label = Label::createWithSystemFont(stage_info_font.c_str(), "Ariel", 50);
  label->setColor(Color3B(255, 0, 0)); 
  //label->setWidth(400);
  label->setPosition(Vec2(center_.x, center_.y + 500));
  this->addChild(label, 2);
}

void single_play_scene::update_spot_info(int total_spot_count) {
  auto answer_count = 0;
  if(spots_info_) {
    for(const auto& answer : spots_info_->answer_container) {
      if(answer) {
	answer_count++;
      }
    }
  }

  if (answer_count == total_spot_count) {
    // end of stage callback with timer
    this->scheduleOnce(SEL_SCHEDULE(&single_play_scene::on_complete_stage), 1.0f);
  }

  draw_spot_info(answer_count, total_spot_count);
}

void single_play_scene::draw_spot_info(int found_spot_count, int total_spot_count) {
  auto spot_info_font = std::to_string(found_spot_count) + "/" + std::to_string(total_spot_count);
  
  if(!spot_info_font_) {
    spot_info_font_ = Label::createWithSystemFont(spot_info_font.c_str(), "Ariel", 50);
    spot_info_font_->setPosition(Vec2(center_.x + 400, center_.y + 500));
    this->addChild(spot_info_font_, 2);
    return;
  }

  //spot_info_font->setString(ccsf2("x %d", user_info::get().item_info_.get_hint_count()));
  spot_info_font_->setString(spot_info_font.c_str());
}

void single_play_scene::on_complete_stage() {
  CCLOG("on_complete_stage called\n");
}
