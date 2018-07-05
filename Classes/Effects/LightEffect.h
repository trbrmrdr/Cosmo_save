#include "stdafx.h"
#include "Effect.h"

struct lightStruct
{
	Vec3 _lightPos;
	Color3B _lightColor = Color3B::WHITE;
	//Color3B _ambientLightColor = Color3B::BLACK;
	float _lightCutoffRadius = 100.0f;
	float _lightHalfRadius = 0.9f;
	float _brightness = 0.0f;
	float _direct = 0.0f;
	float _cover = _M_2PI;
	lightStruct() {};
};
typedef std::map<size_t, lightStruct*> mapLights;

class LightEffect : public Effect
{
public:

	//CREATE_FUNC( LightEffect );
	static LightEffect* create();
	~LightEffect();

	size_t getNewId();

	bool addLight( size_t id );
	bool remLight( size_t id );

	void setLightCover( size_t id, float value );
	float getLightCover( size_t id ) { return _lights[id]->_cover; };

	void setLightDirect( size_t id, float value );
	float getLightDirect( size_t id ) { return _lights[id]->_direct; };

	void setLightPos( size_t id, const Vec3& pos );
	Vec3 getLightPos( size_t id ) { return _lights[id]->_lightPos; };

	void setLightColor( size_t id, const Color3B& color );
	Color3B getLightColor( size_t id ) { return _lights[id]->_lightColor; };

	void setAmbientLightColor( const Color3B& color );
	Color3B getAmbientColor() { return _ambientLightColor; };

	void setAmbientLightBrightness( float falue );
	float getAmbientBrightness() { return _ambientBrightness; };

	void setBrightness( size_t id, float value );
	float getBrightness( size_t id ) { return _lights[id]->_brightness; };

	void setLightCutoffRadius( size_t id, float value );
	float getLightCutoffRadius( size_t id ) { return _lights[id]->_lightCutoffRadius; };

	void setLightHalfRadius( size_t id, float value );
	float getLightHalfRadius( size_t id ) { return _lights[id]->_lightHalfRadius; };

	const mapLights getLights() { return _lights; };

protected:
	bool init();
	//bool initNormalMap( const std::string& normalMapFileName );
	//virtual void setTarget( EffectSprite* sprite ) override;
	//EffectSprite* _sprite;

	void setDirection();
	void setBCH();

	virtual bool prepareForRender( cocos2d::Sprite *sprite, cocos2d::Texture2D *normalmap );

	mapLights _lights;

	Color3B _ambientLightColor = Color3B::BLACK;
	float _ambientBrightness = 1;


	Vec3* tmp_lightColor = NULL;
	Vec3* tmp_lightPos = NULL;
	Vec3* tmp_bCH = NULL;//bright	-	CutoffRadius	-	HalfRadius
	Vec2* tmp_dc = NULL; //direct	-	cover

};
