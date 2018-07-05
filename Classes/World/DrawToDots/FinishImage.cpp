#include "stdafx.h"
#include "FinishImage.h"
#include "DevelopMenu.h"
#include "Effects/InvertDrawEffect.h"
#include "World/DrawToDots/UI_dtd.h"

bool test_press = false;

//#####################################################

bool EmergingImage::mouseDown(const Vec2& mousePos) {
#if TYPE_EMERGING != 1
	return false;
#endif
	if (!isProcess)return false;
	isDown = true;
	drawP1 = mousePos;
	endP = drawP2 = IONE;
	drawBrush(mousePos, true);
	return true;
}

bool EmergingImage::mouseUp(const Vec2& mousePos) {
#if TYPE_EMERGING != 1
	return false;
#endif
	if (!isProcess)return false;
	endP = mousePos;
	drawP1 = drawP2 = IONE;
	if (isDown) {
		isDown = false;
		//return true;
	}
	return false;
}

bool EmergingImage::mouseMovie(const Vec2& mousePos) {
#if TYPE_EMERGING != 1
	return false;
#endif
	if (!isDown)
		return false;
	//todo remove omly test
	drawP1 = mousePos;

	drawBrush(mousePos);
	return false;
}

void EmergingImage::drawBrush(const Vec2& pos, bool force) {
	int count = 0;
	Vec2 thisPos = MathUtils::ConvertToNodeSpace_ignoreRS(_rt_brushs, pos);
	thisPos += _cpos;// +_image->getPosition();

	if (!ISONE(drawP1)) {
		Vec2 start = ISONE(drawP2) ? thisPos : drawP2;
		Vec2 end = thisPos;
		float distance = start.distance(end);
		float inc = _incBrush;// 10;// distance/100.;
		if (force
			|| distance >= _distanceEdgeBrush) {
			isBrush = true;
			int d = (int)distance;
			float difx = end.x - start.x;
			float dify = end.y - start.y;
			RT_BRUSH_BEGIN();

			Color3B color = COLOR3B_RANDOM();// _brush->getColor();
			for (float i = 0; i <= distance; i += inc) {
				float delta = i / (distance == 0 ? 1 : distance);
				Vec2 newP = Vec2(start.x + (difx * delta), start.y + (dify * delta));
				count++;
				Sprite* sprite = Sprite::createWithTexture(_brush->getTexture());
				sprite->setColor(color);
				sprite->setOpacity(_brush->getOpacity());
				sprite->setPosition(newP);
				sprite->setScale(_brush->getScale());
				sprite->visit();
			}
			RT_BRUSH_END();
			drawP2 = thisPos;
		}
	}
	if (ISONE(drawP2))
		drawP2 = thisPos;
}

//#################################################################

class SpriteTo {
private:
	Sprite * sprite;
	float _elapsed;
	float _duration;

	float _from;
	float _to;
public:
	SpriteTo(Texture2D* texture, const Vec2& pos, float fromS, float toS, float duration) {
		sprite = Sprite::createWithTexture(texture);
		sprite->setColor(COLOR3B_RANDOM());
		sprite->setOpacity(255 * .1f);
		sprite->setPosition(pos);

		sprite->setScale(fromS);

		//sprite->runAction(ScaleTo::create(_duration, 100.));
		sprite->retain();

		_elapsed = .0f;
		_duration = duration;
		_from = fromS;
		_to = toS;
	}
	void update(float delta) {
		_elapsed += delta;
		float percent = MIN(_elapsed / _duration, 1.);
		float scale = (_to - _from)*percent;
		sprite->setScale(_from + scale);
		sprite->visit();
	}

	~SpriteTo() { sprite->release(); }

};
std::vector<SpriteTo*> _sprites;

