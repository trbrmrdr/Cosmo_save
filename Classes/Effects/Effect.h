#pragma once

#include "stdafx.h"

class Effect : public cocos2d::Ref
{
public:
	static Effect* create(const std::string &fragmentFilename);

	cocos2d::GLProgramState* getGLProgramState();// const; { return _glprogramstate; }
	virtual bool prepareForRender(Sprite* sprite, cocos2d::Texture2D* normalmap) { return false; }
	virtual bool prepareForRender(RenderTexture* sprite) { return true; }

	static long getHash(const char* fileName);
protected:

	std::string fragmentFullPath;
	long lastHash;
	bool initGLProgramState(const std::string &fragmentFilename);

	Effect();
	virtual ~Effect();
	cocos2d::GLProgramState* _glprogramstate;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	std::string _fragSource;
	cocos2d::EventListenerCustom* _backgroundListener;
#endif
};

