#ifndef __SINGLE_PLAY_SCENE_H__
#define __SINGLE_PLAY_SCENE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "ui/CocosGUI.h"
#include <atomic>

using namespace cocos2d;
using namespace cocos2d::network;
using namespace ui;

template <typename Ty>
std::string to_string2(Ty v) {
  Ty a = v;
  std::stringstream ss;
  ss << a;
  return  ss.str();
}

struct stage_info;

struct spots_info {
  std::vector<Rect> left_rects;
  std::vector<Rect> right_rects;
  std::vector<bool> answer_container;
  spots_info() {
    left_rects.clear();
    right_rects.clear();
    answer_container.clear();
  }
};

class single_play_scene : public cocos2d::Layer
{
public:
    enum SINGLE_PLAY_STATUS { LOADING, PAUSE, PLAYING, RESULT };

    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void update(float delta_time);
    
    void http_request(std::string req_url, std::string tag);
    void on_http_request_completed(HttpClient *sender, HttpResponse *response);

    bool parsing_stage_info(std::string&& payload);

    virtual bool onTouchBegan(Touch* touch, Event* unused_event);
    virtual void onTouchMoved(Touch* touch, Event* unused_event);
    virtual void onTouchCancelled(Touch* touch, Event* unused_event);
    virtual void onTouchEnded(Touch* touch, Event* unused_event);

    void start_game();
    void end_game(cocos2d::Ref* pSender);

    void on_start_game();

    void pause_game();
    void resume_game();

    void retry_game(cocos2d::Ref* pSender);
    void view_ranking(cocos2d::Ref* pSender);


    Sprite* curtain_left_img_;
    Sprite* curtain_right_img_;

    Sprite* left_img;
    Sprite* right_img;

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(single_play_scene);

    void generate_rects();
    bool check_find_answer(const Point& point);

    void correct_effect(int index);
    void incorrect_effect(Point point);
    void done_incorrect_effect();

    void create_pause();
    void create_timer();
    void on_update_timer();

    void draw_stage_info(int current_stage=1, int end_stage=102);
    void update_spot_info(int total_spot_count=0);
    void draw_spot_info(int found_spot_count=0, int total_spot_count=5);
 
    void on_complete_stage();
    
    void on_load_item_store();
  
    void open_pause_menu();
    void close_pause_menu(cocos2d::Ref* pSender);

    void on_allowing_input();

    void game_over();

 private:
    void start_circle_animation(Vec2 pos);
    void on_unlock_pause_button();

    void on_create_end_navigation_menu();

    Texture2D left_texture;
    Texture2D right_texture;
    Vec2 origin_;
    Vec2 center_;

    //cocos2d::Vector<cocos2d::Rect*> mpData;
    //Vector<Rect*> spot_container;
    Label* label_;

    std::atomic<int> download_count_;
    std::shared_ptr<stage_info> stage_info_;
    std::shared_ptr<spots_info> spots_info_;

    bool enable_input_;

    SINGLE_PLAY_STATUS single_play_status_;

    ProgressTimer* progress_timebar_;
    Sprite* time_bar;

    Label* spot_info_font_ = nullptr;

    Button* pause_button = nullptr;
    Button* resume_button = nullptr;

    Vec2 search_position_;

    bool is_pause_button_;

    Menu* paused_navigation_menu_;

    bool is_allowing_input_ = true;

    //decltype(MenuItemFont) item_1;
};

#endif 

// auto touch_listener = EventListenerTouchOneByOne::create();
// _eventDispatcher->removeEventListener(listener);
// _eventDispatcher->removeAllEventListeners();