void EmergingImage::setStage(int stage) {
	if (_isStage == stage)return;
	reset();
	_isStage = stage;
	if (stage == 1) {
		_duration = emerging_delay_1;
	}
	else if (stage == 2) {
		_duration = emerging_delay_2;
		_background = Director::getInstance()->getTextureCache()->addImage(_curr_level_name + "//background.png");
		Sprite* old_background = dynamic_cast<Sprite*>(_layer->getChildByName(UI_dtd::NAME_BACKGROUND_NODE));

		_background_scale = Vec2(
			_size.width / old_background->getContentSize().width,
			_size.height / old_background->getContentSize().height
		) / old_background->getScale();
	}

#if TYPE_EMERGING == 3
	for (SpriteTo* it : _sprites)
		delete it;
	_sprites.clear();

	for (int i = 0; i < 100; i++) {
		Vec2 newP = _size;// _rt_mask_Sprite->getContentSize();
		newP.scale(Vec2(.15f, .15f) + Vec2(CCRANDOM_0_1(), CCRANDOM_0_1()) * (_isStage == 2 ? .7f : .6f));

		float fromScale = _brush->getScale()*CCRANDOM_0_1()*2.;
		/*
		Sprite* sprite = Sprite::createWithTexture(_brush->getTexture());
		sprite->setColor(COLOR3B_RANDOM());
		//sprite->setOpacity(.9f);
		sprite->setPosition(newP);

		//sprite->setScale(fromScale);

		sprite->runAction(ScaleTo::create(_duration, 100.));
		sprite->retain();
		*/
		float toScale = fromScale * (stage == 2 ? 20.f : 8.f);
		_sprites.push_back(new SpriteTo(_brush->getTexture(), newP, fromScale, toScale, _duration));
	}

#endif
}

void EmergingImage::update(float delay) {
	isProcess = true;
	if (_end)return;

#if TYPE_EMERGING == 1
	RT_BRUSH_BEGIN();
	isBrush = true;
	Vec2 newP = _size;// _rt_mask_Sprite->getContentSize();
	newP.scale(Vec2(.15f, .15f) + Vec2(CCRANDOM_0_1(), CCRANDOM_0_1()) * (_isStage == 2 ? .7f : .6f));

	Sprite* sprite = Sprite::createWithTexture(_brush->getTexture());
	sprite->setColor(COLOR3B_RANDOM());
	sprite->setOpacity(_brush->getOpacity());
	sprite->setPosition(newP);
	sprite->setScale(_brush->getScale()*CCRANDOM_0_1()*2.);
	sprite->visit();
	RT_BRUSH_END();
#endif
	//###############################
	_elapsed += delay;
	if (_elapsed >= _duration)
		_end = true;
}

void EmergingImage::processMaskPoints(float delay, float speed) {
#if TYPE_EMERGING == 1
	GLProgramState* shader = _shader_dissolution->getGLProgramState();
	shader->setUniformFloat("_speed", speed);
	shader->setUniformFloat("_delay", delay);
	shader->setUniformVec2("_resolution", _size);
	shader->setUniformInt("_isBrush", isBrush);
	shader->setUniformTexture("_texture_back", _rt_mask->getBackTexture());

	_clearBrush = _clearBrush && !isBrush;

	if (_clearBrush) {
		_rt_brushs->clear();
		_clearBrush = false;
	}

	if (isBrush) {
		isBrush = false;
		_clearBrush = true;
		shader->setUniformTexture("_texture_brush", _rt_brushs->getTexture());
	}


	_rt_mask->beginWithClear();

	Sprite* sprite = _rt_mask->getSprite();
	sprite->setFlippedY(true);
	sprite->setAnchorPoint(Vec2::ZERO);
	//sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	sprite->setGLProgramState(shader);
	sprite->visit();

	_rt_mask->end();
#elif TYPE_EMERGING ==2
	float percent = MIN(_elapsed / _duration, 1.f);
	if (_end) percent = 1.f;
	//if (_end)_elapsed += delay;
	GLProgramState* shader = _shader_replace_dis->getGLProgramState();
	shader->setUniformVec2("_resolution", _size);
	shader->setUniformFloat("_time", _elapsed);
	shader->setUniformFloat("_percent", 1.f - percent);

	//_rt_mask->begin();
	_rt_mask->beginWithClear();

	Sprite* sprite = _rt_mask->getSprite();
	sprite->setFlippedY(true);
	sprite->setAnchorPoint(Vec2::ZERO);
	sprite->setGLProgramState(shader);
	sprite->visit();

	_rt_mask->end();
#else

	float percent = MIN(_elapsed / _duration, 1.f);
	if (_end) percent = 1.f;
	_rt_mask->begin();

	for (SpriteTo* it : _sprites)
		it->update(delay);

	_rt_mask->end();
#endif
	//if (_end)_elapsed += delay;
}

