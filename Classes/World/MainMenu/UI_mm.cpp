#include "stdafx.h"
#include "UI_mm.h"
#include "Scenes/GamesCamera.h"
#include "DevelopMenu.h"

int UI_mm::_cloudsZOreder = 1;
int UI_mm::_hillsZOreder = 2;

class AnimHill {
	float scale_ui;
	Sprite* sprite;
	Vec2 to_pos;

	//DrawNode* drawNode;
	int id;
	Size size;
public:
	AnimHill(Node* layer, int i, float _scale_ui):scale_ui(_scale_ui) {
		sprite = Sprite::create(StringUtils::format("b_hill_%i.png", i));

		layer->addChild(sprite, UI_mm::_hillsZOreder);
		size = layer->getContentSize()*.5;
		//sprite->addChild(drawNode = DrawNode::create());
		id = i;
		setFromPos(Vec2::ZERO);

		switch (id) {
			case 4:setToPos(Vec2(369., -355.)); setFromPos(Vec2(-94., 1000.)); break;
			case 3:setToPos(Vec2(801., -319.60)); setFromPos(Vec2(-106.6, 1000.)); break;
			case 2:setToPos(Vec2(-203.29, -160.41)); setFromPos(Vec2(-81., 1010.)); break;
			case 1:setToPos(Vec2(568.52, -524.78)); setFromPos(Vec2(-86.7, 963.)); break;
		}

		hide();
		/*
		#if HAS_DEVELOP

		#define SETTERANCHOR(id) 																							   \
		DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, StringUtils::format("from_pos_%i",id),		   \
		([this](const Vec2& val) {																					   \
		hills.at(id).setFromPos(val);																				   \
		}),																												   \
		([this]() {return hills.at(id).getFromPos(); }));															   \
		DevelopMenu::Instance().addPointSetting(this, 1, DevelopTabs_Settings, StringUtils::format("to_pos_%i", id),	   \
		([this](const Vec2& val) {																					   \
		hills.at(id).setToPos(val);																				   \
		}), \
		([this]() {return hills.at(id).getToPos(); }));


		SETTERANCHOR(id);
		#endif
		*/
	}

	const Vec2& getToPos() { return to_pos; }
	void setToPos(const Vec2& pos) {
		sprite->setPosition(pos);
		to_pos = Vec2(pos) + size;
	}

	Vec2 from_pos;
	void setFromPos(const Vec2& d_angle_lenght) {
		from_pos = Vec2::ONE.rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(d_angle_lenght.x - 45.))*d_angle_lenght.y + size;

		/*
		drawNode->clear();
		if (from_pos.length() <= .0001)return;
		drawNode->setLineWidth(2);
		drawNode->drawLine(from_pos, Vec2::ZERO, Color4F::BLACK);
		*/
	}


	const Vec2& getFromPos() {
		float angle = MATH_RAD_TO_DEG(from_pos.getAngle());
		float lenght = from_pos.getLength();
		return Vec2(angle, lenght);
	}

	void hide() {
		sprite->stopAllActions();
		sprite->setPosition(from_pos);
	}

	void show() {
		float delay = id * .25;
		sprite->stopAllActions();

		Vec2 norm = from_pos.getNormalized();
		float lenght = from_pos.length()*.15;
		if (id == 1)
			sprite->runAction(Sequence::create(
				MoveTo::create(.0, from_pos),
				DelayTime::create(delay),
				MoveTo::create(.15, to_pos - norm * lenght*.25),
				MoveTo::create(.15, to_pos),
				nullptr));
		else if (id == 4)
			sprite->runAction(Sequence::create(
				MoveTo::create(.0, from_pos),
				DelayTime::create(delay),
				MoveTo::create(.2, to_pos - norm * lenght*.2),
				MoveTo::create(.1, to_pos + norm * lenght*.05),
				MoveTo::create(.1, to_pos - norm * lenght*.05),
				MoveTo::create(.15, to_pos),
				nullptr));
		else
			sprite->runAction(Sequence::create(
				MoveTo::create(.0, from_pos),

				DelayTime::create(delay),

				MoveTo::create(.2, to_pos - norm * lenght*.4),
				MoveTo::create(.1, to_pos + norm * lenght*.15),
				MoveTo::create(.1, to_pos - norm * lenght*.15),
				MoveTo::create(.05, to_pos),
				nullptr));
	}
};

static Node* SpriteNode_1;
static Node* SpriteNode_2;
static Node* SpriteNode_3;
//static DrawNode* drawNode = NULL;

