#include "stdafx.h"
#include "UI_canvas.h"
#include "Scenes/GamesCamera.h"
#include "DevelopMenu.h"

int UI_canvas::_backgroundZOreder = -1;
int UI_canvas::_selectLevelZOreder = 5;
int UI_canvas::_canvasZOrder = 10;
int UI_canvas::_buttonZOrder = 100;

const std::string UI_canvas::NAME_BACKGROUND_NODE = "background";
const std::string UI_canvas::NAME_CANVAS_NODE = "canvas";



#define DEF_S 160.f
#define FROM_PERCENT(vec2) Vec2(_size.width*vec2.x,_size.height*vec2.y)

#define C_PERCENT_POS .65f //позици€ ченральной части относительно размера

//def size ui 1920.f, 1080.f
const float def_width = 1920.f;
const float def_height = 1080.f;

UI_canvas::UI_canvas(Layer* layer):
	_layer(layer),
	_callback(NULL),
	_c(NULL) {
	_size = GamesCamera::Instance().getSize();
	_scale_ui = MAX(_size.width / def_width, _size.height / def_height);
}

UI_canvas::UI_canvas(Layer* layer, bool hasC /*= false*/):
	UI_canvas(layer) {
	//________________________
	_background = Sprite::create("krosh/background.png");
	_background->setName(NAME_BACKGROUND_NODE);
	_background->setScale(_scale_ui);
	_background->setPosition(_size*.5);

	layer->addChild(_background, _backgroundZOreder);
	//________________________
	Node* canvasNode = createCanvas(layer, _size, _scale_ui, hasC);
	if (hasC) {
		canvasNode->setPosition(-_background->getPosition());
		canvasNode->setName(NAME_CANVAS_NODE);
		layer->addChild(canvasNode, _canvasZOrder);
	}
	else {
		RenderTexture* canvac_rt = RenderTexture::create(_size.width, _size.height);
		canvac_rt->setAutoDraw(false);
		canvac_rt->beginWithClear(.0f, .0f, .0f, .0f);
		canvasNode->visit();
		canvac_rt->end();
		//	canvac_rt->setScale(scale);
		layer->addChild(canvac_rt, _canvasZOrder);
		canvac_rt->setName(NAME_CANVAS_NODE);
	}
	//________________________
	_bt_menu = createBtn(layer, _scale_ui, "button_menu");
	_bt_exit = createBtn(layer, _scale_ui, "button_close");
	_bt_next = createBtn(layer, _scale_ui, "button_left", true);
	_bt_prev = createBtn(layer, _scale_ui, "button_left");

	_bt_exit->setPosition(FROM_PERCENT(Vec2(.89f, .87f)));
	float dt_menu = (_bt_menu->getContentSize().width * _scale_ui) / _size.width;
	_bt_menu->setPosition(FROM_PERCENT(Vec2(MIN(.78f, .89f - dt_menu), .87f)));


	const float new_size = DEF_S * _scale_ui;
	float dt_next = ((_bt_next->getContentSize().width * _scale_ui) / _size.width) *1.1f;
	_bt_next->setPosition(FROM_PERCENT(Vec2(1. - dt_next, .5f)));
	_bt_prev->setPosition(FROM_PERCENT(Vec2(dt_next, .5f)));
#if 1//поправим сдвигом относительно камеры
#define TRANSLATE(node,_pos) node->setPosition(node->getPosition()+_pos)
	Vec2 dt = _size * -.5f;
	TRANSLATE(_background, dt);
	//TRANSLATE(canvasNode, dt);
	TRANSLATE(_bt_menu, dt);
	TRANSLATE(_bt_exit, dt);
	TRANSLATE(_bt_next, dt);
	TRANSLATE(_bt_prev, dt);
#undef TRANSLATE
#endif

	_inner_size = Size(_size.width - new_size, _size.height - new_size);

	DrawNode* tmp_dn = DrawNode::create();
	Vec2 from = Vec2::ZERO + (_size - _inner_size)*.5;
	Vec2 to = Vec2::ZERO + (_size + _inner_size)*.5;
	tmp_dn->drawRect(from, to, Color4F::RED);

	float d_top = new_size * .20;
	d_lh = new_size * .10;


	_inner_rect_l = Rect(from.x, from.y, (_inner_size.width)*C_PERCENT_POS, _inner_size.height - d_top);
	float width = (_inner_size - Size(_inner_rect_l.size.width, .0)).width;
	_inner_rect_r = Rect(_inner_rect_l.origin + Vec2(_inner_rect_l.size.width, .0),
						 Size(width, _inner_rect_l.size.height));

	tmp_dn->drawRect(_inner_rect_l.origin, _inner_rect_l.origin + _inner_rect_l.size, Color4F::GREEN);
	tmp_dn->drawRect(_inner_rect_r.origin, _inner_rect_r.origin + _inner_rect_r.size, Color4F::GRAY);

	//canvasNode->addChild(tmp_dn);

	if (hasC) {
		//обрезка внетрренней правой €чейки дополнительно - чуть пошире тобы под интрерфейс
		_clip_node_c->setClippingRegion(Rect(from.x, -def_height, _inner_size.width*(1.01), def_height*2.));

		_inner_rect_l.origin -= _size * .5;
		_inner_rect_r.origin -= _size * .5;
		{
			Vec2 wf = Vec2(.0, from.y);
			Vec2 wt = Vec2(.0, from.y) + _inner_rect_r.size*1.5;
			_white_r_layout = DrawNode::create();
			_white_r_layout->drawSolidRect(wf, wt, Color4F::WHITE);

			_white_r_layout->setAnchorPoint(Vec2::ZERO);
			ClippingRectangleNode* clip_node = ClippingRectangleNode::create(Rect(_inner_rect_r.origin, Size(_inner_rect_r.size.width*1.1, _inner_rect_r.size.height *1.)));
			clip_node->addChild(_white_r_layout);

			layer->addChild(clip_node, _canvasZOrder - 1);

			_d_pos_from_white_r_l = canvasNode->getPosition();
		}
		//_c->addChild(_white_r_layout, -100);
	}

#if HAS_DEVELOP

	/*
	static Vec2 next_pos = Vec2(.9f, .5f);
	DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, "next",
		([this](const Vec2& val) {_bt_next->setPosition(FROM_PERCENT(val)); next_pos = val; }),
		([this]() {return next_pos; }));

	static Vec2 prev_pos = Vec2(.9f, .5f);
	DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, "prev",
		([this](const Vec2& val) {_bt_prev->setPosition(FROM_PERCENT(val)); prev_pos = val; }),
		([this]() {return prev_pos; }));

	static Vec2 menu_pos = Vec2(.9f, .5f);
	DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, "menu",
		([this](const Vec2& val) {_bt_menu->setPosition(FROM_PERCENT(val)); menu_pos = val; }),
		([this]() {return menu_pos; }));

	static Vec2 exit_pos = Vec2(.9f, .5f);
	DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, "exit",
		([this](const Vec2& val) {_bt_exit->setPosition(FROM_PERCENT(val)); exit_pos = val; }),
		([this]() {return exit_pos; }));
	*/
	/*
	static float _canvasNodeScale = .8;
	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 1, DevelopTabs_Settings,
		"scale",
		([this](const float& val) {

		layout->removeChild(_canvasNode);
		_canvasNode = createCanvas(layout, _size, val);
		_canvasNodeScale = val;
	}),
			([this]() {return _canvasNodeScale; }), .0, 1., SeparatorType_Stroke);
			*/




			/*static float scaleY = ct_2->getScaleY();
			DevelopMenu::Instance().addFloatSettingFromMinMax(this, 1, DevelopTabs_Settings, "st_1",
				([this](const float& val) {
				d_pos = val;
				ct_2->setPosition(pos_ct_2 + Vec2(.0, ct_2->getContentSize().height*(val / 100.)));
			}),
				 ([this]() {return d_pos; }), .0, 100., SeparatorType_Stroke);
				 */

#if 0
	static Vec2 d_val = Vec2(.0, 00.);
	DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, "sp_ct_2",
		([this](const Vec2& val) {d_val = val;
	c->stopAllActions();
	c->setPosition(_pos_c + _pos_c);
	}),
		 ([this]() {return d_val; }));