void EmergingImage::processImageFromMask() {
	GLProgramState* shader = _shader_invert->getGLProgramState();
	shader->setUniformTexture("_texture_mask", _rt_mask->getTexture());

	shader->setUniformTexture("_texture_img_finish", _rt_image_finish_Sprite->getTexture());
	int hasBack = _isStage == 2 ? 1 : 0;
	shader->setUniformInt("_hasBack", hasBack);
	if (hasBack) {
		//показываем задний фон через маску убирая задник с точками
		shader->setUniformVec2("_texture_back_scale", _background_scale);
		shader->setUniformTexture("_texture_back", _background);
		//shader->setUniformTexture("_texture_image2", _texture_image2->getTexture());
	}

	_rt_image_finish_Sprite->setGLProgramState(shader);
	_rt_after_mask->beginWithClear();

	_rt_image_finish_Sprite->visit();

	_rt_after_mask->end();
	_rt_after_mask_Sprite->setVisible(true);

#if 0
	//if (test_press) 
	if (false) {
		test_press = false;
		_rt_after_mask->saveToFile("_rt_after_mask.png");
	}
#endif
}

//#######################################################


#define TESTDRAW 1

//curr , prev
#define FOR_EACH(type,_vector)	\
{int size = _vector.size();		\
if (size > 0){							\
type prev = ( *_vector.begin() ); \
for (std::vector<type>::const_iterator		\
it = _vector.begin() + 1,			\
it_e = _vector.end();				\
it != it_e; ++it){						\
type curr = ( *it );

#define END_FOR_EACH()\
prev = curr;}}}


