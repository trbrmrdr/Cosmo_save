#include "stdafx.h"
#include "DrawToDots.h"
#include "DevelopMenu.h"

#include "WorldSpline/TSpline.h"
#include "Brush.h"

#pragma warning (push)
#pragma warning (disable:4305)

const bool isTestFastEnd = false;


#if IS_PC || IS_IOS
#define  LOGD(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "DrawToDots"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif


Vec2 startP = IONE;
Vec2 movieP = IONE;
Vec2 endP = IONE;

std::vector<Brush*> _brushs_end;
Button* buttonWObj;

std::vector<Brush*> _brushs_1;
std::vector<Brush*> _brushs_2;
std::vector<Brush*> _brushs_3;

std::vector<Brush*> _brushs_helper;

int currPos = 0;
bool isStartCurrPos = false;
bool isCurrPos = false;
bool isNextPos = false;
bool isErr = false;
float incBrush;
float distanceEdgeBrush;
float m_brush_width;

float _durationFromTo = 1.f;
float _delayFromEnd = .5f;
float m_brush_opacity = 210.f;

#if 0//first test
Size  m_size = Size(4961.f, 3508.f);
#ifdef IS_PC

float dContainsR = 50;// 22;// 50;
float dDrawR = 32;

float mRTScale = .5f;	// =>4961.f, 3508.f
float m_brush_scale = 2.f;	// =>32

float scaleDistance = .25;// .25f; // 
float scaleInc = .125;// .125f;
#else
float dContainsR = 200;
float dDrawR = 50;

float mRTScale = .09f;
//float m_brush_scale = .5f;
float m_brush_scale = .3f;

float scaleDistance = .4;// .25f; // 
float scaleInc = .3;// .125f;
#endif
#else

//testing success
//float mRTScale = 1.;				//scale для размера  текстуры в которую будет рисоватся
//Size  m_size = Size(1730.f, 910.f);//размер в пикселях для игрового поля





float mRTScale = 1.;
#if 1
float scaleDistance = .25f;
float scaleInc = .125f;
#else
float scaleDistance = 1.5f;
float scaleInc = 2.f;
#endif
#endif




float m_helper_amplitude = .0f;// 10.f;//скорость пунктира
float m_helper_space = .6f;//длинна пробела (в зависимости от количества)

float m_helper_duration = 1.5f;
float m_helper_delay = .01f;//задержка I того элемента пунктира

#if 1
double t_delay_startHelper = 2.;
uint t_count_from_helper = 2;

#else
double t_delay_startHelper = .0;
uint t_count_from_helper = 10;
#endif


DrawNode* testDraw = NULL;

#define CREATE_RT(val,size,scale) \
{val = EffectRenderTexture::create(size.width, size.height, Texture2D::PixelFormat::RGBA8888);\
val->retain();\
val->setScale(1.f/scale);\
val->setPosition(Vec2::ZERO);\
val->setAutoDraw(false);\
}


#define BEGIN_RT_1() \
if (!isClearRT_1) { \
	isClearRT_1 = true; \
	m_rt_1->beginWithClear(); \
}

#define END_RT_1() \
if (isClearRT_1){ \
m_rt_1->end(); \
}

Vec2 pos_td = Vec2(-512, -384);
int  d_len = 138;
int count_spline = 5;
DrawNode* testDraw_na = NULL;
DrawNode* testDraw_a = NULL;
int start_i = 0;
int end_i = 2;
bool show_dif = true;

DrawToDots::DrawToDots(Node* layer, float scale_ui, const Size& inner_size):
	_layer(layer),
	_c_pos_rt_texture(),
	_last_end_helper(-1),
	WorldSpline(1, DevelopTabs_Spline, true),
	isEndGame(false),
	isEndGameAnim(false),
	_need_save_texture(0),
	hasStartGame(false),
	_callback_render_texture(nullptr),
	_setting(NULL),
	_point_texture(NULL),
	mLastAdd(0) {
	//_______________________
	_isChanged = false;
	_maxCountSplines = 1;
	_isVisible = true;
	_fileName.clear();
	//_______________________
#if 1//
	m_scale = scale_ui - .1;
	m_size = inner_size;
#else // only test
	m_scale = scale_ui-.1;
	m_size = VisibleRect::getVisibleRect().size;
#endif

	setZ(1);
	Size rt_size = m_size * mRTScale;

	//todo add white image
	CREATE_RT(m_rt_0, rt_size, mRTScale);
	m_rt_0->beginWithClear(1.f, 1.f, 1.f, 1.f);
	m_rt_0->end();
	_layer->addChild(m_rt_0, getZ());

	m_back_img = Sprite::create();
	m_back_img->retain();
	m_back_img->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	m_back_img->setScale(m_scale);
	_layer->addChild(m_back_img, getZ() + 1);

	CREATE_RT(m_rt_1, rt_size, mRTScale);
	_layer->addChild(m_rt_1, getZ() + 3);

	CREATE_RT(m_rt_2, rt_size, mRTScale);
	_layer->addChild(m_rt_2, getZ() + 2);

	_c_pos_rt_texture = m_rt_1->getTexture()->getContentSize()*.5f;

	Size vRect = VisibleRect::getVisibleRect().size;
	float rt_toEnd_scale = 1.f; //.5f;
	CREATE_RT(m_rt_toEnd, vRect, rt_toEnd_scale);

	// create a brush image to draw into the texture with
	m_brush_sprite = Sprite::create("brush_sprite.png");
	m_brush_sprite->retain();
	//brush->setColor(Color3B::RED);
	m_brush_sprite->setOpacity(m_brush_opacity);
	setBrushSize();
	//_______________________
	m_converter_coords = ([this](const Vec2& point) {return (mRTScale == 1.f) ?
						  point + _c_pos_rt_texture :
						  MathUtils::ConvertToNodeSpace_ignoreRS(m_rt_1, point) + _c_pos_rt_texture; });
#if HAS_DEVELOP
	/*
	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings, "m_helper_delay",
		([this](const float& val) {
		m_helper_delay = val;
		reset();
	}),
		 ([this]() { return m_helper_delay; }), .0f, 10.f, SeparatorType_Bottom, .1f
		);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings, "m_helper_duration",
		([this](const float& val) {
		m_helper_duration = val;
		reset();
	}),
		 ([this]() { return m_helper_duration; }), .0f, 10.f, SeparatorType_Bottom, .1f
		);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings, "m_helper_amplitude",
		([this](const float& val) {
		m_helper_amplitude = val;
		reset();
	}),
		 ([this]() { return m_helper_amplitude; }), .0f, 300.f, SeparatorType_Bottom, .5f
		);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings, "m_helper_space",
		([this](const float& val) {
		m_helper_space = val;
		reset();
	}),
		 ([this]() { return m_helper_space; }), .0f, 5.f, SeparatorType_Bottom, .1f
		);

	DevelopMenu::Instance().addUIntSettings(this, 0, DevelopTabs_Settings, "t_count_from_helper",
		([this](const size_t& val) {
		t_count_from_helper = val;
		reset();
	}),
		 ([this]() { return t_count_from_helper; }), SeparatorType_Bottom
		);
	/**/

	/*
#define CLEAR_TD()	\
if (NULL != testDraw_na)testDraw_na->removeFromParent(); testDraw_na = NULL;\
if (NULL != testDraw_a)testDraw_a->removeFromParent(); testDraw_a = NULL;

	DevelopMenu::Instance().addUIntSettings(this, 0, DevelopTabs_Settings, "start_i",
		([this](const size_t& val) {
		start_i = MIN(end_i - 1, val);
		CLEAR_TD();
	}),
		 ([this]() { return start_i; }), SeparatorType_Bottom
		);

	DevelopMenu::Instance().addUIntSettings(this, 0, DevelopTabs_Settings, "end_i",
		([this](const size_t& val) {
		end_i = MAX(val, start_i + 1);
		CLEAR_TD();
	}),
		 ([this]() { return end_i; }), SeparatorType_Bottom
		);

	DevelopMenu::Instance().addUIntSettings(this, 0, DevelopTabs_Settings, "count_spline",
		([this](const size_t& val) {
		count_spline = val == 0 ? -1 : val;
		CLEAR_TD();
	}),
		 ([this]() { return count_spline == -1 ? 0 : count_spline; }), SeparatorType_Bottom
		);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings, "d_len",
		([this](const float& val) {
		d_len = val < 1. ? -1.f : val;
		CLEAR_TD();
	}),
		 ([this]() { return d_len == -1 ? 0.f : d_len; }), .0f, 200.f, SeparatorType_Bottom, .5f
		);


	DEVELOP_ADD_S_BOOL(0, DevelopTabs_Settings, "show_dif", show_dif);
	/**/

	/*
	DEVELOP_ADD_SEPARATOR(0, DevelopTabs_Settings, SeparatorType::SeparatorType_Bottom, "DrawToDots");


	DevelopMenu::Instance().addColorSetting(this, 0, DevelopTabs_Settings,
											"color_yes",
											([this](const Color3B& color) { setColorYes(color, true); }),
											([this](void) { return  getColorYes(); }));

	DevelopMenu::Instance().addColorSetting(this, 0, DevelopTabs_Settings,
											"color_circle_start",
											([this](const Color3B& color) { setColorCircleStart(color, true); }),
											([this](void) { return  getColorCircleStart(); }));


	DevelopMenu::Instance().addColorSetting(this, 0, DevelopTabs_Settings,
											"color_text",
											([this](const Color3B& color) { setColorText(color, true); }),
											([this](void) { return  getColorText(); }));

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings,
													  "dContainsR",
													  ([this](const float& val) { _dContainsR = val; setBrushSize(); }),
													  ([this](void) { return _dContainsR; }), 5.f, 100.f);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings,
													  "dDrawR",
													  ([this](const float& val) { setParam(_dContainsR, val, _brush_scale); resetColor(); }),
													  ([this](void) { return _dDrawR; }), 1.f, 20.f);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings,
													  "brush_scale",
													  ([this](const float& val) { _brush_scale = val; setBrushSize(); }),
													  ([this](void) { return _brush_scale; }), .1f, 2.f);
	*/
	DevelopMenu::Instance().changeDebugMode(this, ([this](const bool& val) { if (!val)saveSetting(); }));


	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "m_brush_scale", m_brush_scale);
	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "scaleDistance", scaleDistance);
	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "scaleInc", scaleInc);
