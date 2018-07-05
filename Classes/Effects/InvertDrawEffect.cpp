#include "stdafx.h"
#include "InvertDrawEffect.h"
#include "EffectSprite.h"


InvertDrawEffect* InvertDrawEffect::create()
{
	InvertDrawEffect *normalMappedSprite = new (std::nothrow) InvertDrawEffect();
	if (normalMappedSprite && normalMappedSprite->init())
	{
		normalMappedSprite->autorelease();
		return normalMappedSprite;
	}
	CC_SAFE_DELETE( normalMappedSprite );
	return nullptr;
}

InvertDrawEffect::~InvertDrawEffect()
{
}

bool InvertDrawEffect::init()
{
	if (initGLProgramState( "shaders/drawInvert.frag" ))
	{
		//addLight( 0 );
		//setLightColor( 0, Color3B::WHITE );
		//setAmbientLightColor( Color3B::BLACK );
		//setLightCutoffRadius( 0, 500.0f );
		//setLightHalfRadius( 0, 0.5f );
		return true;
	}
	return false;
}

bool InvertDrawEffect::prepareForRender( Sprite *sprite, Texture2D *texture2 )
{
	if (texture2 == NULL)return false;
	auto gl = getGLProgramState();

	//gl->setUniformInt( "_countLight", countLights );

	Vec2 size = MathUtils::GetSizeScale( sprite, true );
	//gl->setUniformVec2( "_contentSize", size );

	Vec2 pos_sprite = MathUtils::ConvertToFirstSpace_ignoreRS( sprite, Vec2::ZERO );
	//gl->setUniformTexture( "_texture2", texture2);

	return true;
}