bool has_frame = false;
FinishImage::FinishImage(Layer* layer, DrawToDots* drawToDots, float scale_ui, const Size& size):
	_layer(layer),
	_drawToDots(drawToDots),
	_elapsed(.0f),
	callback_to_end_clear(NULL),
	_rt_cleared_canvas(NULL),
	_draw_to_dot_sprite(NULL),
	callback_successLevel(NULL),
	_border(NULL),
	_callback_sl_disabled(false),
	start_free_canvas(false),
	_end_emerging(false) {

	_isVisible = true;

#if TYPE_PARTICE ==1
#if IS_PC
	_RTScale = .7f;
#else
	_RTScale = .5f;
#endif
#else
	_RTScale = 1.;
#endif

	//_size = VisibleRect::getVisibleRect().size * _RTScale;
	_size = size * _RTScale;

	_emergin_img = new EmergingImage(layer, _size, _RTScale, scale_ui);
	_cpos = _size * 0.5f;//для кординат при соприкосновении с текстуркой для капелек от пальца
	//###############################################

	//текстура частиц
	_rt_to_particle = EffectRenderTexture::create(_size.width, _size.height, Texture2D::PixelFormat::RGBA8888);
	//_rt_to_particle->getSprite()->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	_rt_to_particle->retain();
	_rt_to_particle->setAutoDraw(false);
#if TYPE_PARTICE == 1
	_rt_to_particle->setEnableBackBuffer(true);
#endif
	//from test
	///*
	_rt_to_particle_sprite = EmergingImage::AddSprite(_rt_to_particle, Vec2::ZERO, _RTScale, true);
	_layer->addChild(_rt_to_particle_sprite);
	_rt_to_particle_sprite->setVisible(false);
	/**/

	//############################
	_rt_cleared_canvas = EffectRenderTexture::create(_size.width, _size.height, Texture2D::PixelFormat::RGBA8888);
	_rt_cleared_canvas->retain();
	_rt_cleared_canvas->setAutoDraw(false);

	_rt_to_cleared_sprite = EmergingImage::AddSprite(_rt_cleared_canvas, Vec2::ZERO, _RTScale, true);
	//_rt_to_cleared_sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	_layer->addChild(_rt_to_cleared_sprite);
	_rt_to_cleared_sprite->setVisible(false);

#if TYPE_PARTICE == 1
	_shader_particle = Effect::create("shaders/cleared_particle.frag");
	_shader_particle->retain();

	_shader_cleared_end = Effect::create("shaders/shader_particle_cleared_end.frag");
	_shader_cleared_end->retain();
#else
	_shader_cleared_end = Effect::create("shaders/sprite_particle_cleared_end.frag");
	_shader_cleared_end->retain();
#endif


#if HAS_DEVELOP
	DevelopMenu::Instance().changeKeyEvents(([this](EventKeyboard::KeyCode keyCode, KeyState keyState) {
		bool ret = false;
		if (keyState != KeyState_RELEASE)
			return false;
		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_Q:
				reset();
				return true;
			case EventKeyboard::KeyCode::KEY_A:
				_emergin_img->reset();
				return true;
				///*
			case EventKeyboard::KeyCode::KEY_X:
				test_press = !test_press;
				return true;
				/*
			case EventKeyboard::KeyCode::KEY_SPACE:
				has_frame = true;
				return true;
				*/
		}
		return false;
	}), this);
#endif

}

FinishImage::~FinishImage() {
	CC_SAFE_DELETE(_emergin_img);
	_rt_to_particle->release();
#if TYPE_PARTICE == 1
	_shader_particle->release();
#endif

	_rt_cleared_canvas->release();
	_shader_cleared_end->release();
}

class ParticleSprite {
private:
public:
	Sprite * sprite;
	float _elapsed;

	Vec2 _pos;
	float _width;
	float s_percent;

	float to_angle;
	Vec2 to_pos;
	Vec2 from_pos;
	bool hasStart;
public:
	ParticleSprite(Node* layer, Vec2 d_pos, Texture2D* texture, const Rect& rect, float width) {
		Vec2 old_s = rect.size;
		Vec2 d_new_s = Vec2(random(.0, .5), random(0., .5));
		Vec2 new_s = Vec2(d_new_s.x*old_s.x, d_new_s.y*old_s.y);

		sprite = Sprite::createWithTexture(texture, Rect(rect.origin - new_s, Size(old_s + new_s * 2.)));
		sprite->setFlipY(true);
		sprite->setPosition(rect.origin + rect.size*.5);
		sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
		//sprite->setColor(COLOR3B_RANDOM());
		sprite->retain();
		_elapsed = .0f;


		/*
		DrawNode* test = DrawNode::create();
		test->setPosition(rect.origin - d_pos);
		test->drawRect(Vec2::ZERO, rect.size, Color4F::BLACK);
		//test->drawRect(Vec2::ZERO - new_s, old_s + new_s, Color4F(COLOR3B_RANDOM(), 1.));
		*/


		_pos = rect.origin;
		_width = width;
		s_percent = _pos.x / _width;
		to_angle = 360.*rand_minus1_1();
		from_pos = sprite->getPosition();
		to_pos = from_pos + Vec2(old_s.x *-random(.5, 2.), old_s.y*2.*rand_minus1_1());
		hasStart = false;

		/*
		test->drawLine(from_pos, to_pos, Color4F(COLOR3B_RANDOM(), 1.));
		test->drawPoint(to_pos, 5, Color4F::BLACK);
		layer->addChild(test, 9999);
		*/
	}

