#include "stdafx.h"
#include "PhysicsObject.h"
#include "DevelopMenu.h"
#include "../World/CreateWorld/PhysicsSpline.h"
#include "../World/CreateWorld/UI_cw.h"
//todo
//http://evidcom.blogspot.ru/2013/03/soft-body-physics-with-box2d-and.html

#define BUBBLE_NAME "bubbles/0B1BFB.png"

//размеры захардкожены
//сплайн один дл€ всех , и только загружаетс€(при создании нового будет пустой , сохран€ть созданный объект ненадо сплайн перепишетс€ на пустой)



//todo
//добавить сложность, 
//если шарик не в свою клетку, то исчезаем, пересоздаЄмс€ и выкидываем этот шарик занво
extern b2World* m_b2world;
extern bool m_b2dRecreate;
extern b2MouseJoint* m_mouseJoint;

#define USING_DRAW_NODE() {			\
if(!drawNode){						\
	drawNode = DrawNode2::create();	\
	_layer->addChild(drawNode,9999);}	\
drawNode->clear();					}


#define SET_DELAY_C_GRAVITY()  delay_change_gravity = CCRANDOM_0_1()*_delay_g

float _angle = 25.0;
PhysicsObject::PhysicsObject(Layer* layer):
	draw_rope(NULL),

	Soul::Soul(layer),
	_spline_down(false),
	_image_circle(NULL),
	_node_text(NULL),
	_stop_physics(false),
	_recreate_physics(false),
	_pos_spline(false),

	_active_to_stop(false),
	_active_check_on(false),
	_hasTemp(false),

	_enable_wind(false),
	_has_select(false),
	_has_finish(false),

	_callback_press(nullptr),
	_rt_rope(NULL),
	_rt_rope_Sprite(NULL),
	_rt_scale(2.),

	_let_obj(NULL) {

	d_gravity = .0;
	SET_DELAY_C_GRAVITY();
	_image = Sprite::create(BUBBLE_NAME);
	_image->setCascadeOpacityEnabled(true);
	_image_debug = Sprite::create(BUBBLE_NAME);
	_spline = new WorldSpline_and_Physics(1, DevelopTabs_World, false);

	//todo not edit spline
	_spline->setEnableSave(false);

	//int t_z = -1;
	//for (auto it : _layer->getChildren()) { t_z = MAX(t_z, it->getLocalZOrder()); }
	setZ(UI_cw::_canvasZOrder + 1);
	_layer->addChild(_image, getZ());
	_layer->addChild(_image_debug, getZ());

	_image_debug->setColor(Color3B::WHITE);
	//_image_debug->setOpacity(180);
	//_image_debug->setVisible(false);
#if HAS_DEVELOP
	DevelopMenu::Instance().changeDebugMode(this, ([this](const bool& enable) {
		_image_debug->setVisible(enable);
	}));
#else
	_image_debug->setVisible(false);
#endif
}

void PhysicsObject::setZ(int val) {
	Soul::setZ(val);
	_image->setLocalZOrder(val);
	_image_debug->setLocalZOrder(val);
}

bool PhysicsObject::select(bool isParent) {
#if HAS_DEVELOP
	if (!Soul::select(true))
		return false;
	_spline->changeEdits(true);


#define ADD_SETTINGF(name) \
{DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, #name,\
		( [this](const float& val){\
		DataSpline* t_data_spline = GET_DATA_SPLINE();\
		t_data_spline->name = val;\
		m_b2dRecreate = true;\
	} ),( [this](void){ return  GET_DATA_SPLINE()->name; } ));}

#define ADD_SETTINGI(name) \
{DevelopMenu::Instance().addUIntSettings(this, 0, DevelopTabs_World, #name,\
		( [this](const size_t& val){\
		DataSpline* t_data_spline = GET_DATA_SPLINE();\
		t_data_spline->name = val;\
		m_b2dRecreate = true;\
	} ),( [this](void){ return  GET_DATA_SPLINE()->name; } ));}

#define ADD_SETTINGP(name) \
{DevelopMenu::Instance().addPointSetting(this, 0, DevelopTabs_World, #name,\
		( [this](const Vec2& val){\
		DataSpline* t_data_spline = GET_DATA_SPLINE();\
		t_data_spline->name = val;\
		m_b2dRecreate = true;\
	} ),( [this](void){ return  GET_DATA_SPLINE()->name; } ));}

#if 0

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "_angle",
		([this](const float& val) { _angle = val; }),
											([this](void) { return  _angle; }));
	DevelopMenu::Instance().addSeparatorString(this, 0, DevelopTabs_World);
	ADD_SETTINGF(springiness);
	ADD_SETTINGF(damp_rat);
	DevelopMenu::Instance().addSeparatorString(this, 0, DevelopTabs_World);
	ADD_SETTINGI(m_count_w);
	ADD_SETTINGI(m_count_h);
	DevelopMenu::Instance().addSeparatorString(this, 0, DevelopTabs_World);
	ADD_SETTINGF(m_radiys_j);
	ADD_SETTINGF(m_sc_r);
	ADD_SETTINGP(d_c);


	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "delay_grav",
		([this](const float& val) { _delay_g = val; delay_change_gravity = .0; }),
											([this](void) { return  _delay_g; }));

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "max_d_g",
		([this](const float& val) { _max_d_g = val; delay_change_gravity = .0; }),
											([this](void) { return  _max_d_g; }));

	DevelopMenu::Instance().addBoolSetting(this, 0, DevelopTabs_World, "_enable_wind",
		([this](const bool& val) { _enable_wind = val; delay_change_gravity = .0; }),
										   ([this](void) { return  _enable_wind; }));