#endif
}

void DrawToDots::saveSetting() {
	if (NULL == _setting)return;
	_setting->setColor3BForKey("_color_yes", getColorYes());
	_setting->setColor3BForKey("_color_circle_start", getColorCircleStart());
	_setting->setColor3BForKey("_color_text", getColorText());

	_setting->setFloatForKey("_dContainsR", _dContainsR);
	_setting->setFloatForKey("_dDrawR", _dDrawR);
	_setting->setFloatForKey("_scaleBr", _brush_scale);
	//_setting->save();
}

void DrawToDots::save(bool force /*= false*/, const Vec2& d_pos/* = Vec2::ZERO*/, const float& scale /*= 1.f*/) {
	WorldSpline::save(force, d_pos, m_scale);
	saveSetting();
}

void DrawToDots::load(const std::string& levelName, const Vec2& d_pos/* = Vec2::ZERO*/, const float& scale /*= 1.f*/) {
	_setting = new SettingUtil(levelName + ".setting");
	setColorYes(_setting->getColor3BForKey("_color_yes"));
	setColorCircleStart(_setting->getColor3BForKey("_color_circle_start"));
	setColorText(_setting->getColor3BForKey("_color_text"));

#if defined(IS_PC)
	float scale_param = 1.f;
#else
	//float _dContainsR = 8;// 25;// 50;
	//float _dDrawR = 2.5;// 8;
	//float _brush_scale = .25f;    // =>32

	//setParam(20, 5, .5);
	float scale_param = 2.f * MIN(1.,m_scale*2. );
#endif
	//scale_param = 2.f;
	setParam(
		_setting->getFloatForKey("_dContainsR") * scale_param,
		_setting->getFloatForKey("_dDrawR")* scale_param,
		_setting->getFloatForKey("_scaleBr")* scale_param
	);

	reset();
	WorldSpline::load(levelName, d_pos, m_scale);
	//WorldSpline::load("test_spline.xml", d_pos, m_scale);
}

bool DrawToDots::changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState) {
#if HAS_DEVELOP
	if (WorldSpline::changeKeys(keyCode, keyState))return true;
	//if (!DevelopMenu::Instance().isEnable())return false;
	bool ret = false;
	switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_Z:
			if (keyState == KeyState_RELEASE) {
				finishGame();
				ret = true;
			}
			break;

		case EventKeyboard::KeyCode::KEY_R:
			if (keyState == KeyState_RELEASE) {
				reset();
				ret = true;
			}
			break;
			/*
			case EventKeyboard::KeyCode::KEY_T:
			if (keyState == KeyState_RELEASE) {
			setBrushSize();
			ret = true;
			}
			break;
			/**/
	}
	return ret;
#endif
	return false;
}

void DrawToDots::finishGame() {
	isEndGameAnim = false;
	isEndGame = true;
}

void DrawToDots::saveTexture(const callback_render_texture& callback) {
	if (_need_save_texture != 0)return;
	_need_save_texture = 1;
	_callback_render_texture = callback;
	m_rt_toEnd->setEndCallback([this](void*data) {
		_callback_render_texture(data);

		_layer->setPosition(Vec2::ZERO);
		_layer->setScale(1.);
		setVisible(false);
	});

}

