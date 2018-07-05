#pragma once

#include "stdafx.h"
#include "Soul.h"
#include "UI/LayoutRefresh.h"
#include "WorldSpline/TSpline.h"
#include "Effects/EffectRenderTexture.h"
#include "ExtendedNode/DrawNode2.h"


//#define SHADOW_METHOD  0 // no shadow
//#define SHADOW_METHOD  1 // cpu shadow map + sahder angle blur + blur
#define SHADOW_METHOD  2 // raial map from gpu + blur

struct TShadowRay
{
    Vec2 _pos;
    float _angle;
    float _lenght;

    float _unique;

    TShadowRay( const Vec2& pos ) :_angle( pos.getAngle() ), _unique( false ) { setPos( pos ); }

    TShadowRay( const Vec2& pos, float angle, bool unique = false ) :_angle( angle ), _unique( unique ) { setPos( pos ); }

    void setPos( const Vec2& pos )
    {
        _pos = pos;
        _lenght = _pos.getLength();
    }
};

class Light : public Soul
{
public:
    Light( Layer* layer, size_t id );
    virtual ~Light();
    virtual SoulType getType() { return SoulType_Light; };
    virtual void Load( SettingUtil* setting );
    virtual SettingUtil* Save();

    virtual bool select();
    virtual void deselect();
    virtual void refresh();

    virtual void setPos( const Vec2& pos );

    void setColor( const Color3B& color );
    Color3B getColor() { return _color; };

    //void setAmbientColor( const Color3B& color );
    //Color3B getAmbientColor() { return _ambientColor; };

    void setCover( float value );
    float getCover() { return _cover; };

    void setDirect( float value );
    float getDirect() { return _direct; };

    void setBrightness( float value );
    float getBrightness() { return _brightness; };

    void setCutoffRadius( float value, bool force = false );
    float getCutoffRadius() { return _cutoffRadius; };

    void setHalfRadius( float value );
    float getHalfRadius() { return _halfRadius; };

    virtual void setZ( int val );

    void update( const std::vector<TSpline*>& polygon, bool isChange );

    size_t getId() { return _id; }
    bool isVisible();

    Node* getLightMap() { return _lightMap; };

private:

#if HAS_DEVELOP
    LayoutRefresh* edit_direct;
    LayoutRefresh* edit_cover;
    LayoutRefresh* edit_cutoffRadius;
    LayoutRefresh* edit_halfRadius;
    LayoutRefresh* edit_color;
    //LayoutRefresh* edit_ambientColor;
    LayoutRefresh* edit_brightness;
    LayoutRefresh* edit_brightnessShadow;

    LayoutRefresh* edit_size;
    LayoutRefresh* edit_ep;
#endif

    bool _debugDraw;

    Sprite* _lamp;

	Vec2 _shadow_rt_centr;
	RenderTexture* _shadow_renderTexture = NULL;

#if SHADOW_METHOD == 1
    Effect* _shader_GradientMap = NULL;
    Effect* _shader_RadialBlur = NULL;
    Effect* _shader_Blur = NULL;
    RenderTexture* _light_renderTexture = NULL;
    

    RenderTexture* _shadow_renderTexture_2 = NULL;
    RenderTexture* _shadow_renderTexture_3 = NULL;

	float _ep_lenghtLine;
	size_t _segments = 40;
	std::vector<Vec2> _lightsRay;

	void drawSector(const TShadowRay& ray1, const TShadowRay& ray2, float radiuas, DrawNode* draw, const Color4F& color);
	void drawSectors(Vec2 centr, bool sector, const std::vector<TShadowRay>& list, float radius, DrawNode* draw, const Color4F& color);

	std::vector<Vec2> getListSector(Vec2 centr, bool sector, const std::vector<TShadowRay>& list);
	void drawSectors2(Vec2 centr, bool sector, const std::vector<TShadowRay>& list, DrawNode2* draw, const Color4F& color);
	void drawLight(const std::vector<TSpline*>& polygon, bool isSector, const std::vector<TShadowRay>& lightRay, DrawNode2* drawNode, const Color4F& color_shadow, const Color4F& color_Lshadow);
	void checkNewRay(const TShadowRay& ray, const TShadowRay& ray2, std::vector<TShadowRay>& list, float radius);

	std::vector<TShadowRay> getLightRay(const std::vector<Vec2>& lights, const std::vector<TSpline*>& polygon,
										const Vec2& pos,
										float radius, float direct, float cover,
										int segments, bool ignoreZ = false);

#elif SHADOW_METHOD == 2
	size_t _countHeightSahdow = 2;//min = 1
    RenderTexture* _occlusionMap_rendrTexture = NULL;
    RenderTexture* _shadowMap_1D_rederTexture = NULL;
    Effect* _shader_shadowMap = NULL;
    Effect* _shader_shadowRender = NULL;
#endif
    Node* _lightMap = NULL;

    size_t _id;
    Color3B _color;
    //Color3B _ambientColor;
    float _brightness;
    float _cutoffRadius;
    float _halfRadius;
    float _direct;
    float _cover;
    bool _updateShadow;
    size_t _softness = 0;



    bool _prevVisible = false;

	//size_t _countHeightSahdow = 2;
    float _upscale;

    float _brightnessShadow;
    int _methodBlur;

    float _blurAngle;
    float _blurStrenght;

    float _blurRadius;
    float _blurSample;

    bool inDirContains( const Vec2& p, float direct, float cover );
    bool inDirContains( float rad, float direct, float cover );

    std::vector<Vec2> createLightRay( float radius, float direct, float cover, int segments );
};