#endif
#if 0
	//### temp
	DevelopMenu::Instance().addPointSetting(this, 0, DevelopTabs_World, "d_pos_circle",
		([this](const Vec2& pos) { setDPosCircle(pos, _circle_r); }),
											([this](void) { return  getDPosCircle(); }));

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "circle_r",
		([this](const float& val) { setDPosCircle(getDPosCircle(), val); }),
											([this](void) { return  _circle_r; }));
	//###
	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "scale",
		([this](const float& val) {setScale(val); }), ([this](void) {return getScale(); })
	);

	DevelopMenu::Instance().addPointSetting(this, 0, DevelopTabs_World, "pos_start_unit",
		([this](const Vec2& pos) { setParamUnit(pos, _count_unit, _d_unit, _size_unit, true); }),
											([this](void) { return  _pos_start_unit; }));

	DevelopMenu::Instance().addUIntSettings(this, 0, DevelopTabs_World, "count_unit",
		([this](const uint& val) { setParamUnit(_pos_start_unit, val, _d_unit, _size_unit, true); }),
											([this](void) { return  _count_unit; }));

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "d_unit",
		([this](const float& val) { setParamUnit(_pos_start_unit, _count_unit, val, _size_unit, true); }),
											([this](void) { return  _d_unit; }));

	DevelopMenu::Instance().addPointSetting(this, 0, DevelopTabs_World, "size_unit",
		([this](const Vec2& val) { setParamUnit(_pos_start_unit, _count_unit, _d_unit, val, true); }),
											([this](void) { return  _size_unit; }));

	//-------------------------------- 

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "density",
		([this](const float& val) { setParamUnit(_pos_start_unit, _count_unit, _d_unit, _size_unit, true); _density_unit = val; }),
											([this](void) { return  _density_unit; }));

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "friction",
		([this](const float& val) { setParamUnit(_pos_start_unit, _count_unit, _d_unit, _size_unit, true); _friction_unit = val; }),
											([this](void) { return  _friction_unit; }));

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_World, "_d_angle_unit",
		([this](const float& val) { setParamUnit(_pos_start_unit, _count_unit, _d_unit, _size_unit, true); _d_angle_unit = val; }),
													  ([this](void) { return  _d_angle_unit; }), 0, 180);

	//-------------------
	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_World, "_frequencyHz",
		([this](const float& val) { setParamUnit(_pos_start_unit, _count_unit, _d_unit, _size_unit, true); _frequencyHz = val; }),
													  ([this](void) { return  _frequencyHz; }), 0, 100);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_World, "_dampingRatio",
		([this](const float& val) { setParamUnit(_pos_start_unit, _count_unit, _d_unit, _size_unit, true); _dampingRatio = val; }),
													  ([this](void) { return  _dampingRatio; }), 0, 100);
#endif
	if (!isParent)
		PhysicsObject::refresh();
#endif
	return true;
}

void PhysicsObject::deselect() {
#if HAS_DEVELOP
	Soul::deselect();
	_spline->changeEdits(false);
#endif
}

void PhysicsObject::refresh() {
#if HAS_DEVELOP
	if (!isSelected)
		return;
	DevelopMenu::Instance().refresh(this);
	//Soul::refresh();
#endif
}

PhysicsObject::~PhysicsObject() {
	//CCLOG("~PhysicsObject");
	if (drawNode)drawNode->removeFromParent();
	_image_debug->removeFromParent();
	_image->removeFromParent();
	if (_rt_rope)
		_rt_rope->release();
	if (draw_rope)
		draw_rope->release();
	deselect();
	delete _spline;

	if (m_b2world) {
		for (auto& it : _body_rope)
			m_b2world->DestroyBody(it);
		//for (auto& it : _join_rope)
		//    m_b2world->DestroyJoint(it);
	}
	//CCLOG("~PhysicsObject_end");
}

bool PhysicsObject::mouseDown(const Vec2 & mousePos) {
	if (!isEdit)return false;
	if (isSelected) {
		if (_spline_down = _spline->mouseDown(mousePos))
			return true;
	}
	return Soul::mouseDown(mousePos);
}

bool PhysicsObject::mouseUp(const Vec2 & mousePos) {
	if (!isEdit)return false;
	if (!isSelected) return false;
	_spline_down = false;
	bool ret = _spline->mouseUp(mousePos);
	if (!ret) {
		ret = Soul::mouseUp(mousePos);
	}
	return ret;
}

bool PhysicsObject::mouseMovie(const Vec2 & mousePos) {
	if (!isEdit)return false;
	if (_spline_down) {
		return _spline->mouseMovie(mousePos);
	}
	bool ret = Soul::mouseMovie(mousePos);
	if (!ret)
		ret = _spline->mouseMovie(mousePos) && ret;
	return ret;
}

void PhysicsObject::toEnd(PhysicsObject* end_box, bool success) {
	std::vector<TSpline*> t_splines = _spline->getPolygons();

	Sprite* end_image = end_box->getImage();

	MathUtils::SetAnchorFromSavePosition_2(_image, end_image->getAnchorPoint());

	_image->runAction(MoveTo::create(.8f, end_image->getPosition()));
	_image->runAction(RotateTo::create(1.f, .0f));

	_rt_rope_Sprite->runAction(FadeOut::create(.3f));
	_image->setLocalZOrder(getZ() - 1);

	if (success) {
		_stop_physics = true;
		end_box->setAStopPhysics(true);
		_has_finish = true;
	}
	else {
		_stop_physics = true;
		//_recreate_physics = true;
		setPos(end_box->getPos());

		Vector<FiniteTimeAction*> t_actions;
		t_actions.pushBack(DelayTime::create(1.2));
#define R_POS() RAND_F(-.5,.5)
		for (int i = 8; i > 0; i--)
			t_actions.pushBack(MoveTo::create(.0 + i * .01, end_box->getPos() + Vec2(R_POS(), R_POS())));

		t_actions.pushBack(CallFunc::create([this](void) {
			_recreate_physics = true;
		}));

		_image->runAction(Sequence::create(t_actions));

		_image->setZOrder(getZ());
	}

	if (m_mouseJoint) {
		m_b2world->DestroyJoint(m_mouseJoint); m_mouseJoint = NULL;
	}
}