void DrawToDots::setVisible(bool visible, bool force /*= false*/) {
	if (visible == _isVisible && !force)return;

	_isVisible = visible;

	reset();

	if (_isVisible) {
		m_back_img->setVisible(true);
		m_back_img->setOpacity(0);

		m_back_img->runAction(
			Sequence::create(
				DelayTime::create(1.5 + rand_0_1()*.5),
				FadeTo::create(.7, 255), nullptr
			));
	}

	int i = 0;
	for (Vertex* p1 : getControlAll()) {
		if (p1->hasAssistant())continue;
		i++;
		//############ text
		//printText(_layer, p1, i, _recreateLabel);
		Node* parent = p1->node;
		if (_isVisible) {
			float delay = .5 + rand_0_1()*2.;
			//___________________________________________
			Node* nodeText = printText(_layer, p1, i, true);

			//Node* nodeText = parent->getChildByName("text");
			nodeText->stopAllActions();
			nodeText->setOpacity(0);

			nodeText->runAction(
				Sequence::create(
					DelayTime::create(delay + 1.f),
					FadeTo::create(.7, 255), nullptr
				));
			//continue;
			//____________________________________________
			//незнаю зачем - наверное для редактора
			/*
			if (NULL == parent) {
			drawCircle(_layer, p1, true);
			parent = p1->node;
			}
			parent->setPosition(p1->getPos());
			*/
			Sprite* nodeCircle = parent->getChildByName<Sprite*>(NAME_CHILD_CIRCLE);

			nodeCircle->setColor(getColorCircleStart());
			nodeCircle->setOpacity(0);
			nodeCircle->stopAllActions();

			nodeCircle->setScale(_scale_node_circle);

			Vector<FiniteTimeAction*> opacityActions;
			Vector<FiniteTimeAction*> movieActions;


			opacityActions.pushBack(DelayTime::create(delay));
			opacityActions.pushBack(FadeTo::create(.4, 255));

			movieActions.pushBack(DelayTime::create(delay));
			movieActions.pushBack(ScaleTo::create(.2, _scale_node_circle*1.4));
			movieActions.pushBack(DelayTime::create(.2));
			movieActions.pushBack(ScaleTo::create(.2, _scale_node_circle));

			nodeCircle->runAction(
				Spawn::create(
					Sequence::create(opacityActions),
					Sequence::create(movieActions),
					nullptr
				)
			);
		}
		else {
			if (NULL != parent) {
				Node* nodeText = parent->getChildByName(NAME_CHILD_TEXT);
				CALL_VOID_METHOD(nodeText, removeFromParent());
				//______________________________
				Node* nodeCircle = parent->getChildByName(NAME_CHILD_CIRCLE);
				CALL_VOID_METHOD(nodeCircle, removeFromParent());
				//nodeCircle->stopAllActions();
				//nodeCircle->setOpacity(0);
				//nodeCircle->runAction(FadeTo::create(.3, 0));
				/**/
			}
		}
	}
}

void DrawToDots::reset() {
	_last_change_currPos = HelperUtils::GetCurrTime() + 7.;//задержка на появление текста точек
	hasStartGame = false;
	_need_save_texture = 0;

	isEndGameAnim = false;
	isEndGame = false;
	currPos = 0;
	m_clearRT = true;
	m_back_img->setVisible(false);

	freeBrushs(_brushs_1, true);
	freeBrushs(_brushs_2, true);
	freeBrushs(_brushs_3, true);
	freeBrushs(_brushs_helper, true);

	startP = movieP = IONE;
	isCurrPos = isStartCurrPos = false;

	if (hasSpline())
		for (Vertex* v1 : getControlAll()) {
			drawCircle(_layer, v1, false, true, 2);
		}

	if (NULL != testDraw)
		testDraw->clear();
}

void DrawToDots::setBrushSize() {
	m_brush_sprite->setScale(_brush_scale);

	m_brush_width = m_brush_sprite->getTexture()->getContentSize().width * _brush_scale;
	distanceEdgeBrush = m_brush_width * scaleDistance;
	incBrush = m_brush_width * scaleInc;
}

void DrawToDots::setParam(float _d_contain_r, float _d_draw_w, float _scale_br) {
	if (_d_contain_r > .0 && _d_draw_w > .0 && _scale_br > .0) {
		_dContainsR = _d_contain_r;
		_dDrawR = _d_draw_w;
		_brush_scale = _scale_br;

		if (NULL == _point_texture)
			_point_texture = Director::getInstance()->getTextureCache()->addImage("point_1.png");
		_scale_node_circle = _dDrawR / (_point_texture->getContentSize().width*.25);
	}
	setBrushSize();
}

DrawToDots::~DrawToDots() {
	m_rt_0->release();
	m_rt_1->release();
	m_rt_2->release();
	m_rt_toEnd->release();

	m_back_img->release();

	m_brush_sprite->release();

	CC_SAFE_DELETE(_setting);
}

void DrawToDots::select(bool force /*= false*/) {
	if (_isAllocated_prev == _isAllocated)
		return;
	deselect();
	if (_isAllocated == NULL)
		return;
	_isAllocated_prev = _isAllocated;
	WorldSpline::select(true);
#if HAS_DEVELOP
	DevelopMenu::Instance().addBoolSetting(this, 1, DevelopTabs_Spline, "setIsAssistant",
		([this](const bool& val) {
		_isAllocated->setAssistant(val);
		setChangedSpline(true);
	}),
		 ([this](void) {
		return _isAllocated->hasAssistant();

	}), SeparatorType_None);
	/*

	DevelopMenu::Instance().addBoolSetting(
	this, 1,
	DevelopTabs_Spline, "Invert Orientation",
	([this](const bool& clear) {
	if (NULL!=_isEditableSpline)
	_isEditableSpline->recreteSpline(true);
	}),
	([this](void) { return false; }),
	SeparatorType_Bottom);

	const std::vector<std::string> namesType{"Point", "Reflected", "Free", "Only C1", "Only C2"};

	edit_isVertexType = DevelopMenu::Instance().addSelectItem(
	this, 1,
	DevelopTabs_Spline, "Vertex type", namesType,
	([this](const size_t& item) {
	_isAllocated->setType((TypeVertex)item);
	setChangedSpline(true);
	}),
	([this](void) {
	return (size_t)_isAllocated->getType();
	})
	, SeparatorType_Bottom
	);
	*/
#endif
}

