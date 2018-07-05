#pragma once
#include "stdafx.h"
#include "UI/LayoutRefresh.h"
#include "Effects/EffectSprite.h"
#include "LightSystem/LightSystem.h"
#include "World/DrawToDots/DrawToDots.h"

#include "Effects/EffectRenderTexture.h"
#include "ExtendedNode/DrawNode2.h"

#define CREATE_RT(val) \
{val = EffectRenderTexture::create(_size.width, _size.height, Texture2D::PixelFormat::RGBA8888, 0);\
val->retain();\
val->setAutoDraw(false);\
val->setScale(1/_RTScale);\
val->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);\
}

#define RT_BRUSH_BEGIN()  \
{if (_clearBrush) {          \
    _rt_brushs->beginWithClear(.0, .0, .0, .0);\
    _clearBrush = false;\
}\
else{\
_rt_brushs->begin();}}

#define RT_BRUSH_END() _rt_brushs->end()


//#define TYPE_EMERGING 1//через back buffer растекающиеся
//#define TYPE_EMERGING 2//через CellularNoise
#define TYPE_EMERGING 3 // sprite 

#define emerging_delay_1  2.5f
#define emerging_delay_2  3.5f

class EmergingImage {
public:
	static Node * AddRect(const Size& size, const Color3B& color) {
		DrawNode* rect = DrawNode::create(3);
		rect->setContentSize(size);
		//rect->drawRect(Vec2::ZERO, size, Color4F(color, 0.4f));
		return rect;
	}

	static Sprite* AddSprite(Texture2D* texture, Vec2 pos, float scale, bool flip) {
		Sprite* ret = Sprite::createWithTexture(texture);
		ret->setScale(1. / scale);
		ret->setFlippedY(flip);
		//ret->addChild(AddRect(texture->getContentSize(), Color3B(128, 126, 255)));
		ret->setPosition(pos);
		return ret;
	}

	static Sprite* AddSprite(EffectRenderTexture* rt, Vec2 pos, float scale, bool flip) {
		Sprite* ret = AddSprite(rt->getSprite()->getTexture(), pos, scale, flip);
		return ret;
	}

private:
	Layer * _layer;

	int _isStage;

	Size _size;
	float _RTScale;
	float _scale_ui;

	Vec2 _cpos;

	EffectRenderTexture* _rt_brushs;		//текстура капель как с руки так и програмно

	EffectRenderTexture* _rt_mask;	//текстурка для растекающихся капель
	Sprite* _rt_mask_Sprite;

	EffectRenderTexture* _rt_after_mask;	//текстура для картины через маску из капель
	Sprite* _rt_after_mask_Sprite;

	EffectRenderTexture* _rt_image_finish;
	Sprite* _rt_image_finish_Sprite;

	Effect* _shader_invert;
#if TYPE_EMERGING == 1
	Effect* _shader_dissolution;
#elif TYPE_EMERGING == 2
	Effect* _shader_replace_dis;
#endif

	Texture2D* _background;//самый задний фон
	Vec2 _background_scale;

	std::string _curr_level_name;
	bool _clearBrush;

	Vec2 drawP1 = IONE;
	Vec2 drawP2 = IONE;
	Vec2 endP = IONE;
	bool isProcess;
	bool isDown;

	Sprite* _brush;
	float _distanceEdgeBrush;
	float _incBrush;

	bool _end;
	float _elapsed;
	float _duration;

	bool isBrush;
	float sacleBrSp = .9f;//13.f;	// =>32
	float _scaleDistance = .125f;// .25f; // 
	float _scaleInc = .25f;// .125f;
public:
	DrawNode2 * _white_draw_node;
	EmergingImage(Layer* layer, Size size, float rt_scale, float scale_ui):
		_duration(emerging_delay_1),
		_isStage(0),

		_layer(layer),
		_size(size),
		_RTScale(rt_scale),
		_scale_ui(scale_ui),
		_clearBrush(false),
		isProcess(false),
		isDown(false),
		isBrush(false),
		_end(false),
		_elapsed(.0f),

		_background(NULL),
		_white_draw_node(NULL) {
		_cpos = _size * 0.5f;

		CREATE_RT(_rt_mask);
#if TYPE_EMERGING == 1
		CREATE_RT(_rt_brushs);
		_rt_mask->setEnableBackBuffer(true);
#endif
		CREATE_RT(_rt_after_mask);

		_rt_mask_Sprite = AddSprite(_rt_mask, Vec2::ZERO, _RTScale, true);
		_layer->addChild(_rt_mask_Sprite);
		_rt_mask_Sprite->setVisible(false);
		//_______________
		_rt_after_mask_Sprite = AddSprite(_rt_after_mask, Vec2::ZERO, _RTScale, true);
		_rt_after_mask_Sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
		_layer->addChild(_rt_after_mask_Sprite, 11);
		_rt_after_mask_Sprite->setVisible(false);

		//_________________
		CREATE_RT(_rt_image_finish);
		_rt_image_finish_Sprite = AddSprite(_rt_image_finish, Vec2::ZERO, _RTScale, true);
		_rt_image_finish_Sprite->retain();

		_rt_image_finish_Sprite->setPosition(_cpos);
		_rt_image_finish_Sprite->setScale(1.);
		//_layer->addChild(_rt_image_finish_Sprite,-100);
		//_rt_image_finish_Sprite->setVisible(false);
		//###################################

		_shader_invert = Effect::create("shaders/drawInvert.frag");
		_shader_invert->retain();

#if TYPE_EMERGING == 1
		_shader_dissolution = Effect::create("shaders/dissolution.frag");
		_shader_dissolution->retain();
#elif TYPE_EMERGING == 2
		_shader_replace_dis = Effect::create("shaders/replace_dissolution.frag");
		_shader_replace_dis->retain();
#endif
		_white_draw_node = DrawNode2::create();

		float tw = _size.width*.5;
		float th = _size.height*.5;
		_white_draw_node->drawPolygon({
			Vec2(-tw, -th),
			Vec2(-tw, th),
			Vec2(tw, th),
			Vec2(tw, -th) }, Color4F::WHITE);
		_layer->addChild(_white_draw_node, 6);
		_white_draw_node->setVisible(false);

		//###################################

		_brush = Sprite::create("brush_sprite.png");
		_brush->retain();
		_brush->setColor(Color3B::BLACK);
		_brush->setOpacity(150);
		_brush->setScale(sacleBrSp);

		float wBrush = _brush->getTexture()->getContentSize().width * sacleBrSp;
		_distanceEdgeBrush = wBrush * _scaleDistance;
		_incBrush = wBrush * _scaleInc;
	};


