#include "stdafx.h"
#include "Effect.h"

class InvertDrawEffect : public Effect
{
public:
	static InvertDrawEffect* create();
	~InvertDrawEffect();

protected:
	bool init();
	//bool initNormalMap( const std::string& normalMapFileName );
	//virtual void setTarget( EffectSprite* sprite ) override;
	//EffectSprite* _sprite;

	virtual bool prepareForRender( cocos2d::Sprite *sprite, cocos2d::Texture2D *normalmap );
};