void PhysicsObject::play_anim_hide(float max_delay) {

	_image->stopAllActions();
	if (_image->getScaleX() == .0 || !_image->isVisible()) return;

	float delay = RAND_F(.1f, MAX(.11, max_delay)) - .1;
	_image->runAction(Sequence::create(
		DelayTime::create(delay),
		ScaleTo::create(.1, .0),
		nullptr));
}

void PhysicsObject::play_anim_translate(bool left) {
	_stop_physics = true;
	_hasTemp = false;
	_image->runAction(Spawn::create(
		MoveTo::create(RAND_F(.5, 1.), _image->getPosition() + (left ? (-1.) : (+1.))* Vec2(RAND_F(120., 200.), RAND_F(-30., 30.))),
		RotateTo::create(RAND_F(.5, 1.), RAND_F(100., 600.)),
		nullptr));
}

void PhysicsObject::play_anim(float delay, bool start) {
	float s_def_x = _image->getScaleX();
	float s_def_y = _image->getScaleY();

	float s_in = MAX(s_def_x, s_def_y)*.25;
	_image->runAction(Sequence::create(
		ScaleTo::create(.0,
						start ? s_def_x : .0,
						start ? s_def_y : .0, 1.),

		DelayTime::create(delay),
		CallFunc::create([this](void) { _image->setVisible(true); }),

		ScaleTo::create(.3, s_def_x + s_in, s_def_y + s_in, 1.),
		ScaleTo::create(.3, s_def_x - s_in, s_def_y - s_in, 1.),

		ScaleTo::create(.2, s_def_x + s_in * .5, s_def_y + s_in * .5, 1.),
		ScaleTo::create(.2, s_def_x - s_in * .5, s_def_y - s_in * .5, 1.),

		ScaleTo::create(.1, s_def_x + s_in * .25, s_def_y + s_in * .25, 1.),
		ScaleTo::create(.1, s_def_x - s_in * .25, s_def_y - s_in * .25, 1.),
		nullptr));
}

void PhysicsObject::setEndBox(PhysicsObject* let) {
	_let_obj = let;
	_let.clear();
	_image->initWithFile("opacity_bubble.png");
	_image->setLocalZOrder(getZ() - 1);
	setSize(_width, _height);

	if (_image_circle) { _image_circle->removeFromParentAndCleanup(true); _image_circle = NULL; }
	if (_node_text) { _node_text->removeFromParentAndCleanup(true); _node_text = NULL; }

	DataSpline* t_data = getDataSpline();
	t_data->setType(DSType_EndBox);
	//t_data->destroy_physics();
	//___
	_image->setVisible(false);
	play_anim(RAND_F(1.4, 2.4), false);
}

void PhysicsObject::setBubble(const std::string& let, const Color4B& color_let, const std::string& image_name) {
	_let_obj = NULL;
	_let = let;// "–†—Т";//–Р
	_color_let = color_let;// Color4B(16, 200, 146, 255);
	if (!image_name.empty())
		_image->initWithFile("bubbles/" + image_name + ".png");
	_image->setLocalZOrder(getZ());
	setSize(_width, _height);

	setDPosCircle();

	DataSpline* data_obj = getDataSpline();
	data_obj->setType(DSType_Bubble);
	data_obj->setFilter_b();

	_hasTemp = true;
	_active_check_on = true;
	//delay_change_gravity = .0;
	//_enable_wind = true;
}

void PhysicsObject::setTemplate(bool hasActive, float delay) {
	_hasTemp = true;
	//not active
	//_template_delay = delay;
	_active_check_on = hasActive;

	DataSpline* data_obj = getDataSpline();
	data_obj->setFilter_out_b();

	//DataSpline* data_obj_in = TO_DATA_SPLINE(_spline_in->getPolygons()[0]->getData());
	//data_obj_in->setFilter_out_b();
	//_________
	delay_change_gravity = .0;

	//_enable_wind = true;
}

void PhysicsObject::active_to_button(const on_press_release& callback) {
	_has_press = false;
	_join_len_activate = false;
	_callback_press = callback;
	_image->setVisible(false);
	play_anim(RAND_F(1., 1.5), false);
}
//todo test only
void PhysicsObject::setParam() {
	_let = "–†—Т";//–Р
	_color_let = Color4B(16, 200, 146, 255);
	_image->initWithFile("bubbles/0CFB8F.png");
	setSize(_width, _height);
}

//DrawNode* _in_circle = NULL;
void PhysicsObject::setDPosCircle() {
	_image_debug->setOpacity(180);

	_circle_r = 222.462;
	_d_pos_circle = Vec2(.0f, 106.8f);

	Vec2 t_pos = Vec2(_d_pos_circle) + _image->getContentSize()*.5;

#if 1
	if (!_image_circle)
		_image->addChild(_image_circle = Sprite::create("bubble_circle.png"), 2);

	_image_circle->setScale(_circle_r / 113.f);
	_image_circle->setPosition(t_pos);
#endif

#if 1
	if (_node_text)_node_text->removeFromParentAndCleanup(true);

	///222.462 -> def to 135.f
	float font_size = 240.*(_circle_r / 135.f);
	_node_text = Label::createWithTTF(_let, "fonts/CirceBold.ttf",
									  font_size,
									  Size::ZERO, TextHAlignment::CENTER,
									  TextVAlignment::CENTER);
	_node_text->setTextColor(_color_let);

	_image->addChild(_node_text, 3);
	_node_text->setPosition(t_pos);
#endif

#if 0
	if (_in_circle) {
		_in_circle->clear();
	}
	else {
		_image->addChild(_in_circle = DrawNode::create(), 1);
	}

	_in_circle->setPosition(t_pos);
	_in_circle->drawCircle(Vec2::ZERO, _circle_r, 360., 100, false, Color4F(Color3B(16, 200, 146)));
#endif
}