#endif
#endif
}

void UI_canvas::setVisiblePanel(bool visible, bool force /*= false*/) {
	if (visible == _c->isVisible() && !force) return;

	_c->stopAllActions();
	_white_r_layout->stopAllActions();

	float _d_m = .25f;
	float _d_s = .15f;
	if (force) _d_m = _d_s = .0f;


#define set_action(_c, d_pos)																	   \
	if (visible) {																				   \
		_c->setScaleX(.0);																		   \
		_c->setPosition(Vec2(_size.width, .0f)+d_pos);											   \
		_c->setVisible(true);																	   \
																								   \
		Vector<FiniteTimeAction*> show_action;													   \
		show_action.pushBack(ScaleTo::create(_d_s, 1.f));										   \
		show_action.pushBack(MoveTo::create(_d_m, _pos_c + d_pos));								   \
		_c->runAction(Sequence::create(show_action));											   \
	}																							   \
	else {																						   \
		_c->setScaleX(1.);																		   \
		_c->setPosition(_pos_c + d_pos);															   \
		_c->setVisible(true);																	   \
																								   \
		Vector<FiniteTimeAction*> hide_action;													   \
		hide_action.pushBack(MoveTo::create(_d_m, Vec2(_size.width, .0f) + d_pos));				   \
		hide_action.pushBack(Sequence::create(													   \
			ScaleTo::create(_d_s, 0.f, 1.f, 1.f), NULL));										   \
		hide_action.pushBack(CallFunc::create([this](void) { _c->setVisible(false); }));		   \
		_c->runAction(Sequence::create(hide_action));											   \
		if (force)_c->setVisible(false);														   \
	}
	//DelayTime::create(_d_m - _d_s),														   \

	set_action(_c, Vec2::ZERO);
	set_action(_white_r_layout, _d_pos_from_white_r_l);
}

