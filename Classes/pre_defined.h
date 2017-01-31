#ifndef __PRE_DEFINED_H__
#define __PRE_DEFINED_H__

static const int iphone6_width  = 1920;
static const int iphone6_height = 1080;

static const int offset_x = 2;
static const int offset_y = 50;

static const int image_size_x = (1920 / 2) - offset_x; // 958
static const int image_size_y = 1000 - 20;             // 980

static const char* req_stage_info_url = "http://127.0.0.1:3000/stage_info/";
//static const char* req_stage_info_url = "http://192.168.25.18:3000/stage_info/";

// img width  = 958;
// img height = 1080;

struct spot {
  spot(int _x, int _y) : x(_x), y(_y) {}
  int x;
  int y;
};

struct spot_rect {
  spot_rect(int _x, int _y) : x(_x), y(_y) {}
  int x;
  int y;
};

struct stage_info {
  std::vector<spot> spots;
  std::vector<spot_rect> spot_rects;
  std::string left_img;
  std::string right_img;
  int current_stage_count;
  int total_stage_count;
  int spot_count;
  float play_time;
};

#endif