	void update(float percent, float delta) {

		sprite->visit();

		if (percent <= .0) {
			sprite->setOpacity(255.);
			sprite->setScale(1.);
			sprite->setPosition(from_pos);
			sprite->setRotation(.0);
			hasStart = false;
			_elapsed = .0f;
			return;
		}

		if (!hasStart && percent >= s_percent) {
			//hasStart = true;
			_elapsed += delta;
			float percent = MIN(_elapsed / .7, 1.);
			float scale = 1. - percent;
			sprite->setScale(scale);
			sprite->setRotation(to_angle * percent);
			sprite->setPosition(from_pos - (from_pos - to_pos)*percent);

			sprite->setOpacity(255.*(1. - percent));
		}

	}

	~ParticleSprite() {
		sprite->release();
		//sprite->removeFromParent(); 
	}
};

std::vector<ParticleSprite*> _particle_cprite;

void FinishImage::reset() {
	if (_elapsed <= .0f)return;
	_emergin_img->reset();

	_elapsed = .0;
	_draw_to_dot_sprite = NULL;
	start_free_canvas = false;
	_rt_to_particle->clear();
	_rt_cleared_canvas->clear();

	//_rt_to_cleared_sprite->setLocalZOrder(0);
	_callback_sl_disabled = false;
	_end_emerging = false;

	//test reset
	if (NULL != _border)
		_border->setVisible(true);
#if TYPE_PARTICE ==2
	for (auto* sprite : _particle_cprite)
		delete sprite;
	_particle_cprite.clear();
#endif
}

EffectRenderTexture* to_save = NULL;
void FinishImage::textureSaved(void* data) {
	//to_save = static_cast<EffectRenderTexture*>( data );
	_draw_to_dot_sprite = static_cast<EffectRenderTexture*>(data)->getSprite();
}

void FinishImage::startClear() {
	start_free_canvas = true;
	_drawToDots->saveTexture([this](void* data) { textureSaved(data); });
}

void FinishImage::setVisible(bool visible) {
	if (_isVisible == visible) return;
	_isVisible = visible;
	reset();
	_emergin_img->setVisible(visible);
}

//удалил обнуление callback_successLevel после использования (и проверку на null визде перед использованием)
void FinishImage::update_emerging(float delay) {
	if (_end_emerging)return;
	if (!_drawToDots->isEnd()) return;

	if (_elapsed <= .0) {
		_emergin_img->setStage(1);
		callback_successLevel(true);
	}
	_elapsed += delay;
	_emergin_img->update(delay);


#if 0//test
	_emergin_img->setStage(2);
	_emergin_img->processMaskPoints(delay, 12.5f);

	if (!_callback_sl_disabled) {
		callback_successLevel(true);
		_callback_sl_disabled = true;
	}
	/*
	if (NULL == _draw_to_dot_sprite) {
		_drawToDots->saveTexture([this](void* data) { textureSaved(data); });
		return;
	}
	else {
		_emergin_img->setEmerging(_draw_to_dot_sprite);
	}
	*/

#else
	if (_elapsed <= emerging_delay_1) {
		_emergin_img->processMaskPoints(delay, 9.5f);
	}
	else if (_elapsed >= emerging_delay_1 + emerging_delay_2) {
		_end_emerging = true;
		_elapsed = .0f;

		if (!_callback_sl_disabled) {
			callback_successLevel(false);
			_callback_sl_disabled = true;

			//сохраняе задний фон для дальнейшего прохождения
			Sprite* background = dynamic_cast<Sprite*>(_layer->getChildByName(UI_dtd::NAME_BACKGROUND_NODE));
			background->setTexture(_emergin_img->getBackground());
		}

		//form circle test
		//_elapsed = .0;
		//_emergin_img->reset();
		//_end_emerging = false;
	}
	else {
		_emergin_img->setStage(2);
		_emergin_img->processMaskPoints(delay, 10.5f);
	}
#endif
	_emergin_img->processImageFromMask();

	//Director::getInstance()->getRenderer()->render();
}