UI_canvas::~UI_canvas() {}

void AnimButton::setVisible(bool visible) {
	if (visible == _visible_from_process)return;
	setVisible(visible, .1f + CCRANDOM_0_1()*.2f, true);
}

void AnimButton::hide() {
	Node::setVisible(false);
}

void AnimButton::shake() {
	this->stopAllActions();

	Vector<FiniteTimeAction*> shakeActions;
	shakeActions.pushBack(DelayTime::create(.2 + .2*CCRANDOM_0_1()));
	shakeActions.pushBack(CallFunc::create([this](void) { Node::setVisible(true); Node::setScale(_prev_scale); }));

#define RAND(len,scale) (len + (len * scale)* CCRANDOM_0_1())
	float a1 = RAND(10., .25);
	float a2 = RAND(10., .25);

	shakeActions.pushBack(RotateBy::create(.2f, a1));
	shakeActions.pushBack(RotateBy::create(.2f, -a1 - a2));

	float a3 = RAND(5., .25);
	float a4 = RAND(5., .25);
	shakeActions.pushBack(RotateBy::create(.1f, a2 + a3));
	shakeActions.pushBack(RotateBy::create(.1f, -a3 - a4));

	shakeActions.pushBack(RotateTo::create(.05f, a4));
	this->runAction(Sequence::create(shakeActions));
}

void AnimButton::setVisible(bool visible, float delay, bool force) {
	if (!force && _visible_from_process == visible)return;
	_visible_from_process = visible;
	//if (force)
	this->stopAllActions();

	float sp1 = .15f;// .3f;
	float sp2 = .1f;// .2f;
	if (visible) {
		Vector<FiniteTimeAction*> movieActions;
		movieActions.pushBack(DelayTime::create(delay));
		movieActions.pushBack(CallFunc::create([this](void) { Node::setVisible(true); Node::setScale(.0); }));
		movieActions.pushBack(ScaleTo::create(sp1, _prev_scale * 1.1f));
		movieActions.pushBack(DelayTime::create(.1f));
		movieActions.pushBack(ScaleTo::create(sp2, _prev_scale));
		this->runAction(Sequence::create(movieActions));
	}
	else {
		Vector<FiniteTimeAction*> movieActions;
		movieActions.pushBack(DelayTime::create(delay));
		movieActions.pushBack(ScaleTo::create(sp2, _prev_scale * 1.1f));
		movieActions.pushBack(DelayTime::create(.1f));
		movieActions.pushBack(ScaleTo::create(sp1, .0f));
		movieActions.pushBack(CallFunc::create([this](void) { Node::setVisible(false); Node::setScale(.0); }));
		this->runAction(Sequence::create(movieActions));
	}
}