void PhysicsObject::setScale(const float& scale) {
	_scale = scale;

	//setSize(50.f, 71.f);//старые  нулевые значени€ сетки дл€ world spline
	setSize(50.f*_scale, 71.f*_scale);

	_spline->load("bubble.xml", getPos(), _scale - _scale * RAND_F(.1f, .2f));
	//_spline->load("bubble.xml", Vec2::ZERO, _scale);
}

void PhysicsObject::Load(SettingUtil* setting) {
	Soul::Load(setting);
	//setSize(50.f, 71.f);//старые  нулевые значени€ сетки дл€ world spline
	//7x10

	_stop_physics = false;
	setScale(setting->getFloatForKey("_scale", 1.));//устанавливаем размер и перезагружаем
	//_spline->load("bubble.xml", getPos(), _scale);

	setParam();//цвет размер и картинка
	setDPosCircle();//рисуем всЄ что надо в центре шарика


	//костыль дл€ позиции сплайна и картинки (при загрузке надо раздельно применить )
	_pos_spline = true;

	_pos_start_unit = setting->getVec2ForKey("_pos_start_unit");
	_count_unit = setting->getIntegerForKey("_count_unit", 0);
	_d_unit = setting->getFloatForKey("_d_unit");
	_size_unit = setting->getVec2ForKey("_size_unit");

	_density_unit = setting->getFloatForKey("_density_unit", 5.f);
	_friction_unit = setting->getFloatForKey("_friction_unit", .2f);
	_d_angle_unit = setting->getFloatForKey("_d_angle_unit", 45.f);

	_frequencyHz = setting->getFloatForKey("_frequencyHz", 8.f);
	_dampingRatio = setting->getFloatForKey("_dampingRatio", .5f);

	setParamUnit(_pos_start_unit, _count_unit, _d_unit, _size_unit);
	//setDefEnableMouse(setting->getBoolForKey("enableMouse"));
	//______	
	setZ(UI_canvas::_canvasZOrder - 1);
}

SettingUtil* PhysicsObject::Save() {

	SettingUtil* setting = Soul::Save();

	setting->setVec2ForKey("_pos_start_unit", _pos_start_unit);
	setting->setIntegerForKey("_count_unit", _count_unit);
	setting->setFloatForKey("_d_unit", _d_unit);
	setting->setVec2ForKey("_size_unit", _size_unit);


	setting->setFloatForKey("_density_unit", _density_unit);
	setting->setFloatForKey("_friction_unit", _friction_unit);
	setting->setFloatForKey("_d_angle_unit", _d_angle_unit);

	setting->setFloatForKey("_frequencyHz", _frequencyHz);
	setting->setFloatForKey("_dampingRatio", _dampingRatio);

	//setSize(50.f*_scale, 71.f*_scale);
	_scale = getWidth() / 50.f;
	setting->setFloatForKey("_scale", _scale);

	_spline->save(true, getPos(), _scale);
	return setting;
}

void PhysicsObject::setPos(const Vec2 & pos) {
	if (_pos_spline) {
		Vec2 prevPos = _image_debug->getPosition();
		Vec2 d_pos = prevPos - pos;
		_spline->setDPos(d_pos);
		getDataSpline()->setSPos(pos);

		//_spline_in->setDPos(d_pos);
		//TO_DATA_SPLINE(_spline_in->getPolygons()[0]->getData())->setSPos(pos);
	}
	Soul::setPos(pos);
	_image_debug->setPosition(pos);
}

void PhysicsObject::setAStopPhysics(bool stop) {
	_has_finish = true;
	_active_to_stop = stop;
	_image->runAction(FadeOut::create(3.));
}

void PhysicsObject::check_action_physics(b2Body* body) {

	Vec2 t_realPos = _image->getPosition(); //todo check pos body in _image
	Vec2 size = Vec2(_width * .5, _height * 2.);;
	Rect contains_rect = Rect(_curr_rect.origin + size, _curr_rect.size - Size(size.x*2., size.y*1.));

	if (_hasTemp && _active_check_on && contains_rect.containsPoint(t_realPos)) {
		_active_check_on = _hasTemp = false;
		_enable_wind = true;
		DataSpline* data_obj = TO_DATA_SPLINE(_spline->getPolygons()[0]->getData());
		data_obj->setFilter_b();

		for (auto& it : _body_rope) {
			for (b2Fixture* fixture = it->GetFixtureList(); fixture != NULL; fixture = fixture->GetNext()) {
				fixture->SetFilterData(data_obj->getFilter());
			}
		}
		//DataSpline* data_obj_in = TO_DATA_SPLINE(_spline_in->getPolygons()[0]->getData());
		//data_obj_in->setFilter_b();

		setZ(UI_canvas::_canvasZOrder + 1);
		//data_obj->resetDC(); 
		return;
	}
	//__________________________________
	if (_image->getOpacity() <= .01) {
		_stop_physics = true;
	}
	//__________________________________
	if (
		t_realPos.y >= (_curr_rect.origin.y + _curr_rect.size.height*1.5)
		) {
		/*
		if (_active_to_stop) {
			_stop_physics = true;
		}
		else*/
			{
				b2Vec2 pos = body->GetPosition();

				float mi_dx = -90 - t_realPos.x;
				float ma_dx = 26. - t_realPos.x;
				b2Vec2 d_pos = b2Vec2(RAND_F(mi_dx, ma_dx), -140.);
				body->SetTransform(pos + d_pos, body->GetAngle());
				for (auto& it : _body_rope) {
					it->SetTransform(it->GetPosition() + d_pos, it->GetAngle());
				}
			}
	}

	if (!_hasTemp) {
		//    _image->setColor(Color3B::BLACK);
	}
}