void DrawToDots::update(float delta) { //if isLast delta ==0
#if 1
	if (NULL == testDraw) {
		testDraw = DrawNode::create();
		//_layer->addChild(testDraw);
		testDraw->retain();
	}
#endif
#if 0
	//from test 
	if (!m_file_name_to_back.empty()) {
		Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(m_file_name_to_back);
		if (!texture) {
			texture = Director::getInstance()->getTextureCache()->addImage(m_file_name_to_back);
		}
		if (texture) {
			m_file_name_to_back.clear();
			m_rt_0->begin();
			Sprite* t_sprite = Sprite::createWithTexture(texture);
			t_sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

			Vec2 t_new_pos = m_rt_0->getTexture()->getContentSize();
			t_sprite->setPosition(t_new_pos*.5);
			t_sprite->visit();

			m_rt_0->end();
		}
	}
	//end from test
#endif

	if (_need_save_texture > 0) {
		_layer->setPosition(Vec2::ZERO);
		if (_need_save_texture == 1) {
			_need_save_texture = 2;
			m_rt_toEnd->beginWithClear();

			Rect vRect = VisibleRect::getVisibleRect();
			Vec2 pos = vRect.size - m_size;

			_layer->setPosition(pos*.5+m_size*.5);
			_layer->visit();

			m_rt_toEnd->end();

			m_rt_toEnd->saveToFile("test.png");
		}
		else {
			_need_save_texture = 0;
			_layer->setPosition(Vec2::ZERO);
			_layer->setScale(1.);

			m_rt_1->clear();
			m_rt_2->clear();
		}
		Director::getInstance()->getRenderer()->render();
		return;
	}
	//
	//очищать кисти только в начале    иначе рендер в текстуру будет рисовать  удалённые спрайты !!!!!!!!!!!!!!!!!!!!!!!
	freeBrushs(_brushs_helper);
	freeBrushs(_brushs_1);
	freeBrushs(_brushs_2);

	freeBrushs(_brushs_end, true);// кисти которые рисуются 1 раз, они на своём месте (готовые)
								  //##################################
	if (m_clearRT) {
		m_rt_1->clear();
		m_rt_2->clear();
		freeBrushs(_brushs_end, true);
		m_clearRT = false;

		if (NULL != testDraw) {
			testDraw->clear();
		}
		return;
	}
	if (!_isVisible) return;
#if 1//начало блока обработки кистей на сплайне
	bool isClearRT_1 = false;

	bool t_isVisit = delta > .0f;
	if (t_isVisit && !_brushs_1.empty() || !_brushs_2.empty()) {
		BEGIN_RT_1();
	}
	//######################################################################## 1
	//если ошибка запускаем анимацию окончания 
	//новых и ожидающих кистей
	if (isErr) {
		endBrush(_brushs_1);
		endBrush(_brushs_2);
	}
	{
		uint i = 0;
		//новые кисти сортируем по
		//правильным 
		//остальные запускаем на исчезновение
		int t_start_i = _brushs_1.size() - mLastAdd;
		for (ITBRUSH it = _brushs_1.begin(), it_e = _brushs_1.end(); it_e != it; ) {
			Brush* brush = (*it);
			if (mLastAdd > 0 && i < t_start_i) {
				it++; i++;
				continue;
			}
			if (brush->isNew() &&
				(isCurrPos || isNextPos) && !isErr
				) {
				brush->setType(Brush::BrushType::YES);
				_brushs_2.push_back(brush);
				it = _brushs_1.erase(it);
				it_e = _brushs_1.end();
				continue;
			}
			if (t_isVisit) {
				brush->update(delta);
				brush->visit();

				endBrush(brush);
			}
			it++; i++;
		}
	}
	//######################################################################## 2
	//ожидающие кисти
	//добавляем в готовые 
	//и запускаем анимацию места
	//
	int t_count_new_brush_3 = 0;
	{
		bool end = _brushs_2.empty()
			|| ((isErr || !isNextPos)
				&& !t_isVisit);
		for (ITBRUSH it = _brushs_2.begin(), it_e = _brushs_2.end(); !end;) {
			it_e--;
			end = it_e == it;
			Brush* brush = (*it_e);
			if (!isErr && isNextPos && !brush->hasStartToEnd()) {
				t_count_new_brush_3++;

				brush->setType(Brush::BrushType::FINISH);

				_brushs_3.push_back(brush);
				it_e = _brushs_2.erase(it_e);
			}
			if (t_isVisit) {
				brush->update(delta);
				brush->visit();
			}
		}
	}
	if (isNextPos) {
		isNextPos = false;
		isCurrPos = true;
	}
	//######################################################################## 3
	//готовые кисти анимируем на место
	//если пришли добавляем во временный список готовых(который чистим со следующим update)
	{
		if (!_brushs_3.empty() || 0 != t_count_new_brush_3) {
			int t_start_i = _brushs_3.size() - t_count_new_brush_3;
			BEGIN_RT_1();
			Vec2* segment = NULL;
			if (0 != t_count_new_brush_3 && currPos > 0) {

#if 0
				segment = createSegments(currPos - 1, currPos, &t_count_new_brush_3, NULL, true);

#else
				float len_segment = .0f;

				Vec2* not_align_segment = createSegments(currPos - 1, currPos, &t_count_new_brush_3, &len_segment, false);

				float dt_min = distanceEdgeBrush * .25;
				int align_count_new_brusuh_3 = t_count_new_brush_3;
				segment = TSpline::aliginSegments(not_align_segment, &align_count_new_brusuh_3, len_segment, distanceEdgeBrush*.5);

				int dt = align_count_new_brusuh_3 - t_count_new_brush_3;
				//CCLOG("from->to %i:%i", currPos - 1, currPos);
				if (dt > 0 && dt > dt_min) {
					CC_SAFE_DELETE_ARRAY(not_align_segment);
					float inc = (float)t_count_new_brush_3 / (float)dt;

					std::vector<Brush*> _brushs_3_new;
					float fi = .0f;
					int i = -1;
					int new_count = 0;
					std::string tmp1;
					for (Brush* it : _brushs_3) {
						i++;
						_brushs_3_new.push_back(it);
						tmp1 += "_";
						if (i < t_start_i)
							continue;
						int curr = fi;
						while (new_count < dt && curr >= (int)fi) {
							fi += inc;
							Brush* tmp = new Brush(it);
							tmp->setType(Brush::BrushType::FINISH_IN);
							_brushs_3_new.push_back(tmp);
							new_count++;
							tmp1 += "^";
						}
					}

					//CCLOG("1) %i : %i < %i : old %i :new %i \n[%s]", t_start_i,
					//	  new_count, dt,
					//	  _brushs_3.size(), _brushs_3_new.size(),
					//	  tmp1.c_str());
					t_count_new_brush_3 = align_count_new_brusuh_3;
					_brushs_3 = _brushs_3_new;
				}
				else if (dt < 0 && ABS(dt) > dt_min) {
					CC_SAFE_DELETE_ARRAY(not_align_segment);
					dt = -dt;
					float inc = (float)t_count_new_brush_3 / (float)align_count_new_brusuh_3;
					float fi = .0f;
					int i = 0;
					int rm_count = 0;
					std::string tmp1;
					std::string tmp2;
					float dt_count = .0f;
					for (ITBRUSH it = _brushs_3.begin(), it_e = _brushs_3.end(); it_e != it; ++i) {
						if (i < t_start_i) {
							it++;
							continue;
						}
						tmp1 += "_";
						tmp2 += "_";
						it++; if (it_e == it)break;

						dt_count += inc;
						while (rm_count < dt && dt_count >= 1) {
							dt_count -= 1.f;
							(*it)->setType(Brush::BrushType::FINISH_OUT);
							rm_count++;
							tmp1 += "_";
							tmp2 += " ";
							it++; if (it_e == it)break;
						}
						if (rm_count == dt)break;
					}
					t_count_new_brush_3 = align_count_new_brusuh_3;
					//CCLOG("2) %i : %i < %i \n[%s]\n[%s]", t_start_i, rm_count, dt, tmp1.c_str(), tmp2.c_str());
				}
				else {
					CC_SAFE_DELETE_ARRAY(not_align_segment);
					//CCLOG("3) ");
				}

#endif
#if 0 //only test
				if (NULL != testDraw) {
					//testDraw->drawCubicBezier(p1, c1, c2, p2, t_count_new_brush_3, Color4F::ORANGE);

					for (int i = 0; i < t_count_new_brush_3 - 1; i++) {
						testDraw->drawLine(segment[i], segment[i + 1],
										   //Color4F(CCRANDOM_0_1(), CCRANDOM_0_1(), CCRANDOM_0_1(), 1.)
										   //Color4F::BLACK
										   Color4F::WHITE
						);
						testDraw->drawPoint(segment[i], 3, Color4F::ORANGE);
						//testDraw->drawPoint(segment[i + 1], 3, Color4F::BLACK);
					}


					/*
					CC_SAFE_DELETE_ARRAY(segment);
					segment = TSpline::createSegments(p1, c1, p2, c2, t_count_new_brush_3, &lenSegment, true);
					for (i = 0; i < t_count_new_brush_3 - 1; i++)
					{
					testDraw->drawLine(segment[i], segment[i + 1], Color4F::GREEN);
					testDraw->drawPoint(segment[i], 13, Color4F::GREEN);
					testDraw->drawPoint(segment[i + 1], 13, Color4F::GREEN);
					}
					/**/
			}
#endif
		}
			else {
				t_count_new_brush_3 = _brushs_3.size();
				t_start_i = -1;
			}

			std::string type_1 = "";
			std::string type_2 = "";

			Brush* brushP = NULL;
			int i = -1;
			for (ITBRUSH it = _brushs_3.begin(), it_e = _brushs_3.end(); it_e != it; ) {
				Brush* brush = (*it);
				type_1 += (Brush::BrushType::FINISH_OUT == brush->m_type ? " " :
						   Brush::BrushType::FINISH_IN == brush->m_type ? "^" : "_");
				if (brush->m_type == Brush::BrushType::FINISH_OUT && !brush->hasStartToEnd())
					i = i;
				else
					i++;


				if (NULL != segment && i >= t_start_i) {
					type_2 += "e";
					int seg_id = MIN(i - t_start_i, t_count_new_brush_3 - 1);
					Vec2 toPos = segment[seg_id];
					brush->toEnd(toPos, _durationFromTo);
#if 0//from test only
					if (NULL != testDraw) {
						testDraw->drawLine(brush->getCurrPos(), toPos, Color4F(COLOR3B_RANDOM(), 1.));
						//testDraw->drawPoint(brush->getCurrPos(), 13, Color4F::BLACK);
						//testDraw->drawPoint(toPos, 13, Color4F::ORANGE);
						if (NULL != brushP) {
							Vec2 p1 = brush->getCurrPos();
							Vec2 p2 = brushP->getCurrPos();
							testDraw->drawLine(p1, p2, Color4F::GREEN);
							/*
							float angle = MATH_RAD_TO_DEG(Vec2(p1, p2).getAngle());
							Vec2  p3 = p2 + Vec2(20, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(angle - 110));
							Vec2 p4 = p2 + Vec2(20, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(angle + 115));

							testDraw->drawLine(p1, p3, Color4F::GREEN);
							testDraw->drawLine(p1, p4, Color4F::GREEN);
							*/
						}
						it++;
						brushP = brush;
						continue;
				}
#endif
			}
				else
					type_2 += "_";

				brush->update(delta);
				//###############
				if (brush->isEnd) {
					_brushs_end.push_back(brush);
					it = _brushs_3.erase(it);
					it_e = _brushs_3.end();
					continue;
				}
				if (t_isVisit)
					brush->visit();
				it++;
	}
			//if (NULL != segment)
			//	CCLOG("type \n[%s]\ntoEnd \n[%s]", type_1.c_str(), type_2.c_str());

			CC_SAFE_DELETE_ARRAY(segment);
}
	}
	//###############################
	if (NULL != testDraw) {
		BEGIN_RT_1();
		testDraw->visit();
	}
#endif 
	//##################################### //обработка помошника
	//if (false) 
#if 1
	{
		double now_t = HelperUtils::GetCurrTime();
		if (_last_end_helper < .0 && _brushs_helper.empty()) {
			_last_end_helper = now_t;
		}

		//длительность последнего удачного поподания точки
		if (
			now_t - _last_change_currPos > t_delay_startHelper &&
			now_t - _last_end_helper && 
			_brushs_helper.empty()
			) {
			_last_change_currPos = now_t;
			_last_end_helper = -1;
			createHelper(currPos, currPos + t_count_from_helper);
		}

		BEGIN_RT_1();

		for (Brush* brush : _brushs_helper) {
			brush->update(delta);
			if (t_isVisit) brush->visit();
		}
	}
#endif
	//#####################################
	END_RT_1();

	//#########################################
	//рисуем послдение точки один раз 
	// которые уже на месте (на следующем хаходе все удаляем)if (t_isVisit && !_brushs_end.empty()) 
	{
		m_rt_2->begin();
		for (Brush* it : _brushs_end)
			it->visit();
		m_rt_2->end();
	}

	if (isEndGame &&
		_brushs_end.empty() &&
		_brushs_1.empty() &&
		_brushs_2.empty() &&
		_brushs_3.empty())
		isEndGameAnim = true;

	mLastAdd = 0;

	Director::getInstance()->getRenderer()->render();
}

