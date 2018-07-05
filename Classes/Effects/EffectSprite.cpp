#include "stdafx.h"
#include "EffectSprite.h"
#include "Effect.h"

static int tuple_sort(const std::tuple<ssize_t, Effect*, QuadCommand> &tuple1, const std::tuple<ssize_t, Effect*, QuadCommand> &tuple2)
{
	return std::get<0>(tuple1) < std::get<0>(tuple2);
}

EffectSprite::EffectSprite() :
	Sprite(),
	_defaultEffect(NULL)
{
	_effects.reserve(2);
}

EffectSprite::~EffectSprite()
{
	for (auto &tuple : _effects)
	{
		std::get<1>(tuple)->release();
	}
	CC_SAFE_RELEASE(_defaultEffect);
}

EffectSprite *EffectSprite::create()
{
	auto ret = new (std::nothrow) EffectSprite;
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_RELEASE(ret);
	return nullptr;
}

EffectSprite *EffectSprite::create(const std::string& filename)
{
	auto ret = new (std::nothrow) EffectSprite;
	if (ret && ret->initWithFile(filename))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_RELEASE(ret);
	return nullptr;
}


EffectSprite* EffectSprite::createWithSpriteFrame(cocos2d::SpriteFrame* spriteFrame)
{
	auto ret = new (std::nothrow) EffectSprite;
	if (ret && ret->initWithSpriteFrame(spriteFrame))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_RELEASE(ret);
	return nullptr;

}

#if 0 //LIGHE_ENABLE
bool EffectSprite::setTexture2(const std::string &texture2Filename)
{
	_texture2 = cocos2d::Director::getInstance()->getTextureCache()->addImage(texture2Filename);
	return _texture2 != NULL;
}
#endif

void EffectSprite::setEffect(Effect *effect)
{
	if (_defaultEffect != effect)
	{
		_defaultState = getGLProgramState();

		CC_SAFE_RELEASE(_defaultEffect);
		_defaultEffect = effect;
		CC_SAFE_RETAIN(_defaultEffect);

		setGLProgramState(_defaultEffect->getGLProgramState());
	}
}


void EffectSprite::disableEffect()
{
	if (_defaultEffect == NULL)
		return;
	CC_SAFE_RELEASE(_defaultEffect);
	_defaultEffect = NULL;
	setGLProgramState(_defaultState);
}

void EffectSprite::addEffect(Effect *effect, ssize_t order)
{
	effect->retain();

	_effects.push_back(std::make_tuple(order, effect, QuadCommand()));

	std::sort(std::begin(_effects), std::end(_effects), tuple_sort);
}

void EffectSprite::clearEffects()
{
	for (auto &tuple : _effects)
	{
		std::get<1>(tuple)->release();
	}
	_effects.clear();
}

void EffectSprite::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
#if CC_USE_CULLING
	// Don't do calculate the culling if the transform was not updated
	_insideBounds = (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;

	if (_insideBounds)
#endif
	{
		// negative effects: order < 0
		int idx = 0;
		for (auto &effect : _effects)
		{

			if (std::get<0>(effect) >= 0)
				break;
			QuadCommand &q = std::get<2>(effect);
			q.init(_globalZOrder, _texture->getName(), std::get<1>(effect)->getGLProgramState(), _blendFunc, &_quad, 1, transform, flags);
			renderer->addCommand(&q);
			idx++;
		}

		if (NULL != _defaultEffect)
		{
#if 0//LIGHT_ENABLE
			if (_defaultEffect->prepareForRender(this, _texture2))
			{
			}
			else
			{
				GLProgramState* shader = _defaultEffect->getGLProgramState();
				//todo remove trash log
				shader->setUniformVec2("resolution", getTexture()->getContentSize());
				setGLProgramState(shader);
			}
#else
			GLProgramState* shader = _defaultEffect->getGLProgramState();
			//shader->setUniformVec2("resolution", getTexture()->getContentSize());
			setGLProgramState(shader);
#endif
		}
		Sprite::draw(renderer, transform, flags);
		//renderer->render();
		return;

		// normal effect: order == 0
		_trianglesCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _polyInfo.triangles, transform, flags);
		renderer->addCommand(&_trianglesCommand);

		// postive effects: oder >= 0
		for (auto it = std::begin(_effects) + idx; it != std::end(_effects); ++it)
		{
			QuadCommand &q = std::get<2>(*it);
			q.init(_globalZOrder, _texture->getName(), std::get<1>(*it)->getGLProgramState(), _blendFunc, &_quad, 1, transform, flags);
			renderer->addCommand(&q);
			idx++;
		}
	}
}