void PhysicsObject::drawFromPhysics() {
	if (_let_obj) {
		_image->setPosition(_image_debug->getPosition());
		return;
	}

	DataSpline* t_data_spline = getDataSpline();
	if (NULL == t_data_spline)return;

	if (_recreate_physics) {
		_recreate_physics = false;
		setBubble(_let, _color_let, "");
		createPhysics(true);

		b2Body* t_body = t_data_spline->getBody();
		Vec2 t_force = Vec2(.0, t_body->GetMass()*200.);
		t_force.rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(RAND_F(70., 120.)));
		t_body->ApplyForceToCenter(to_b2Vec2(t_force), true);
		//t_body->ApplyLinearImpulse(to_b2Vec2(t_force), b2Vec2(.0, .0), true);
		return;
	}
	else if (_stop_physics) {
		if (hasSelect() && m_mouseJoint) {
			m_b2world->DestroyJoint(m_mouseJoint);
			m_mouseJoint = NULL;
		}
		t_data_spline->destroy_physics();
		for (auto& it : _body_rope)m_b2world->DestroyBody(it);
		_join_rope.clear();
		_body_rope.clear();
		return;
	}

	b2Body* body = t_data_spline->getBody();
	if (!body) return;//// destroy physics

	b2Vec2 tp = body->GetWorldCenter();
	Vec2  t_realPos = Vec2(tp.x, tp.y);
	float t_angle = MATH_RAD_TO_DEG(-body->GetAngle());

	_image->setPosition(t_realPos);
	_image->setRotation(t_angle);

	check_action_physics(body);
	if (_stop_physics || _recreate_physics) drawFromPhysics();
}

FromSpline PhysicsObject::drawSmootLine(const Vec2& t_p, const Vec2& t_c, const Vec2& t_n) {
	_smoot_percent = 1.;

	Vec2 norm = (t_n - t_p).getNormalized();
	float t_angle = Vec2::angle(t_p, t_n);
	t_angle = Vec2::angle(t_p - t_c, t_n - t_c);
	t_angle = 1. - MATH_RAD_TO_DEG(t_angle) / 180.;
	//180 - 0
	//90 - 50
	//0 - 100

	//next
	float t_nl = t_c.distance(t_n) * _smoot_percent * t_angle;
	Vec2 c2 = t_c + norm * t_nl;
	//--prev
	float t_pl = t_c.distance(t_p) * _smoot_percent * t_angle;
	Vec2 c1 = t_c - norm * t_pl;

	return FromSpline(t_c, c1, c2);
}

void PhysicsObject::updateWind(float delta) {
	if (_let_obj || _stop_physics)return;

	if (!_enable_wind)return;
	//_____________
	if (_has_select) {
		DataSpline* data_obj = getDataSpline();
		b2Body* body = data_obj->getBody();
		body->SetAwake(true);

		body->SetGravityScale(data_obj->getGScale());
		return;
	}
	//_____________
	delay_change_gravity -= delta;
	if (delay_change_gravity <= 0) {
		SET_DELAY_C_GRAVITY();
		//_enable_wind = false;
		float t_d_g = RAND_F(.0f, _max_d_g);
		d_gravity = d_gravity >= .0 ? -1 * t_d_g : t_d_g;

		std::vector<TSpline*> t_splines = _spline->getPolygons();
		DataSpline* data_obj = TO_DATA_SPLINE(t_splines[0]->getData());
		b2Body* body = data_obj->getBody();
		body->SetAwake(true);

		float defg = data_obj->getGScale();
		if (_join_rope.empty()) {
			defg = .0;
			d_gravity *= .5;
		}
		if (!_active_check_on)
			body->SetGravityScale(defg + d_gravity);
	}
}