struct T_Segments {
	Vec2 p2;
	Vec2 c2_2;

	Vec2 p1;
	Vec2 c1_1;
	Vec2 c1_2;

	bool hasNext;
	const DrawToDots::converterFunc& _converter;

	T_Segments(Vertex* first, const DrawToDots::converterFunc& converter):
		_converter(converter) {
		hasNext = false;
		p2 = first->getPos();
		c2_2 = first->getC2();


		if (nullptr != converter) {
			p2 = converter(p2);
			c2_2 = converter(c2_2);
		}
	}

	void pushNext(Vertex* next) {
		if (hasNext) {
			p2 = p1;
			c2_2 = c1_2;

		}
		p1 = next->getPos();
		c1_1 = next->getC1();
		c1_2 = next->getC2();
		hasNext = true;

		if (nullptr != _converter) {
			p1 = _converter(p1);
			c1_1 = _converter(c1_1);
			c1_2 = _converter(c1_2);
		}
	}

	Vec2* createSegments(int count, float* length) {
		return  TSpline::createSegments(p1, c1_1, p2, c2_2, count, length);
	}
};

Vec2* DrawToDots::createSegments(int start_id, int end_id, int* count_segments_out, float* length_out /*= NULL*/, bool align /*= false*/) {
	float t_lenght = 0.f;
	T_Segments* t_segments = NULL;
	int i = -1;
	//##
	std::vector<int> t_counts;
	int t_all_counts = 0;
	//float t_count;
	//считаем длинну секторов
	int t_count_sectors = 0;
	int count_last_assistant = 0;
	for (Vertex* vertex : getControlAll()) {
		bool t_assistant = vertex->hasAssistant();
		if (!t_assistant) i++;
		if (i >= end_id)		break;
		if (i < start_id)	continue;
		t_count_sectors++;

		if (t_assistant)
			count_last_assistant++;
		else
			count_last_assistant = 0;

		if (*count_segments_out == -1 && t_count_sectors > 1) {// длинну считаем со следуйщей точки
			int tmp = vertex->getCountFromBezier()*10.;
			t_all_counts += tmp;
			t_counts.push_back(tmp);
		}
	}
	if (count_last_assistant != 0 && !t_counts.empty()) {
		for (auto it = t_counts.end() - count_last_assistant, it_e = t_counts.end(); it != it_e; ++it)
			t_all_counts -= *it;
		t_counts.erase(t_counts.end() - count_last_assistant, t_counts.end());
	}


	if (*count_segments_out != -1 && t_count_sectors > *count_segments_out)
		return NULL;
	if (*count_segments_out == -1 && t_count_sectors <= 1)
		return NULL;
	if (t_counts.empty()) {
		int last_count = (int)((float)*count_segments_out / (float)t_count_sectors);
		for (int i = 0; i < t_count_sectors; ++i) {
			t_all_counts += last_count;
			t_counts.push_back(last_count);
		}
		if (t_all_counts != *count_segments_out) {
			last_count = (*count_segments_out) - t_all_counts;
			std::vector<int>::iterator t_it_last = t_counts.end() - 1;
			if (last_count > 0) {
				*t_it_last += last_count;
			}
			else {
				t_it_last += last_count;
			}
		}
		t_all_counts = *count_segments_out;
	}
	else {
		*count_segments_out = t_all_counts;
	}

	Vec2* ret = new (std::nothrow) Vec2[t_all_counts];
	//##
	std::vector<int>::iterator t_it_last = t_counts.begin();
	int ret_size = 0;
	i = -1;
	for (Vertex* vertex : getControlAll()) {
		bool t_assistant = vertex->hasAssistant();
		if (!t_assistant) i++;
		if (!t_assistant && i > end_id)		break;
		if (i < start_id)	continue;
		if (t_it_last == t_counts.end()) {
			break;
		}

		if (NULL == t_segments) {
			t_segments = new T_Segments(vertex, m_converter_coords);
			continue;
		}
		else {
			t_segments->pushNext(vertex);
		}
		float t_len = .0f;

		Vec2* t_ret = t_segments->createSegments((*t_it_last - 1), &t_len);
		t_lenght += t_len;

		memmove(ret + (t_all_counts - ret_size - *t_it_last)
				, t_ret
				, (size_t)((*t_it_last) * sizeof(Vec2)));
		ret_size += (*t_it_last);
		t_it_last++;
	}
	delete t_segments;

	if (NULL != length_out)
		*length_out = t_lenght;
	if (align) {
		Vec2* ret_align = TSpline::aliginSegments(ret, count_segments_out, t_lenght);
		CC_SAFE_DELETE_ARRAY(ret);
		return ret_align;
	}
	return ret;

}