	~EmergingImage() {
		if (_white_draw_node)_white_draw_node->removeFromParent();
		_brush->release();

		_rt_mask->release();
		_shader_invert->release();

#if TYPE_EMERGING == 1
		_rt_brushs->release();
		_shader_dissolution->release();
#elif TYPE_EMERGING == 2
		_shader_replace_dis->release();
#endif
		_rt_image_finish->release();
		_rt_image_finish_Sprite->release();
	};

	void reset() {
		_duration = 0.;
		_isStage = 0;

		_background = NULL;

		_end = false;
		_elapsed = .0;

		isProcess = false;
		_clearBrush = false;
#if TYPE_EMERGING == 1
		_rt_brushs->clear();
#endif
		_rt_mask->clear();
		_rt_after_mask->clear();
		isBrush = false;

		_rt_after_mask_Sprite->setVisible(false);

		CALL_VOID_METHOD(_white_draw_node, setVisible(false));
	};


	void setZ(int val) {
		CALL_VOID_METHOD(_rt_mask_Sprite, setLocalZOrder(val));
	};

	void setVisible(bool visible) {
		_rt_after_mask_Sprite->setVisible(visible);
	}

	Texture2D* getMask() { return _rt_mask->getTexture(); };
	Texture2D* getEmergingTex() { return _rt_after_mask->getTexture(); };

	void loadImageFinish(const std::string& name) {
		Sprite* image_resilt = Sprite::create(name);
		image_resilt->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		image_resilt->setPosition(_cpos);
		image_resilt->setScale(_scale_ui*_RTScale);

		_rt_image_finish->beginWithClear();

		image_resilt->visit();

		_rt_image_finish->end();
	};

	void update(float delay);
	void processMaskPoints(float delay, float speed);
	void processImageFromMask();

	bool mouseDown(const Vec2& mousePos);
	bool mouseUp(const Vec2& mousePos);
	bool mouseMovie(const Vec2& mousePos);
	void drawBrush(const Vec2& pos, bool force = false);
	void setStage(int stage);//второй проход  появляется фон	 поверх    текстуры с точками

	bool hasEmerging() { return _isStage != 0; }
	void setLevelName(const std::string& name) { _curr_level_name = std::string(name); }
	Texture2D* getBackground() { return _background; };
};


//#define TYPE_PARTICE 1//частицы через шайдер
#define TYPE_PARTICE 2//частицы через спрайты

class FinishImage {
private:
	Layer * _layer;
	EmergingImage * _emergin_img;
	bool _end_emerging;
	//Sprite* _border;
	RenderTexture* _border;
public:
	FinishImage(Layer* layer, DrawToDots* drawToDots, float scale_ui, const Size& size);
	virtual ~FinishImage();

	virtual bool mouseDown(const Vec2& mousePos) {
		if (!_isVisible)return false;
		return _emergin_img->mouseDown(mousePos);
	}

	virtual bool mouseUp(const Vec2& mousePos) {
		if (!_isVisible)return false;
		return _emergin_img->mouseUp(mousePos);
	}

	virtual bool mouseMovie(const Vec2& mousePos) {
		if (!_isVisible)return false;
		return _emergin_img->mouseMovie(mousePos);
	}


	void setZ(int val) {
		_emergin_img->setZ(val);
	};

	void update_shader_particle(float percent, float delay);
	void update_sprite_particle(float percent, float delay);

	void update(float delay);
	void startClear();

	void setEndClear(const callback_void& callback) {
		callback_to_end_clear = callback;
	}

	void setSuccessLevel(const setBool& callback) {
		callback_successLevel = callback;
	}

	void textureSaved(void* data);

	void update_emerging(float delay);

	void setLevelName(const std::string& name) { _emergin_img->setLevelName(name); };

	void loadImageFinish(const std::string& name) { _emergin_img->loadImageFinish(name); };

	void setVisible(bool visible);
private:
	callback_void callback_to_end_clear;

	bool _callback_sl_disabled;
	setBool callback_successLevel;
	DrawToDots* _drawToDots;
	bool _isVisible;

	Size _size;

	Effect* _shader_particle;
	Effect* _shader_cleared_end;

	Vec2 _cpos;

	bool start_free_canvas;

	float _RTScale;//scale для текстур капель и появления
	float _elapsed;

	Sprite* _rt_to_particle_sprite;
	EffectRenderTexture* _rt_to_particle;

	Sprite* _rt_to_cleared_sprite;
	Sprite* _draw_to_dot_sprite;
	EffectRenderTexture* _rt_cleared_canvas;


	void reset();
};
