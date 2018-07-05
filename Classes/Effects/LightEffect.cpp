#include "stdafx.h"
#include "LightEffect.h"
#include "EffectSprite.h"

#define MAX_COUN_LIGHT 100

LightEffect* LightEffect::create()
{
	LightEffect *normalMappedSprite = new (std::nothrow) LightEffect();
	if (normalMappedSprite && normalMappedSprite->init())
	{
		normalMappedSprite->autorelease();
		return normalMappedSprite;
	}
	CC_SAFE_DELETE( normalMappedSprite );
	return nullptr;
}

LightEffect::~LightEffect()
{
	for (auto& it : _lights)
		delete (it).second;
	CC_SAFE_DELETE_ARRAY( tmp_bCH );
	CC_SAFE_DELETE_ARRAY( tmp_lightPos );
	CC_SAFE_DELETE_ARRAY( tmp_lightColor );
	CC_SAFE_DELETE_ARRAY( tmp_dc );
}

size_t LightEffect::getNewId()
{
	size_t ret = 0;
	for (auto& it : _lights)
	{
		if (it.first != ret)
			return ret;
		ret++;
	}

	if (_lights.size() >= MAX_COUN_LIGHT)
		DebugBreak();
	return ret;
}

bool LightEffect::addLight( size_t id )
{
	if (_lights.find( id ) != _lights.end())
		return false;

	_lights.insert( std::pair<size_t, lightStruct*>( id, new lightStruct() ) );
	return true;
}

bool simple_sort( std::pair<size_t, lightStruct*> i, std::pair<size_t, lightStruct*> j ) { return i.first < j.first; }

bool LightEffect::remLight( size_t id )
{
	if (_lights.find( id ) == _lights.end())
		return false;
	_lights.erase( id );
	//std::sort( _lights.begin(), _lights.end(), simple_sort );
	return true;
}