void DrawToDots::createHelper(uint fromPosId, uint toPosId) {
	if (!hasSpline())return;
	Vec2* segment = NULL;
	int sectorId = -1;

	uint maxTo = MIN(getControlLSize() - 1, toPosId) + 1;
	float t_lenght = 0;
	int count_sector = -1;
	Vec2* t_segment = createSegments(fromPosId, maxTo, &count_sector, &t_lenght, true);
	if (NULL == t_segment)return;

	float i_brush = 0;
	for (int si = count_sector - 1; si >= 0; --si) {
		Brush* nb = new Brush(this, m_brush_sprite, t_segment[si]);
		nb->setType(Brush::BrushType::HELPER);

		nb->setAmplitude(m_helper_amplitude,
						 m_helper_space * i_brush);

		nb->toEnd(m_helper_delay*i_brush,
				  m_helper_duration);
		i_brush++;
		_brushs_helper.push_back(nb);
	}

	CC_SAFE_DELETE_ARRAY(t_segment);
}

bool DrawToDots::mouseDown(const Vec2& mousePos, bool isCreated) {
	if (!_isVisible) return false;
	if (WorldSpline::mouseDown(mousePos, isCreated))	return true;
	if (isEnd())										return false;

	startP = mousePos;
	endP = movieP = IONE;

	int lastAdd = drawBrush(mousePos, true);
	flyDraw(mousePos);
	if (lastAdd > 0) {
		hasStartGame = true;
		mLastAdd = lastAdd;
		update();
	}
	return true;
}

bool DrawToDots::mouseMovie(const Vec2& mousePos, Vec2* dPos) {
	if (!_isVisible) return false;
	if (WorldSpline::mouseMovie(mousePos, dPos))return true;
	if (isEnd())return false;
	int lastAdd = drawBrush(mousePos);
	flyDraw(mousePos);
	if (lastAdd > 0 || (isNextPos && !isErr)) {
		if (isNextPos && !isErr)
			_last_change_currPos = HelperUtils::GetCurrTime();
		mLastAdd = lastAdd;
		update();
	}
	return	true;
}

uint DrawToDots::drawBrush(const Vec2& changedPos, bool force) {
	if (isEnd())	return 0;
	uint ret = 0;
	Vec2 cMousePos = MathUtils::ConvertToNodeSpace_ignoreRS(m_rt_1, changedPos);
	//_c_pos_rt_texture
	Texture2D* texture = m_rt_1->getTexture();
	cMousePos += texture->getContentSize()*.5;

	if (!ISONE(startP)) {
		Vec2 start = ISONE(movieP) ? cMousePos : movieP;
		Vec2 end = cMousePos;
		float distance = start.distance(end);
		float inc = incBrush;// 10;// distance/100.;
		if (
			force // && distance > 10
			|| distance >= distanceEdgeBrush) {
			int d = (int)distance;
			float difx = end.x - start.x;
			float dify = end.y - start.y;
			//int startS = _brushs_1.size();
			for (float i = 0; i <= distance; i += inc) {
				float delta = i / (distance == 0 ? 1 : distance);
				Vec2 newP = Vec2(start.x + (difx * delta), start.y + (dify * delta));
				//globId = NEXTID();
				Brush* nb = new Brush(this, m_brush_sprite, newP);// , globId);
				_brushs_1.push_back(nb);
				ret++;
			}
			movieP = cMousePos;
		}
	}
	if (ISONE(movieP))
		movieP = cMousePos;
	return ret;
}

bool DrawToDots::mouseUp(const Vec2& mousePos) {
	if (!_isVisible) return false;
	if (WorldSpline::mouseUp(mousePos))
		return true;
	if (isEnd())
		return false;

	endP = mousePos;
	startP = movieP = IONE;
	isErr = false;
	//isNextPos = 
	isCurrPos = isStartCurrPos = false;
	//flyDraw(mousePos, true);

	endBrush(_brushs_1);
	endBrush(_brushs_2);
	return true;
}