class Cloud {
	int id;
	int  g_id;
	Sprite* sprite;
	Rect contain_rect;

	float scale_height = 1.;

	Size sp_size;
	Size size;

	float dy_pos;
	float dx_pos;
public:
	Cloud(Node* layer, int _id):
		id(_id) {
		g_id = id >= 7 ? 3 : id >= 4 ? 2 : 1;

		//if (NULL == drawNode)
		//	layer->addChild(drawNode = DrawNode::create());
		sprite = Sprite::create(StringUtils::format("cloud_%i.png", id));
		//sprite->setScale(scale);


#define C_SPRITENODE(ID)													\
		if(g_id == ID){														\
			if (NULL == SpriteNode_##ID) {									\
				SpriteNode_##ID = Node::create();								\
				layer->addChild(SpriteNode_##ID,UI_mm::_cloudsZOreder);\
			}																\
			SpriteNode_##ID->addChild(sprite);								\
		}

		C_SPRITENODE(1);
		C_SPRITENODE(2);
		C_SPRITENODE(3);

		sp_size = sprite->getContentSize();// *scale_ui;
		//size = GamesCamera::Instance().getSize();
		size = layer->getContentSize();
		//drawNode->drawRect(Vec2::ZERO, size, Color4F::BLACK);
		switch (id) {
			case 1:dy_pos = -334.f; dx_pos = 878.0f; break;
			case 2:dy_pos = -483.6; dx_pos = 2164.0f; break;
			case 3:dy_pos = -523.7; dx_pos = 1276.0f; break;

			case 4:dy_pos = -594.f; dx_pos = 496.0f; break;
			case 5:dy_pos = -676.2; dx_pos = 1710.0f; break;
			case 6:dy_pos = -654.f; dx_pos = 886.0f; break;

			case 7:dy_pos = -763.f; dx_pos = 560.0f; break;
			case 8:dy_pos = -719.f; dx_pos = 1200.0f; break;
			case 9:dy_pos = -799.f; dx_pos = 1965.0f; break;
		}
		//setParam(dy_pos, dx_pos);
		start(true);
		/*
		#if HAS_DEVELOP
		DevelopMenu::Instance().addFloatSetting(this, 1, DevelopTabs_Settings, StringUtils::format("dy_pos_%i", id),
			([this](const float& val) {
			setParam(dx_pos, val);
		}),
			 ([this]() {return this->dy_pos; }));

		DevelopMenu::Instance().addFloatSetting(this, 1, DevelopTabs_Settings, StringUtils::format("dx_pos_%i", id),
			([this](const float& val) {
			setParam(val, dy_pos);
		}),
			 ([this]() {return this->dx_pos; }));

		DEVELOP_ADD_SEPARATOR(1, DevelopTabs_Settings, SeparatorType_Bottom, STD_STRING_EMPTY);
		#endif
		*/
	}

	bool has_start = false;
	void start(bool _has_start = false) {
		has_start = _has_start;
		setParam(dx_pos, dy_pos);
	}

	void setParam(const float& _dx_pos, const float& _dy_pos) {
		dy_pos = _dy_pos;
		dx_pos = _dx_pos;
		Vec2 pos = Vec2(-size.width*.5 - sp_size.width, -size.height*.5) + Vec2(.0, dy_pos);//если на layer положить
		pos = Vec2(-sp_size.width*.5, size.height) + Vec2(.0, dy_pos);

		contain_rect = Rect(pos,
							Size(
								size.width + sp_size.width*1.,
								sp_size.height * scale_height)
		);

		//drawNode->drawRect(contain_rect.origin, contain_rect.origin + contain_rect.size, Color4F(COLOR3B_RANDOM()));


		sprite->setPosition(pos + Vec2(.0, sp_size.height));// *rand_0_1()));
		Vec2 to_pos = Vec2(contain_rect.origin.x + contain_rect.size.width, sprite->getPosition().y);// sp_size.height*rand_0_1());

		//drawNode->drawLine(sprite->getPosition(), to_pos, Color4F(COLOR3B_RANDOM()));
		sprite->stopAllActions();

		float time = 1.4;
		if (3 == g_id) {
			time = 2.6;
		}
		else if (2 == g_id) {
			time = 1.5;
		}
		time *= 15.;

		float delay = .0f;

		if (has_start) {
			time -= (dx_pos) / ((to_pos.x - sprite->getPosition().x) / time);
			sprite->setPosition(sprite->getPosition() + Vec2(dx_pos*.5, .0));
			has_start = false;
		}


#define SET_Z(_id) {if(g_id == _id )sprite->setLocalZOrder(SpriteNode_##_id->getChildrenCount()*CCRANDOM_0_1());}
		SET_Z(1);
		SET_Z(2);
		SET_Z(3);

		//sprite->setPosition(sprite->getPosition() + Vec2(dx_pos /*- size.width*/, .0));

		sprite->runAction(Sequence::create(
			//DelayTime::create(delay),
			MoveTo::create(time
						   //+ CCRANDOM_MINUS1_1()*(speed*.25)
						   , to_pos),

			CallFunc::create([this](void) { start(); }),
			nullptr));

	}

};

UI_mm::UI_mm(Layer* layer):
	UI_canvas(layer) {
	SpriteNode_1 = NULL;
	SpriteNode_2 = NULL;
	SpriteNode_3 = NULL;
	//________________________
	_background = Sprite::create("MainMenu/background.png");
	//_background = Sprite::create("MainMenu/original.png");
	//_background = Sprite::create("MainMenu/clouds.png");

	_background->setName(NAME_BACKGROUND_NODE);
	_background->setScale(_scale_ui);
	//_background->setPosition(Vec2::ZERO);
	//_background->setPosition(_size*-.5);

	layer->addChild(_background, _backgroundZOreder);

	for (int i = 4; i > 0; --i)
		hills.push_back(new AnimHill(_background, i, _scale_ui));

	for (int i = 1; i < 10; ++i)
		clouds.push_back(new Cloud(_background, i));

	//show();
	but_dtd = createBtn(layer, _scale_ui, "but_dtd");
	but_dtd->setPosition(Vec2(-12.3f, -98.f));

	but_cw = createBtn(layer, _scale_ui, "but_cw");
	but_cw->setPosition(Vec2(138.46f, 27.4f));

	but_dtd->hide();
	but_cw->hide();

	_background->setPosition(Vec2(-_background->getContentSize().width, .0));
	_background->runAction(
		Spawn::create(
			MoveBy::create(1., Vec2(_background->getContentSize().width, .0)),
			Sequence::create(
				DelayTime::create(1.1),
				CallFunc::create([this](void) { show_hill(); }),
				//DelayTime::create(2.25),
				CallFunc::create([this](void) {
		but_dtd->setVisible(true, 2. + .5*CCRANDOM_0_1(), true);
		but_cw->setVisible(true, 2. + .5*CCRANDOM_0_1(), true);
		startShake();
	}),
				nullptr), nullptr)
	);

#if HAS_DEVELOP
	DevelopMenu::Instance().changeKeyEvents(

		([this](EventKeyboard::KeyCode keyCode, KeyState keyState) {
		if (keyState == KeyState_RELEASE) {
			if (keyCode == EventKeyboard::KeyCode::KEY_Z) {
				show_hill();
				return true;
			}
			else if (keyCode == EventKeyboard::KeyCode::KEY_X) {
				//if (NULL != drawNode)drawNode->clear();
				for (Cloud* cloud : clouds)cloud->start(true);
				return true;
			}
		}
		return false;
	}), this);

	DevelopMenu::Instance().addPointSetting(this,
											1, DevelopTabs_Settings, "but_cw",
											([this](const Vec2& val) { but_cw->setPosition(val); }),
											([this](void) { return  but_cw->getPosition(); }));

	DevelopMenu::Instance().addPointSetting(this,
											1, DevelopTabs_Settings, "but_dtd",
											([this](const Vec2& val) { but_dtd->setPosition(val); }),
											([this](void) { return  but_dtd->getPosition(); }));
	/*
	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 1, DevelopTabs_Settings,
													  "anchor_1",
													  ([this](const float& val) {
		show();
	}),
													   ([this]() {return .1; }), .0, 1., SeparatorType_Stroke);
													   */
#endif

	layer->addChild(shake_process = Node::create());
}

void UI_mm::startShake() {
	shake_process->stopAllActions();
	shake_process->runAction(Sequence::create(
		DelayTime::create(3.),
		CallFunc::create([this](void) {but_dtd->shake(); but_cw->shake(); startShake(); }),
		nullptr));
}

void UI_mm::show_hill() {
	for (AnimHill* hill : hills) hill->show();
}

void UI_mm::touchEvent(Ref*ref, Widget::TouchEventType eventType) {
	//UI_canvas::touchEvent(ref, eventType);
	startShake();
	if (Widget::TouchEventType::ENDED == eventType)
		_callback((Button*)ref);
}

UI_mm::~UI_mm() {
	for (auto hill : hills)delete hill;
	for (auto cloud : clouds)delete cloud;
}