AnimButton* UI_canvas::createBtn(Node* layer, float scale, const std::string name, bool flipX/* = false*/) {
	AnimButton* btn = AnimButton::create(name + ".png", name + "_press.png");
	if (flipX) {
		btn->setFlippedX(true);
	}
	btn->setScale(scale);
	layer->addChild(btn, _buttonZOrder);

	btn->addTouchEventListener(CC_CALLBACK_2(UI_canvas::touchEvent, this));

	return btn;
}

Node* UI_canvas::createCanvas(Node* layer, const Size& size, float scale, bool hasC /*= false*/) {
	Node* ret = Node::create();
	ret->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	//layer->addChild(ret, _canvasZOrder);

	//const float def_s = 160.f;
	const float def_h = 760.f;
	const float def_w = 1600.f;
	const float new_size = DEF_S * scale;

	Sprite* lt = Sprite::create("canvas/LT.png");
	lt->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	lt->setPosition(Vec2(.0f, size.height));
	lt->setScale(scale);

	Sprite* l = Sprite::create("canvas/L.png");
	l->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	l->setPosition(Vec2(.0f, size.height*.5f));
	l->setScaleX(scale); l->setScaleY((size.height - new_size * 2.f) / def_h);

	Sprite* lb = Sprite::create("canvas/LB.png");
	lb->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	lb->setPosition(Vec2(.0f, .0f));
	lb->setScale(scale);

	Sprite* b = Sprite::create("canvas/B.png");
	b->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	b->setPosition(Vec2(size.width*.5, .0));
	b->setScaleX((size.width - new_size * 2.f) / def_w); b->setScaleY(scale);

	Sprite* rb = Sprite::create("canvas/RB.png");
	rb->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	rb->setPosition(Vec2(size.width, .0f));
	rb->setScale(scale);

	Sprite* r = Sprite::create("canvas/R.png");
	r->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	r->setPosition(Vec2(size.width, size.height*.5f));
	r->setScaleX(l->getScaleX()); r->setScaleY(l->getScaleY());

	Sprite* rt = Sprite::create("canvas/RT.png");
	rt->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	rt->setPosition(Vec2(size.width, size.height));
	rt->setScale(scale);

#define C_T(var,name)\
		Sprite* var = Sprite::create(name);\
		var->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);\
		var->setPosition(Vec2(size.width*.5f, size.height));\
		var->setScaleX(b->getScaleX()); var->setScaleY(b->getScaleY());

	C_T(t1, "canvas/T1.png");
	C_T(t2, "canvas/T2.png");

	//___________________
	if (hasC) {
		//const float def_h_cе = 192.f * scale;//высота центрального внутреннего столбика  - верхней или нижней части
		const float def_h_c = 1080.f;//высота центрального внутреннего столбика			 - центральной

		float pos_x = size.width*C_PERCENT_POS;
		Sprite* c_1 = Sprite::create("canvas/C1.png");
		c_1->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
		c_1->setPosition(Vec2(.0f, size.height));
		c_1->setScale(scale);


		Sprite* c_2 = Sprite::create("canvas/C2.png");
		c_2->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		c_2->setPosition(Vec2(.0f, size.height*.5));
		c_2->setScaleX(scale); c_2->setScaleY(size.height / def_h_c);


		Sprite* c_3 = Sprite::create("canvas/C3.png");
		c_3->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		c_3->setScale(scale);

		_pos_c = Vec2(pos_x, .0f);

		_c = Node::create();
		_c->setPosition(_pos_c);
		_c->addChild(c_1);
		_c->addChild(c_2);
		_c->addChild(c_3);

		_clip_node_c = ClippingRectangleNode::create();
		_clip_node_c->addChild(_c);
	}

	//___________________

	ret->addChild(lt);
	ret->addChild(l);
	ret->addChild(lb);
	ret->addChild(b);


	ret->addChild(t2);
	if (hasC)
		//ret->addChild(_c);
		ret->addChild(_clip_node_c);

	ret->addChild(t1);

	ret->addChild(rb);
	ret->addChild(rt);
	ret->addChild(r);


	//test
	//ret->addChild(c);

	return ret;
}

void UI_canvas::touchEvent(Ref*ref, Widget::TouchEventType eventType) {
	if (Widget::TouchEventType::ENDED == eventType)
		_callback((Button*)ref);
}