bool DrawToDots::flyDraw(const Vec2& pos, bool up) {
	if (isEndGame)		return false;
	if (ISONE(startP))	return false;
	if (!hasSpline()) return false;

	int i = -1;
	int newPos = -1;
	for (Vertex* ver : getControlAll()) {
		if (ver->hasAssistant())continue;
		i++;
		if (i < currPos)
			continue;
		if (isStartCurrPos && i == currPos)
			continue;
		if (ver->getPos().distance(pos) <= _dContainsR) {
			newPos = i;
			break;
		}
	}

	if (newPos == -1)
		return false;
	isNextPos = newPos == currPos + 1;
	isCurrPos = newPos == currPos;
	//CCLOG("%i %i", newPos, currPos);
	if (!isStartCurrPos &&
		(isCurrPos || (newPos == 0 && currPos == -1))
		) {
		isStartCurrPos = true;
	}

#if 0
	if (isCurrPos)
		LOGD("isCurrPos");
	if (isNextPos)
		LOGD("isNextPos");
#endif
	if (!isStartCurrPos)
		isNextPos = isCurrPos = false;


	if (isNextPos || isCurrPos) {
		isErr = false;
		//попали в следующую точку
		if (isNextPos) {
			startShake(newPos - 1, 3);

			currPos = newPos;
			//обнуления помошника
			_last_change_currPos = HelperUtils::GetCurrTime();
			//freeBrushs(_brushs_helper, true);
			endBrush(_brushs_helper);

#if 1//IS_PC
			if (newPos + 1 == getControlLSize()
#else
			if (newPos + 1 >= 5
#endif
				//XXXXXXXXXXXXXX
				|| (isTestFastEnd && newPos + 1 >= 1)
				) {
				isEndGame = true;
				if (isTestFastEnd) {
					for (Vertex* v1 : getControlAll())
						drawCircle(_layer, v1, false, true, 3);
				}
				else {
					startShake(newPos, 3);
				}
			}
			else {
				startShake(newPos, 0);
			}

		}
		else {
			startShake(newPos, 0);
		}
	}
	else {
		//LOGD("isErr");
		isErr = true;
		isStartCurrPos = false;
		startShake(newPos, currPos > newPos ? -1 : 1);
	}

	return true;
}

bool DrawToDots::setChangedSpline(bool splineChanged) {
	if (splineChanged) {
		controllReset();
		//recreate excude assistant
	}
	_isChanged = splineChanged;
	return WorldSpline::setChangedSpline(splineChanged);
}

void DrawToDots::recalc() {
	if (_TSplines.size() != 0)
		_TSplines[0]->recalc();
}

void DrawToDots::startShake(int id, int type) {
	if (getControl(id) == NULL)
		return;
	Vertex* v1 = getControl(id);
	drawCircle(_layer, v1, false, true, type);
}

Node* DrawToDots::printText(Node* layer, Vertex* p1, int id, bool forceClear/* = false*/) {
	Node* text = WorldSpline::printText(layer, p1, id, forceClear);
	if (NULL != text) {
		text->setScale(m_scale);
	}
	return text;
}

void DrawToDots::drawCircle(Node* layer, Vertex* p1, bool changed, bool shake /*= false*/, int type /*= 0*/) {
	Node* parent = p1->node;

	if (p1->hasAssistant()) {//для редактора
		if (NULL == parent)return;
		Sprite* nodeCircle = parent->getChildByName<Sprite*>(NAME_CHILD_CIRCLE);
		if (NULL != nodeCircle)
			parent->removeChild(nodeCircle);
		p1->node->removeFromParent();
		p1->node = NULL;
		return;
	}
	if (NULL == parent) {
		parent = createParent(layer, p1);
	}
	if (changed)
		parent->setPosition(p1->getPos());
	Sprite* nodeCircle = parent->getChildByName<Sprite*>(NAME_CHILD_CIRCLE);


	if (changed && NULL != nodeCircle) {
		nodeCircle->stopAllActions();
		nodeCircle->setScale(_scale_node_circle);
		return;
	}
	if (NULL != nodeCircle) {
		if (shake &&type != -1)//& nodeCircle->getNumberOfRunningActions() == 0)
		{
			nodeCircle->stopAllActions();
			nodeCircle->setScale(_scale_node_circle);
			Vec2 pos = Vec2::ZERO;// p1->getPos();
			nodeCircle->setPosition(pos);

			Vector<FiniteTimeAction*> movieActions;
			Vector<FiniteTimeAction*> opacityActions;

			if (type == 3)// opacity end
			{
				Node* nodeText = parent->getChildByName(NAME_CHILD_TEXT);
				CALL_VOID_METHOD(nodeText, runAction(FadeTo::create(.7, 0)));
				opacityActions.pushBack(FadeTo::create(.7, 0));
			}
			else if (type == 2)//default
			{
				nodeCircle->setScale(_scale_node_circle);
				Node* nodeText = parent->getChildByName(NAME_CHILD_TEXT);
				if (NULL != nodeText)
					nodeText->setOpacity(255);
				nodeCircle->setColor(getColorCircleStart());
				opacityActions.pushBack(FadeTo::create(.2, 255));
			}
			else if (type == 0)//yes
			{
				float prevScale = nodeCircle->getScale();
				bool isEnd = nodeCircle->getColor() == getColorYes();
				opacityActions.pushBack(TintTo::create(.01, getColorYes()));
				opacityActions.pushBack(FadeTo::create(.2, 255));
				if (isEnd) {
					movieActions.pushBack(ScaleTo::create(.3, prevScale*1.6));
					movieActions.pushBack(DelayTime::create(.1));
					movieActions.pushBack(ScaleTo::create(.2, prevScale));
				}
				else {
					opacityActions.pushBack(FadeTo::create(.2, 255));
					for (int i = 0; i < 3 + CCRANDOM_0_1() * 3; i++)
						movieActions.pushBack(MoveTo::create(.05, pos + Vec2(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())*
						(_dDrawR*.4)));

					//opacityActions.pushBack(FadeTo::create(.7, 0));
				}
			}
			else if (type == -1)//<
			{
				return;
				Color3B prevCol = nodeCircle->getColor();
				float prevScale = nodeCircle->getScale();
				bool isVisible = prevCol == getColorYes();

				if (isVisible) {
					nodeCircle->setColor(getColorYes());
					nodeCircle->setOpacity(255);
					opacityActions.pushBack(TintTo::create(.3, COLOR_NO));
					opacityActions.pushBack(TintTo::create(.3, getColorYes()));
				}
				else {
					opacityActions.pushBack(TintTo::create(.01, COLOR_NO));
					opacityActions.pushBack(FadeTo::create(.2, 255));
					opacityActions.pushBack(DelayTime::create(.2));

					opacityActions.pushBack(FadeTo::create(.2, 0));

					opacityActions.pushBack(TintTo::create(.01, prevCol));
				}

				movieActions.pushBack(ScaleTo::create(.3, prevScale*1.5f));
				movieActions.pushBack(ScaleTo::create(.3, prevScale));
				//for (int i = 0; i < 5 + CCRANDOM_0_1() * 5; i++)
				//	movieActions.pushBack(MoveTo::create(.05, pos + Vec2(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())*dDrawR));
			}
			else if (type == 1)//>
			{
				Color3B prevCol = nodeCircle->getColor();

				nodeCircle->setColor(getColorCircleStart());
				nodeCircle->setOpacity(255);

				opacityActions.pushBack(TintTo::create(.1, COLOR_NO));
				opacityActions.pushBack(DelayTime::create(.5));
				opacityActions.pushBack(TintTo::create(.1, getColorCircleStart()));

				for (int i = 0; i < 5 + CCRANDOM_0_1() * 5; i++)
					movieActions.pushBack(MoveTo::create(.05, pos + Vec2(CCRANDOM_MINUS1_1(), CCRANDOM_MINUS1_1())*_dDrawR));
			}

			movieActions.pushBack(MoveTo::create(.1, pos));
			nodeCircle->runAction(
				Spawn::create(
					Sequence::create(opacityActions),
					Sequence::create(movieActions),
					nullptr
				)
			);
		}
		return;
	}

	nodeCircle = Sprite::createWithTexture(_point_texture);
	nodeCircle->setCascadeOpacityEnabled(true);
	//__ centr point
	{
		Sprite* t_sprite = Sprite::create("point_0.png");
		t_sprite->setColor(Color3B::WHITE);
		t_sprite->setAnchorPoint(Vec2::ZERO);
		nodeCircle->addChild(t_sprite, 2);
	}

	//__
	nodeCircle->setScale(_scale_node_circle);
	nodeCircle->setColor(getColorCircleStart());

	parent->addChild(nodeCircle, 1, NAME_CHILD_CIRCLE);
	p1->node = parent;
}

//#include "UI_dtd.h"
//bool once = true;

void DrawToDots::draw() {
	if (!_isVisible)return;
	//return;
	//WorldSpline::draw();
	/*
	if (once)
		//if (false) 
	{
		_layer->getParent()->getChildByName(UI_dtd::NAME_CANVAS_NODE)->setVisible(false);
		_layer->getParent()->getChildByName(UI_dtd::NAME_BACKGROUND_NODE)->setVisible(false);
		m_rt_0->setVisible(false);
		m_back_img->setVisible(false);
		once = false;


	}
	*/
#if 0
	if (NULL == testDraw_na) {
		_layer->addChild(testDraw_na = DrawNode::create());
		_layer->addChild(testDraw_a = DrawNode::create());

		float lenSegment;
		int count = count_spline;
		CCLOG("############################");
		Vec2* segment_na = createSegments(start_i, end_i, &count, &lenSegment, false, CC_CALLBACK_1(DrawToDots::m_converter_coords, this));
		if (NULL != segment_na) {
			CCLOG("len = %f", lenSegment);
			for (int i = 0; i < count; i++) {
				testDraw_na->drawPoint(segment_na[i], 4, Color4F::BLACK);
				if (count > i + 1)
					testDraw_na->drawLine(segment_na[i], segment_na[i + 1], Color4F(.0f, .5f, 1.f, 1.f));
			}
			//CC_SAFE_DELETE_ARRAY(segment_na);
			///*
			Vec2* a_segment = TSpline::aliginSegments(segment_na, &count, lenSegment, d_len);
			CCLOG("count_1 = %f ret_c = %f", lenSegment / d_len, count);
			for (int i = 0; i < count; i++) {
				testDraw_a->drawPoint(a_segment[i], 3, Color4F::GREEN);
				if (count > i + 1)
					testDraw_a->drawLine(a_segment[i], a_segment[i + 1], Color4F(COLOR3B_RANDOM(), 1.));
			}
			CC_SAFE_DELETE_ARRAY(a_segment);
			CC_SAFE_DELETE_ARRAY(segment_na);
		}
		/**/
	}
	if (show_dif) {
		testDraw_na->setPosition(pos_td + Vec2(10, 10));
		testDraw_a->setPosition(pos_td + Vec2(10, -10));
	}
	else {
		testDraw_na->setPosition(pos_td);//+Vec2(10, 10));
		testDraw_a->setPosition(pos_td);// +Vec2(10, -10));
	}
#endif
	//return;
	Vertex* v2 = NULL;
	for (TSpline* _tspline : _TSplines) {
		v2 = NULL;
		Vertex* first = NULL;
#if 1
		for (Vertex* v1 : _tspline->getSpline()) {
			if (NULL == first)
				first = v1;
			v1->Draw(false, _isEdit);
			v2 = v1;
		}
#endif
		//continue;
		//###################################
		v2 = NULL;
		first = NULL;
		int i = 0;
		for (Vertex* v1 : _tspline->getControl()) {
			if (!v1->hasAssistant())
				i++;
			if (NULL == first) first = v1;
			v1->Draw(true, _isEdit);
			if (NULL != v2) {
				Vertex::Draw(v2, v1, false, false, _isEdit);
				//#################################
#if 0
				if (v2 != NULL) {
					//once2 = false;
					uint count = 5;
					Vec2 dPos = _c_pos_rt_texture;
					//#define CONVERT(x) m_converter_coords(x)
#define CONVERT(x) (x)
					Vec2 p2 = CONVERT(v1->getPos());
					Vec2 c2 = CONVERT(v1->getC1());
					Vec2 p1 = CONVERT(v2->getPos());
					Vec2 c1 = CONVERT(v2->getC2());

					float lenSegment;
					Vec2* segment = TSpline::createSegments(p2, c2, p1, c1, count, &lenSegment);
					Vec2* a_segment = TSpline::aliginSegments(segment, &count, lenSegment);
					DrawColor(Color4B::BLACK); glLineWidth(20);
					for (int i = 0; i < count; i++) {
						testDraw_2->drawPoint(a_segment[i], 3, Color4F::BLUE);
						//testDraw->drawPoint(segment[i+1], 1, Color4F::BLUE);
						//ccDrawLine(segment[i], segment[i + 1]);
						//ccDrawPoint(segment[i]); ccDrawPoint(segment[i + 1]);
					}
					CC_SAFE_DELETE_ARRAY(a_segment);

					a_segment = TSpline::aliginSegments(segment, &count, lenSegment, .3);
					DrawColor(Color4B::GREEN); glLineWidth(10);
					for (int i = 0; i < count; i++) {
						testDraw_2->drawPoint(a_segment[i], 3, Color4F::GREEN);
						testDraw_2->drawLine(a_segment[i], a_segment[i + 1], Color4F(COLOR3B_RANDOM(), 1.));
					}
					CC_SAFE_DELETE_ARRAY(a_segment);

					CC_SAFE_DELETE_ARRAY(segment);
			}
#endif
		}
			v2 = v1;

			printText(_layer, v1, i, _recreateLabel);
			drawCircle(_layer, v1, _isChanged);
	}

		if (_tspline->IsClosed()) {
			Vertex::Draw(v2, first, false, false, _isEdit);
		}
}

#if 0
	int count = 100;
	float length;
	Vec2* segment = createSegments(0, 10, &count, &length, true);
	DrawColor(Color4B::BLACK); glLineWidth(20);
	for (int i = 0; i < count; i++) {
		testDraw->drawPoint(segment[i], 4, Color4F::BLUE);
		testDraw->drawLine(segment[i], segment[i + 1], Color4F::ORANGE);
		//ccDrawLine(segment[i], segment[i + 1]);
		//ccDrawPoint(segment[i]); ccDrawPoint(segment[i + 1]);
	}
	CC_SAFE_DELETE_ARRAY(segment);
#endif

	if (!_TSplines.empty()) {
		_recreateLabel = false;
		_isChanged = false;
	}
}


#pragma warning (pop)
