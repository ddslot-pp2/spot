#include "AppDelegate.h"
#include "single_play_scene.h"
#include "pre_defined.h"

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(iphone6_width, iphone6_height);
static cocos2d::Size deviceResolutionSize = cocos2d::Size(iphone6_width, iphone6_height);

static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
      glview = GLViewImpl::createWithRect("spot_ranking", Rect(0, 0, 1920/2, 1080/2));
      //glview = GLViewImpl::createWithRect("spot_ranking", Rect(0, 0, ipad_mini4_width/2, ipad_mini4_height/2));

#else
        glview = GLViewImpl::create("spot_ranking");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    //glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);

    director->setContentScaleFactor(MIN(deviceResolutionSize.height/designResolutionSize.height, deviceResolutionSize.width/designResolutionSize.width));

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = single_play_scene::createScene();
    //auto scene = menu_scene::createScene();

    // run
    director->runWithScene(scene);

    cocos2d::Device::setKeepScreenOn(true);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be paused
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

//CCUserDefault 저장할때 사용

