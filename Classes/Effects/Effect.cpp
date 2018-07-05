#include "stdafx.h"
#include "Effect.h"

Effect* Effect::create(const std::string & fragmentFilename) {
	Effect *effect = new (std::nothrow) Effect();
	if (effect && effect->initGLProgramState(fragmentFilename))
	{
		effect->autorelease();
		return effect;
	}
	CC_SAFE_DELETE(effect);
	return nullptr;
}

long Effect::getHash(const char* fileName) {
#if HAS_DEVELOP
    FILE* fp = fopen(fileName, "rt");
    if (fp == NULL)
        return -1;
    long ret = HelperUtils::GetLastDataEdit(fileName);
    fclose(fp);
    return ret;
#else
	return -1;
#endif
}

cocos2d::GLProgramState* Effect::getGLProgramState() {
#if HAS_DEVELOP
	long newHash = getHash(fragmentFullPath.c_str());
	if (newHash != lastHash)
		initGLProgramState(fragmentFullPath);
#endif
	return _glprogramstate;
}

bool Effect::initGLProgramState(const std::string &fragmentFilename) {
	auto fileUtiles = FileUtils::getInstance();
	fragmentFullPath = fileUtiles->fullPathForFilename(fragmentFilename);
	auto fragSource = fileUtiles->getStringFromFile(fragmentFullPath);
	auto glprogram = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.c_str());

	GLenum __error = glGetError();
	if (__error)
		LOG("OpenGL error 0x%04X in %s %s %d\n", __error, __FILE__, __FUNCTION__, __LINE__);
#if HAS_DEVELOP
	lastHash = getHash(fragmentFullPath.c_str());
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	_fragSource = fragSource;
#endif

	if (__error) {
		LOG("path = %s", fragmentFullPath.c_str());
		LOG("fragSource = %s", fragSource.c_str());
	}
	if (glprogram == nullptr)
		LOG("NOTSHADER");
	_glprogramstate = GLProgramState::getOrCreateWithGLProgram(glprogram);
	_glprogramstate->retain();

	return _glprogramstate != nullptr;
}

Effect::Effect() :
	_glprogramstate(nullptr),
	lastHash(0) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	_backgroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED,
		[this](EventCustom*) {
		auto glProgram = _glprogramstate->getGLProgram();
		glProgram->reset();
		glProgram->initWithByteArrays(ccPositionTextureColor_noMVP_vert, _fragSource.c_str());
		glProgram->link();
		glProgram->updateUniforms();
	}
	);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backgroundListener, -1);
#endif
}

Effect::~Effect() {
	CC_SAFE_RELEASE_NULL(_glprogramstate);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	Director::getInstance()->getEventDispatcher()->removeEventListener(_backgroundListener);
#endif
}