void FinishImage::update_shader_particle(float percent, float delay) {
	//_isVisible = false;
	//if (!has_frame)return;
	has_frame = false;
	//частицы
	{
		//only test
		//_rt_to_particle_sprite->setVisible(true);
		//_rt_to_particle_sprite->setLocalZOrder(10000);
		//##########################
		GLProgramState* shader = _shader_particle->getGLProgramState();
		//shader->setUniformFloat("_delay", delay);

		//shader->setUniformInt("max_percent", max_percent);
		//shader->setUniformInt("_press", test_press ? 1 : 0);

		shader->setUniformVec2("_resolution", _size);
		shader->setUniformFloat("_time", _elapsed);
		shader->setUniformFloat("_time_delta", delay);
		//shader->setUniformInt("has_first", has_first);
		shader->setUniformTexture("_texture_back", _rt_to_particle->getBackTexture());
		shader->setUniformFloat("_percent", percent);

		_rt_to_particle->beginWithClear();

		Sprite* sprite = _rt_to_particle->getSprite();
		sprite->setFlippedY(true);
		sprite->setAnchorPoint(Vec2::ZERO);
		//sprite->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);

		sprite->setGLProgramState(shader);
		sprite->visit();

		_rt_to_particle->end();
	}

#if 1
	//2 вывод самой картинки
	{

		GLProgramState* shader = _shader_cleared_end->getGLProgramState();
		//shader->setUniformFloat("_delay", delay);
		//shader->setUniformInt("_press", test_press ? 1 : 0);
		//shader->setUniformInt("_max_percent", max_percent);
		shader->setUniformTexture("_texture2_1", _draw_to_dot_sprite->getTexture());
		/*
		#if 1
		Size size = _size_from_clear;
		Vec2 scale = Vec2(_size_from_clear.width / _draw_to_dot_sprite->getContentSize().width,
		_size_from_clear.height / _draw_to_dot_sprite->getContentSize().height);

		float t_s = _RTScale;
		scale = Vec2(t_s, t_s);

		#else

		Size size = _image->getContentSize();
		Vec2 scale = Vec2(size.width / _draw_to_dot_sprite->getContentSize().width,
		size.height / _draw_to_dot_sprite->getContentSize().height);
		#endif
		shader->setUniformVec2("_scale2", scale);
		*/
		if (_emergin_img->hasEmerging()) {
			_emergin_img->setVisible(false);
			shader->setUniformInt("_is_2_texture", 10);
			shader->setUniformTexture("_texture2_2", _emergin_img->getEmergingTex());
		}
		else {
			shader->setUniformInt("_is_2_texture", 0);
		}
		//###################
		if (NULL == _border) {
			//todo border not finded in canvas
			_border = dynamic_cast<RenderTexture*>(_layer->getChildByName(UI_dtd::NAME_CANVAS_NODE));
			_border->setVisible(false);
		}
		shader->setUniformTexture("_texture_border", _border->getSprite()->getTexture());
		_rt_to_cleared_sprite->setLocalZOrder(7);
		//#####################

		shader->setUniformVec2("_resolution", _size);
		shader->setUniformTexture("_particle", _rt_to_particle->getTexture());
		shader->setUniformFloat("_percent", percent);

		_rt_to_cleared_sprite->setVisible(true);
		//_rt_cleared_canvas->beginWithClear();
		_rt_cleared_canvas->beginWithClear();

		Sprite* sprite = _draw_to_dot_sprite;
		//--
		sprite->setFlippedY(true);
		sprite->setAnchorPoint(Vec2::ZERO);
		sprite->setGLProgramState(shader);
		sprite->visit();
		//--

		_rt_cleared_canvas->end();
	}
#endif
	//подсчёт времени програсса очистки
	/*
	if (test_press) {
	_percent_elapsed = .0;
	_rt_to_particle->beginWithClear();
	_rt_to_particle->end();
	}
	else
	*/
}