void PhysicsObject::update(float delta) {
	_has_select = false;
	if (m_mouseJoint) {
		b2Body* body_select = m_mouseJoint->GetBodyB();
		_has_select = getDataSpline()->getBody() == body_select;
		if (!_has_select) {
			for (auto& it : _body_rope) {
				if (body_select == it) {
					_has_select = true;
					break;
				}
			}
		}
		_join_len_activate = (m_mouseJoint->GetAnchorB() - m_mouseJoint->GetTarget()).Length() <= 2.;
		//если есть callback провер€ем на первое трогание и увеличиваем
		if (_callback_press && !_has_press && _has_select) {
			_has_press = true;
			s_def = Vec2(_image->getScaleX(), _image->getScaleY());
			Vec2 scale_to = s_def + s_def * .3f;
			_image->stopAllActions();
			_image->runAction(ScaleTo::create(.2f, scale_to.x, scale_to.y));
		}
	}
	//если есть callback то провер€ем на отпускание и запуск
	if (_callback_press && _has_press && !m_mouseJoint) {
		_has_press = false;
		if (_join_len_activate) {
			_stop_physics = true;
			_join_len_activate = false;

			_image->stopAllActions();
			_rt_rope_Sprite->runAction(FadeOut::create(.2f));
			_image->runAction(Spawn::create(
				Sequence::create(
					RotateBy::create(.35f, 360.f),
					RotateBy::create(.25f, 360.f),
					RotateBy::create(.15f, 360.f),
					RotateBy::create(.1f, 360.f),
					RotateBy::create(.05f, 360.f),
					nullptr),
				ScaleTo::create(1.f, .0f, .0f), nullptr));

			_callback_press(this);
		}
		else {
			_image->runAction(ScaleTo::create(.1f, s_def.x, s_def.y));
		}
	}
	//_________________________
	updateWind(delta);
	//дл€ рисовки точек todo remove in release
	_spline->setScaleFromDraw(1. / _layer->getScale());

	if (delta > .0)drawFromPhysics();
	//update intersection
#if 0
#if 0
	bool has_action_to_finish = false;
	if (m_mouseJoint) {
		DataSpline* data_join = TO_DATA_SPLINE(m_mouseJoint->GetBodyB()->GetUserData());
		if (data_join && DataSplineType::DSType_Bubble == data_join->getType()) {
			has_action_to_finish = true;
		}
	}
#endif
	//добавить пересечение с несвоими буквами
	while (!_stop_physics && _let_obj && hasSelect())//если финишный квадрат - (у нас есть буква котора€ нужна)
	{
		std::vector<TSpline*> t_splines = _let_obj->getSpline()->getPolygons();
		DataSpline* data_obj = TO_DATA_SPLINE(t_splines[0]->getData());
		b2Body* b = data_obj->getBody();
		if (b) {
			//USING_DRAW_NODE();
			/**/
			//___________
			{
				// _let_obj litera
				const b2Transform& xf = b->GetTransform();
				for (b2Fixture* fixture = b->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
					b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
					Vec2 A, B;
					bool second = false;
					for (int32 i = 0; i < poly->m_count - 1; ++i) {
						if (second) {
							A = B;
						}
						else {
							second = true;
							A = to_Vec2(b2Mul(xf, poly->m_vertices[i]));
						}
						B = to_Vec2(b2Mul(xf, poly->m_vertices[i + 1]));


						//drawNode->drawLine(*pos_b0, *pos_b1, Color4F::BLACK);
						//___________
						{
							//this end box
							vector<Vertex*> t_vertexs = _spline->getPolygons()[0]->getSpline();
							for (vector<Vertex*>::iterator it = t_vertexs.begin(), it_e = t_vertexs.end(); it != it_e; ++it) {
								Vec2 C = ((*it)->getPos());
								if (!(*it)->getHead())continue;
								Vec2 D = (*it)->getHead()->getPos();
								//drawNode->drawLine(pos_e0, pos_e1, Color4F::RED);
								//______________
								if (Vec2::isSegmentIntersect(A, B, C, D)) {
									_let_obj->toEnd(this, true);

									_image->runAction(FadeOut::create(.8));
									return;
								}
							}
							//_________________________________
						}
					}
				}
			}
#if 0
			//if(!drawNode)
			if (false) {
				drawNode = DrawNode2::create();
				for (vector<Vec2>::iterator it = t_end.begin(), it_e = t_end.end();; ++it) {
					if (it + 1 == it_e)break;
					drawNode->drawLine((*it), *(it + 1), Color4F::BLACK);
				}

				for (vector<Vec2>::iterator it = t_bybble.begin(), it_e = t_bybble.end();; ++it) {
					if (it + 1 == it_e) {
						drawNode->drawLine((*it), *(t_bybble.begin()), Color4F::RED);
						break;
					}
					drawNode->drawLine((*it), *(it + 1), Color4F::RED);
				}
				_layer->addChild(drawNode);
			}
			//drawNode->drawPolygon(t_end, Color4F(.0, .0, .0, .0), 2., Color4F::BLACK);
			//drawNode->drawPolygon(t_bybble, Color4F(.0, .0, .0, .0), 2., Color4F::RED);
#endif
			//__________________
		}
		break;
	}
#endif
	drawRope(delta);
}

void PhysicsObject::drawRope(float delta) {
	if (_join_rope.empty())return;// || delta <= .0)return;
	std::vector<Vec2> t_controls;
	bool first = true;
	for (std::list<b2Joint*>::iterator it = _join_rope.begin(), it_e = _join_rope.end(); it != it_e; ++it) {
		b2Vec2 p1 = (*it)->GetAnchorA();
		b2Vec2 p2 = (*it)->GetAnchorB();
		//t_controls.push_back(to_Vec2(p1));
		//continue;
		if (first) {
			first = false;
			//t_controls.push_back(to_Vec2(p2));
			//t_controls.push_back(to_Vec2(p1));

			t_controls.push_back(to_Vec2(p1));
			t_controls.push_back(to_Vec2(p2));
			continue;
		}
		//t_controls.push_back(to_Vec2(p1));
		t_controls.push_back(to_Vec2(p2));
	}
	if (!_body_rope.empty()) {
		b2Vec2 last_pos = _body_rope.back()->GetPosition();
		t_controls.push_back(to_Vec2(last_pos));
	}

	b2Vec2 t_pos = _body_rope.front()->GetPosition();
	Vec2 t_d_pos = to_Vec2(t_pos)*-1.;
	//Vec2 t_d_pos = _image->getPosition()*-1.;
	Vec2 t_p0;
	Vec2 t_n0;
	first = true;
	vector<FromSpline> tc_spline;
	for (std::vector<Vec2>::iterator it = t_controls.begin(), it_e = t_controls.end(); it != it_e; it++) {
		Vec2 t_c = *it;
		//_____________ convert to rt coord 
		t_c = MathUtils::ConvertToNodeSpace_ignoreRS(_rt_rope, t_c + t_d_pos) + _d_pos_rt;
		//thisPos += _cpos;// +_image->getPosition();
		//______________
		if ((it + 1) == it_e || first) {
			first = false;
			tc_spline.push_back(FromSpline(t_c, t_c, t_c));
			t_p0 = t_c;
			continue;
		}
		else {
			t_n0 = *(it + 1);
			t_n0 = MathUtils::ConvertToNodeSpace_ignoreRS(_rt_rope, t_n0 + t_d_pos) + _d_pos_rt;
		}
		tc_spline.push_back(drawSmootLine(t_p0, t_c, t_n0));
		t_p0 = t_c;
	}

	if (!draw_rope) {
		draw_rope = DrawNode::create();
		draw_rope->retain();
	}
	draw_rope->clear();
	draw_rope->setLineWidth(2);
	//to do two curcle
	for (vector<FromSpline>::iterator it = tc_spline.begin(), it_e = tc_spline.end(); ; ++it) {
		if ((it + 1) == it_e) {
			break;
		}
		FromSpline fspline_0 = (*(it));
		FromSpline fspline_1 = (*(it + 1));

		draw_rope->drawCubicBezier(fspline_0._pos,
								   fspline_0._c2, fspline_1._c1,
								   fspline_1._pos, 5, Color4F::BLACK);
	}
	tc_spline.clear();

#if 1
	_rt_rope->beginWithClear();
	draw_rope->visit();
	_rt_rope->end();
	Director::getInstance()->getRenderer()->render();
#endif
	//_rt_rope_Sprite->setPosition(_image->getPosition());
	_rt_rope_Sprite->setRotation(-_image->getRotation());
}

