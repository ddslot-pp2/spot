#ifndef __PRE_DEFINED_H__
#define __PRE_DEFINED_H__

static const int iphone6_width  = 1920;
static const int iphone6_height = 1080;

static const int offset_x = 2;
static const int offset_y = 40;

static const char* req_stage_info_url = "http://127.0.0.1/stage_info/";

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
  int stage_count;
  int spot_count;
};


#endif
