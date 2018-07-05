#pragma once
#include "stdafx.h"
#include "Effect.h"

class EffectSprite: public cocos2d::Sprite {
public:

    static EffectSprite* create();
    static EffectSprite* create(const std::string &filename);
    static EffectSprite* createWithSpriteFrame(cocos2d::SpriteFrame* spriteFrame);

#if 0//LIGHT_ENABLE
	void setTexture2(Texture2D* texture2) { _texture2 = texture2; }
	bool setTexture2(const std::string &texture2Filename);
#endif
	void setEffect(Effect *effect);

    void disableEffect();

    void addEffect(Effect *effect, ssize_t order);
    void clearEffects();
protected:
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;

    EffectSprite();
    virtual ~EffectSprite();

    std::vector<std::tuple<ssize_t, Effect*, QuadCommand>> _effects;
    Effect* _defaultEffect;

#if 0//LIGHT_ENABLE
    Texture2D *_texture2 = nullptr;
#endif
	GLProgramState* _defaultState;
    Effect* effect;
};