bool LightEffect::init()
{
	if (initGLProgramState( "shaders/pointlight.frag" ))
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

void LightEffect::setLightPos( size_t id, const Vec3 &pos )
{
	_lights[id]->_lightPos = pos;
}

void LightEffect::setLightCover( size_t id, float value )
{
	_lights[id]->_cover = value;
	setDirection();
}

void LightEffect::setLightDirect( size_t id, float value )
{
	_lights[id]->_direct = value;
	setDirection();
}

void LightEffect::setDirection()
{
	size_t size = _lights.size();

	CC_SAFE_DELETE_ARRAY( tmp_dc );
	tmp_dc = new Vec2[size];
	int i = 0;
	for (auto& it : _lights)
	{
		tmp_dc[i] = Vec2( it.second->_direct, it.second->_cover );
		i++;
	}
	getGLProgramState()->setUniformVec2v( "_dc", size, &tmp_dc[0] );
}


void LightEffect::setLightColor( size_t id, const Color3B &color )
{
	_lights[id]->_lightColor = color;
	size_t size = _lights.size();
	CC_SAFE_DELETE_ARRAY( tmp_lightColor );
	tmp_lightColor = new Vec3[size];
	int i = 0;
	for (auto& it : _lights)
	{
		Color3B _color = it.second->_lightColor;
		tmp_lightColor[i] = Vec3( _color.r, _color.g, _color.b ) / 255.0f;
		i++;
	}

	getGLProgramState()->setUniformVec3v( "_lightColor", size, &tmp_lightColor[0] );
	//delete tmp;
}

void LightEffect::setAmbientLightBrightness( float falue )
{
	_ambientBrightness = MAX( 0.f, falue );
	setAmbientLightColor( getAmbientColor() );
}

void LightEffect::setAmbientLightColor( const Color3B &color )
{
	_ambientLightColor = color;
	Vec3 vColor = Vec3( _ambientLightColor.r, _ambientLightColor.g, _ambientLightColor.b ) / 255.0f * _ambientBrightness;
	getGLProgramState()->setUniformVec3( "_ambientColor", vColor );
}

void LightEffect::setBrightness( size_t id, float value )
{
	_lights[id]->_brightness = value;
	setBCH();
}

void LightEffect::setLightCutoffRadius( size_t id, float value )
{
	float r = std::max( 1.0f, value );
	_lights[id]->_lightCutoffRadius = r;
	setBCH();
}

void LightEffect::setLightHalfRadius( size_t id, float value )
{
	//float r = std::max( 0.01f, std::min( 0.99f, value ) );
	float r = std::max( 0.0f, std::min( 1.f, value ) );
	_lights[id]->_lightHalfRadius = r;
	setBCH();
}

void LightEffect::setBCH()
{

	size_t size = _lights.size();
	CC_SAFE_DELETE_ARRAY( tmp_bCH );
	tmp_bCH = new Vec3[size];
	int i = 0;
	for (auto& it : _lights)
	{
		tmp_bCH[i] = Vec3( it.second->_brightness, it.second->_lightCutoffRadius, it.second->_lightHalfRadius ); //_brightness, _cutoffRadius, _halfRadius
		i++;
	}
	getGLProgramState()->setUniformVec3v( "_bCH", size, &tmp_bCH[0] );
}

#include "Animations/Animations.h"
#include "Utils/MathUtils.h"

bool LightEffect::prepareForRender( Sprite *sprite, Texture2D *normalmap )
{
	Texture2D* normal = normalmap;
	if (NULL == normal)
	{
		return false;
		normal = Animations::Instance().getNormal( sprite );
		if (NULL == normal)
			return false;
	}

	auto gl = getGLProgramState();

	size_t countLights = _lights.size();
	gl->setUniformInt( "_countLight", countLights );

	Vec2 size = MathUtils::GetSizeScale( sprite, true );
	gl->setUniformVec2( "_contentSize", size );

	Vec2 pos_sprite = MathUtils::ConvertToFirstSpace_ignoreRS( sprite, Vec2::ZERO );

	//lightStruct* light = _lights[0];
	//Vec3 lightPos = light->_lightPos;

	//Vec2 posRelToSprite = Vec2( lightPos.x, lightPos.y ) - pos_sprite;// sprite->getPosition();
	//gl->setUniformVec3v( "_lightPos", Vec3( posRelToSprite.x, posRelToSprite.y, lightPos.z ) );

	{
		CC_SAFE_DELETE_ARRAY( tmp_lightPos );
		tmp_lightPos = new Vec3[countLights];
		int i = 0;
		for (auto& it : _lights)
		{
			Vec3 _pos = it.second->_lightPos;
			tmp_lightPos[i] = Vec3(
				_pos.x - pos_sprite.x,
				_pos.y - pos_sprite.y,
				_pos.z );
			i++;
		}
		gl->setUniformVec3v( "_lightPos", countLights, &tmp_lightPos[0] );
	}


	gl->setUniformTexture( "_normals", normal );

	SpriteFrame *frame = sprite->getSpriteFrame();
	Size untrimmedSize = frame->getOriginalSize();
	Size trimmedSize = frame->getRect().size;
	Vec2 framePos = frame->getRect().origin;
	Size texSize = frame->getTexture()->getContentSize();

	// set sprite position in sheet
	gl->setUniformVec2( "_spritePosInSheet", Vec2( framePos.x / texSize.width, framePos.y / texSize.height ) );
	gl->setUniformVec2( "_spriteSizeRelToSheet", Vec2( untrimmedSize.width / texSize.width, untrimmedSize.height / texSize.height ) );
	bool isRotated = frame->isRotated();
	gl->setUniformInt( "_spriteRotated", isRotated );

	// set offset of trimmed sprite
	Vec2 bottomLeft = frame->getOffset() + (untrimmedSize - trimmedSize) / 2;
	Vec2 cornerOffset = isRotated ? Vec2( bottomLeft.y, bottomLeft.x ) :
		Vec2( bottomLeft.x, untrimmedSize.height - trimmedSize.height - bottomLeft.y );
	gl->setUniformVec2( "_spriteOffset", cornerOffset );


	//CCLOG( "%f:%f  u_lightPos", posRelToSprite.x, posRelToSprite.y );

	//CCLOG( "%f:%f  u_contentSize", size.x, size.y );

	//Vec2 PosInSheet = Vec2( framePos.x / texSize.width, framePos.y / texSize.height );
	//CCLOG( "%f:%f  u_spritePosInSheet", PosInSheet.x, PosInSheet.y );
	//Vec2 SizeRelToSheet = Vec2( untrimmedSize.width / texSize.width, untrimmedSize.height / texSize.height );
	//CCLOG( "%f:%f  u_spriteSizeRelToSheet", SizeRelToSheet.x, SizeRelToSheet.y );
	//CCLOG( "%f:%f  u_spriteOffset", cornerOffset.x, cornerOffset.y );
	return true;
}
