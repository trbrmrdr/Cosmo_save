#pragma once

#include "cocos2d.h"
#include "cocos-ext.h"
#include "ui/CocosGUI.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
#define IS_PC 1
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define IS_IOS 1
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define IS_ANDROID 1
#endif

#if IS_PC || IS_IOS
#define  LOG(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "Effect"
#define  LOG(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif

//редактор и + сохранения при деструкторах
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#define HAS_DEVELOP 1
#else
#define HAS_DEVELOP 0
inline void DebugBreak() {LOG("DebugBreak %s %s %d",__FILE__, __FUNCTION__, __LINE__);}
#endif

//todo Develop Mode crash 
//не все объекты вышружаются после смен сцен
//	#define HAS_DEVELOP 1

//___________________________________


USING_NS_CC;
USING_NS_CC_EXT;
using namespace cocos2d::ui;


#include "Utils/Singleton.h"
#ifdef IS_PC
#define GL_VIEW GLViewImpl//GLViewImpl_2
//#include "System/GLViewImpl_2.h"
#else
#define GL_VIEW GLViewImpl
#include<algorithm>
#endif

#include "2d/CCScene.h"


#include <time.h>
#include <math.h>
#include "Utils/XmlUtils.h"
#include "Utils/MathUtils.h"
#include "Utils/HelperUtils.h"
#include "Utils/SettingUtils.h"
#include "Utils/VisibleRect.h"
#include "Utils/Easing.h"

#define RAND_F(min,max) RandomHelper::random_real(float(min),float(max))
#define RAND_I(min,max) RandomHelper::random_int(int(min),int(max))

#define DrawColor(color) cocos2d::ccDrawColor4B(color.r,color.g,color.b,color.a)

#define _M_PI  M_PI
#define _M_PI2  M_PI_2
#define _M_2PI 6.2831853071794f
#define _RAD_0001 0.0000174532925// MATH_DEG_TO_RAD( 0.001 )
#define _EP_RAD 0.0001f
const Vec2 IONE(-1.0f, -1.0f);
const Vec2 ONE(1.0f, 1.0f);

#define ISONE(X) (X.distance(IONE)<=0.0001f)//X == IONE 

#define SIGN(x) ((x) > 0 ? (+1) : (-1))

#define ABS(x) ( (x) * (SIGN(x)))

#define COLOR3B_RANDOM() Color3B(CCRANDOM_0_1() * 255, CCRANDOM_0_1() * 255, CCRANDOM_0_1() * 255)

#define TAG_LAYER_1		10000
#define TAG_SCENE_1		10001

#define FontArial				"fonts/arial.ttf"
#define FontMarkerFelt			"fonts/Marker Felt.ttf"
#define FontCirceBold			"fonts/CirceBold.ttf"
#define FontCirceLight			"fonts/CirceLight.ttf"
#define kTagMenuAcceptDecline 100012


enum KeyState{
	KeyState_NONE = -1,
	KeyState_PRESS,
	KeyState_RELEASE
	//,KeyState_PROCESS
};

typedef  unsigned int uint;
typedef  unsigned long ulong;

#define CALL_METHOD(obj, method, def) (obj != NULL? obj->method : def )
#define CALL_VOID_METHOD(obj, method) do if (obj != NULL) obj->method ; while(false)

#if 0
typedef void( *setSize )(Size);
typedef Size( *getSize )(void);

typedef std::string( *getString )(void);
typedef void( *setString )(const std::string&);

typedef void( *isDevelopMode )(bool);

#define IS_DEVELOP_MODE(_SELECTOR) static_cast<isDevelopMode>(&_SELECTOR)

#define SET_SIZE(_SELECTOR) static_cast<getSize>(&_SELECTOR)
#define GET_SIZE(_SELECTOR) static_cast<setSize>(&_SELECTOR)

#define SET_STRING(_SELECTOR) static_cast<getString>(&_SELECTOR)
#define GET_STRING(_SELECTOR) static_cast<setString>(&_SELECTOR)
#endif



//typedef std::function<const Rect( void )> getRect;
//typedef std::function<void( const Rect& )> setRect;


typedef std::function<void(void)> callback_void;
typedef std::function<void(void*)> callback_render_texture;

typedef std::function<const float( void )> getFloat;
typedef std::function<void( const float& )> setFloat;

typedef std::function<const std::string( void )> getString;
typedef std::function<void( const std::string& )> setString;

typedef std::function<void( const bool& )> setBool;
typedef std::function<const bool( void )> getBool;

typedef std::function<const Vec2( void )> getPoint;
typedef std::function<void( const Vec2& )> setPoint;

typedef std::function<const size_t( void )> addNewObj;
typedef std::function<void( const size_t& )> eraceObj;

typedef std::function<bool( EventKeyboard::KeyCode, KeyState keyState )> changeKeyEvent;


typedef std::function<void( const size_t& )> setUIntItems;
typedef std::function<const size_t( void )> getUIntItems;


typedef std::function<void( const size_t& )> setUInt;
typedef std::function<const size_t( void )> getUInt;

typedef std::function<void( const Color3B& )> setColor;
typedef std::function<const Color3B( void )> getColor;

typedef std::function<void( void )> changeEvent;