void PhysicsObject::setParamUnit(const Vec2& pos, int count, float d, const Vec2& size, bool recreate /*= false*/) {
	_pos_start_unit = pos;
	_count_unit = count;
	_d_unit = d;
	_size_unit = Vec2(MAX(size.x, .1), MAX(size.y, .1));

	if (recreate) {
		m_b2dRecreate = true;
	}
}

extern float scale_ui;
void PhysicsObject::createPhysics(bool recreate) {
	_stop_physics = false;
	//if (_let_obj) return;
	std::vector<TSpline*> t_splines = _spline->getPolygons();
	WorldSpline_and_Physics::CreateFixture(t_splines, recreate);

	if (t_splines.size() <= 0)return;
	DataSpline* data_obj = TO_DATA_SPLINE(t_splines[0]->getData());
	if (!recreate && (!data_obj || !data_obj->getBody())) {
		WorldSpline_and_Physics::CreateFixture(t_splines, true);
		data_obj = TO_DATA_SPLINE(t_splines[0]->getData());
	}
	data_obj->setParent(this);
	//if(!data_obj) return;
	b2Body* t_body = data_obj->getBody();

	//Vec2 size = Vec2(getWidth(), getHeight());
	//Vec2 size = _image->getContentSize()*_scale;
	Vec2 size = Vec2(_width, _height);
	Vec2 anchor = (to_Vec2(data_obj->getBody()->GetWorldCenter()) - _image_debug->getPosition());
	anchor = Vec2(anchor.x / size.x, anchor.y / size.y);

	_image->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_image->setPosition(_image_debug->getPosition());

	MathUtils::SetAnchorFromSavePosition_2(_image, Vec2::ANCHOR_MIDDLE + anchor);

	_body_rope.clear();
	_join_rope.clear();
	update(.0);
	//return;
	if (_hasTemp && !_active_check_on)return;
	if (_let_obj)return;
	//if (_hasTemp) _count_unit = 1;
	//-----------------------------------------------
#if 1
	//b2PolygonShape shape;
	Vec2 t_size_unit = _size_unit * getScale();
	//__
	//_body_rope.clear();
	//_join_rope.clear();
	Vec2 t_rope_star = _pos_start_unit * getScale();
	float t_lenght_rope = t_size_unit.y*.5;//after inc lenght
	{
		//shape.SetAsBox(t_size_unit.x*.5, t_lenght_rope);
		b2CircleShape shape;
		shape.m_radius = t_lenght_rope;

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = _density_unit;// 5.0f;
		fd.friction = _friction_unit;// 0.2f;
		//fd.filter = data_obj->getFilter();
		fd.filter = data_obj->getFilter_out_b();


		b2RevoluteJointDef jd;
		jd.collideConnected = true;
		//jd.enableLimit = true;


		Vec2 t_start_p = _image_debug->getPosition() + t_rope_star;
		//const float32 y = 25.0f;
		b2Body* prevBody = t_body;
		b2Vec2 prevAnchor(t_start_p.x, t_start_p.y - t_size_unit.y*.5);
		//prevAnchor = t_body->GetWorldCenter();
		//b2Vec2 prevAnchor = b2Vec2(data_obj->getCentr().x, data_obj->getCentr().y);// b2Vec2();
		for (int32 i = 0; i++ <= _count_unit; ) {
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(t_start_p.x, t_start_p.y);// -t_size_unit.y*.5);

			b2Body* body = m_b2world->CreateBody(&bd);
			body->CreateFixture(&fd);

			//body->SetUserData(data_obj);
			_body_rope.push_back(body);
			//-------------------------
			//body->SetUserData(t_splines[0]->getData());
			//-------------------------
			b2Vec2 anchor = bd.position;
			anchor = b2Vec2(t_start_p.x, t_start_p.y);
			jd.Initialize(prevBody, body, anchor);
			jd.lowerAngle = MATH_DEG_TO_RAD(_d_angle_unit)*-.5;// -5.235987901687622e-01f;
			jd.upperAngle = MATH_DEG_TO_RAD(_d_angle_unit)*.5;// 5.235987901687622e-01f;

			//i <= 1 ? jd.enableLimit = true : jd.enableLimit = false;

			b2Joint* t_joint = m_b2world->CreateJoint(&jd);
			//_join_rope.push_back(t_joint);
			//if(false)
			{
				b2DistanceJointDef djd;
				djd.Initialize(prevBody, body, prevAnchor, anchor);

				djd.collideConnected = true;
				djd.frequencyHz = _frequencyHz;// 8.000000000000000e+00f;
				djd.dampingRatio = _dampingRatio;// 5.000000000000000e-01f;

				t_joint = m_b2world->CreateJoint(&djd);
				_join_rope.push_back(t_joint);
			}

			prevAnchor = anchor;
			prevBody = body;

			t_start_p += Vec2(0, -t_size_unit.y - _d_unit * getScale());
		}
		t_lenght_rope = ABS(t_lenght_rope + (t_start_p - (_image_debug->getPosition() + t_rope_star)).y);
	}
	check_action_physics(t_body);
#endif
	//________________________________________________
	_rt_scale = 10. / scale_ui;
	_rt_size = Vec2(t_lenght_rope, t_lenght_rope)*2.;
	_rt_size = _rt_size * _rt_scale;

	_d_pos_rt = _rt_size * .5;

	float in_scale = 1. / _rt_scale;

	if (_rt_rope) _rt_rope->release();
	_rt_rope = EffectRenderTexture::create(_rt_size.x, _rt_size.y, Texture2D::PixelFormat::RGBA8888, 0);
	//_rt_rope = RenderTexture::create(_rt_size.x, _rt_size.y, Texture2D::PixelFormat::RGBA8888, 0);
	_rt_rope->setScale(in_scale);
	_rt_rope->retain();

	Texture2D* t_texture = _rt_rope->getSprite()->getTexture();
	if (_rt_rope_Sprite) _rt_rope_Sprite->removeFromParent();
	_rt_rope_Sprite = Sprite::createWithTexture(t_texture);
	_rt_rope_Sprite->setScale(in_scale);
	_rt_rope_Sprite->setFlippedY(true);

	_rt_rope_Sprite->setOpacity(255);
	/*
	DrawNode* rect = DrawNode::create(3);
	rect->setContentSize(Size(_rt_size));
	rect->drawSolidRect(Vec2::ZERO, _rt_size, Color4F(.0, .0, .0, .2));
	_rt_rope_Sprite->addChild(rect);
	/**/

	_rt_rope_Sprite->setPosition(_image_debug->getPosition());
	//_layer->addChild(_rt_rope_Sprite, getZ());
	_image->addChild(_rt_rope_Sprite, 4);

	_rt_rope_Sprite->setScaleX(in_scale / (_width / _image->getContentSize().width));
	_rt_rope_Sprite->setScaleY(in_scale / (_height / _image->getContentSize().height));
	//_rt_rope_Sprite->setScaleX(in_scale / _image->getScaleX());
	//_rt_rope_Sprite->setScaleY(in_scale / _image->getScaleY());


	if (false) {
		//Vec2 anchor_sprite = Vec2(anchor.x*size.x, anchor.y*size.y) - t_rope_star;
		Vec2 anchor_sprite = -t_rope_star;
		anchor_sprite = Vec2(anchor_sprite.x / _rt_size.x, anchor_sprite.y / _rt_size.y) * _rt_scale;

		//MathUtils::SetAnchorFromSavePosition_2(_rt_rope_Sprite, Vec2::ANCHOR_MIDDLE + anchor_sprite);
	}
	else {
		Vec2 t_d_pos = _image->getContentSize()*.5;
		_rt_rope_Sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		_rt_rope_Sprite->setPosition(Vec2(t_d_pos.x, .0) - t_rope_star);
	}

}