const float c_w = 50.f;
const float c_h = 40.f;

void FinishImage::update_sprite_particle(float percent, float delay) {

	if (_particle_cprite.empty()) {
		Texture2D* texture;
		if (_emergin_img->hasEmerging()) {
			_emergin_img->setVisible(false);
			texture = _emergin_img->getEmergingTex();
		}
		else {
			texture = _draw_to_dot_sprite->getTexture();
		}

		float width = texture->getContentSize().width;
		float height = texture->getContentSize().height;
		float di = width / c_w;
		float dj = height / c_h;
		for (float i = 0.f; i < width; i += di) {
			for (float j = 0.f; j < height; j += dj) {
				_particle_cprite.push_back(
					new ParticleSprite(_layer, _cpos, texture, Rect(i, j, di, dj), width)
				);
			}
		}
	}
	/*x
		if (_elapsed > .0) {
			_emergin_img->setVisible(false);
			shader->setUniformInt("_is_2_texture", 10);
			shader->setUniformTexture("_texture2_2", _emergin_img->getEmergingTex());
		}
		else {
			shader->setUniformInt("_is_2_texture", 0);
		}
		*/
		//return;
	//_rt_to_particle_sprite->setVisible(true);
	//_rt_to_particle_sprite->setLocalZOrder(999999);

	_rt_to_particle->beginWithClear();
	for (ParticleSprite* sprite : _particle_cprite)
		sprite->update(percent, delay);
	_rt_to_particle->end();
	//return;

	GLProgramState* shader = _shader_cleared_end->getGLProgramState();
	//###################
	if (NULL == _border) {
		//todo border not finded in canvas
		_border = dynamic_cast<RenderTexture*>(_layer->getChildByName(UI_dtd::NAME_CANVAS_NODE));
		_border->setVisible(false);
	}
	shader->setUniformInt("_hasBorder", !_emergin_img->hasEmerging() ? 1 : 0);
	shader->setUniformTexture("_texture_border", _border->getSprite()->getTexture());
	shader->setUniformVec2("_resolution", _size);
	shader->setUniformTexture("_texture_particle", _rt_to_particle->getTexture());
	shader->setUniformFloat("_percent", percent);

	_rt_to_cleared_sprite->setVisible(true);


	_rt_cleared_canvas->beginWithClear();

	Sprite* sprite = _draw_to_dot_sprite;
	//--
	sprite->setFlippedY(true);
	sprite->setAnchorPoint(Vec2::ZERO);
	sprite->setGLProgramState(shader);
	sprite->visit();
	//--


	_rt_cleared_canvas->end();
}

float _duration_fast = 1.5f;//если вышли из игры
float _duration_emerging = 3.f;//если прошли уровень

float _max_percent_fast = 1.2f;
float _max_percent_emerging = 1.7f;

void FinishImage::update(float delay) {
	if (!_isVisible) return;
	update_emerging(delay);

	if (!start_free_canvas) return;
	//CALL_VOID_METHOD(_emergin_img->_white_draw_node, setVisible(false));
	if (NULL == _draw_to_dot_sprite) return;
	_drawToDots->setVisible(false);

	float duration = _duration_fast;
	float max_percent = _max_percent_fast;
	if (_emergin_img->hasEmerging()) {
		duration = _duration_emerging;
		max_percent = _max_percent_emerging;
	}

	float t_percent = MIN(_elapsed / duration, max_percent);
	if (t_percent >= max_percent) {

#if 1 //from test - disable release
		reset();
		callback_to_end_clear();
		_rt_to_cleared_sprite->setVisible(false);
		CALL_VOID_METHOD(_border, setVisible(true));
		_border = NULL;
#endif
		_elapsed = .0;
		return;
	}

#if TYPE_PARTICE == 1
	update_shader_particle(t_percent, delay);
#else 
	update_sprite_particle(t_percent, delay);
#endif

	_elapsed += delay;
	Director::getInstance()->getRenderer()->render();
	//########################
}