extern GLESDebugDraw* m_debugDraw;

#include "ExtendedNode/DrawNode2.h"

void PhysicsObject::draw() {
	//drawRope(.0);
#if HAS_DEVELOP
	if (!DevelopMenu::Instance().isEnable())return;
	_spline->draw();
#endif
	return;
#if 0
	//todo many cyrcle
	//#############################
	if (_join_rope.empty())return;
	std::vector<Vec2> t_controls;
	bool first = true;
	for (std::list<b2Joint*>::iterator it = _join_rope.begin(), it_e = _join_rope.end(); it != it_e; ++it) {
		b2Vec2 p1 = (*it)->GetAnchorA();
		b2Vec2 p2 = (*it)->GetAnchorB();
		if (first) {
			first = false;
			t_controls.push_back(to_Vec2(p2));
			t_controls.push_back(to_Vec2(p1));
			continue;
		}
		//t_controls.push_back(to_Vec2(p1));
		t_controls.push_back(to_Vec2(p2));
	}

	Vec2 t_p0;
	Vec2 t_n0;
	first = true;
	vector<FromSpline> tc_spline;
	for (std::vector<Vec2>::iterator it = t_controls.begin(), it_e = t_controls.end(); it != it_e; it++) {
		Vec2 t_c = *it;
		if ((it + 1) == it_e || first) {
			first = false;
			tc_spline.push_back(FromSpline(t_c, t_c, t_c));
			t_p0 = t_c;
			continue;
		}
		else {
			t_n0 = *(it + 1);
		}
		tc_spline.push_back(drawSmootLine(t_p0, t_c, t_n0));
		t_p0 = t_c;
	}

	for (vector<FromSpline>::iterator it = tc_spline.begin(), it_e = tc_spline.end(); ; ++it) {
		if ((it + 1) == it_e) {
			break;
		}
		FromSpline fspline_0 = (*(it));
		FromSpline fspline_1 = (*(it + 1));


		glLineWidth(4);
		DrawColor(Color4B::BLACK);
		if (false) {
			//__
			ccDrawLine(fspline_0._pos, fspline_0._c2);
			ccDrawCircle(fspline_0._c2, 8, 360, 20, false);
			//__
			ccDrawLine(fspline_1._pos, fspline_1._c2);
			ccDrawCircle(fspline_1._c1, 8, 360, 20, false);
			//__
		}
		ccDrawCubicBezier(fspline_0._pos,
						  fspline_0._c2, fspline_1._c1,
						  fspline_1._pos, 20);
	}
	tc_spline.clear();


#else
	glLineWidth(2);
	for (auto* joint : _join_rope) {

		b2Body* bodyA = joint->GetBodyA();
		b2Body* bodyB = joint->GetBodyB();
		const b2Transform& xf1 = bodyA->GetTransform();
		const b2Transform& xf2 = bodyB->GetTransform();
		b2Vec2 x1 = xf1.p;
		b2Vec2 x2 = xf2.p;
		b2Vec2 p1 = joint->GetAnchorA();
		b2Vec2 p2 = joint->GetAnchorB();

		m_debugDraw->DrawSegment(p1, p2, b2Color(0, 0, 1));
	}
	//glLineWidth(2);
	//return;
	for (auto* body : _body_rope) {

		const b2Transform& xf = body->GetTransform();
		for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
			//DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.3f));

			b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
			int32 vertexCount = poly->m_count;
			b2Assert(vertexCount <= b2_maxPolygonVertices);
			b2Vec2 vertices[b2_maxPolygonVertices];

			for (int32 i = 0; i < vertexCount; ++i) {
				vertices[i] = b2Mul(xf, poly->m_vertices[i]);
			}

			m_debugDraw->DrawSolidPolygon(vertices, vertexCount, b2Color(1, 0, 1));

		}
	}
#endif
};
