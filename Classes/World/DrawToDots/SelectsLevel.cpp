#include "stdafx.h"
#include "SelectsLevel.h"
#include "DevelopMenu.h"
#include "UI_dtd.h"

#define IMAGE_STROKE		"/stroke.png"
#define IMAGE_FILL			"/select.png"
#define INDICATOR_COLOR		Color3B(190, 252, 255)

#if HAS_DEVELOP
Level* _level_1 = NULL;
#endif

class Level {
private:
	std::string m_file_name;
	float m_finish_time;
	float m_start_time;
	EffectSprite* image;
	Effect* effect_img;
	float time = .0f;

	Sprite* image_2;
public:
	Level(const std::string& name):
		m_file_name(name),
		m_finish_time(-1.),
		m_start_time(-1.) {
		image = EffectSprite::create(m_file_name + IMAGE_STROKE);
		image_2 = Sprite::create(m_file_name + IMAGE_FILL);
		image_2->retain();
		effect_img = Effect::create("shaders/border.frag");
		effect_img->retain();

		//effect_img->getGLProgramState()->setUniformTexture("image_fill",)

		image->setEffect(effect_img);
		image->setPosition(image->getContentSize()*.5);
#if HAS_DEVELOP
		if (NULL == _level_1)
			_level_1 = this;
#endif
	}

	EffectSprite* getImage() { return image; }

#define shader (effect_img->getGLProgramState())
	void update(float diff) {
		time += diff;
		if (time >= 3600.)time = .0;
		shader->setUniformFloat("_time", time);
	}

	void setStage(bool hasStart, bool hasFinish) {
		if (hasFinish) {//&& m_finish_time < .0) {
			m_finish_time = time;
			m_start_time = -1.;
		}
		else if (hasStart) {
			m_start_time = time;
		}
		else {
			m_start_time = -1.;
			//m_finish_time = -1.;
		}

		shader->setUniformFloat("_time", time);
		shader->setUniformFloat("isStart", m_start_time);
		shader->setUniformFloat("isFinish", m_finish_time);
		shader->setUniformTexture("image_2", image_2->getTexture());
	}

	void reset() {
		m_start_time = -1.;
		m_finish_time = -1.;
		setStage(false, false);
	}
#undef shader


	~Level() {
		image_2->release();
		effect_img->release();
	}
};

//##################################################

SelectsLevel::SelectsLevel(Layer* layer, std::vector<std::string> levels, float scale_ui, const Size& size):
	m_levels(levels),
	_callback(nullptr),
	m_curr_level(-1) {

	// Create the mPage view
	_pageView = PageView::create();

	//_pageView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
	//_pageView->setBackGroundColor(Color3B::BLUE);

	_pageView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_pageView->setDirection(PageView::Direction::HORIZONTAL);
	_pageView->setIndicatorEnabled(true);

	_pageView->setIndicatorIndexNodesColor(INDICATOR_COLOR);
	//_pageView->setIndicatorPosition(Vec2::ZERO);

	loadImages(scale_ui, size);

	layer->addChild(_pageView, UI_dtd::_selectLevelZOreder);

#if HAS_DEVELOP
	/*
	DevelopMenu::Instance().changeDebugMode(this, ([this](const bool&enable) {
		CALL_VOID_METHOD(_image, setVisible(enable));
	}));
	*/

	DevelopMenu::Instance().changeKeyEvents(([this](EventKeyboard::KeyCode keyCode, KeyState keyState) {
		bool ret = false;
		if (keyState != KeyState_RELEASE)
			return false;
		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_1:
				_level_1->setStage(true, false);
				return true;
			case EventKeyboard::KeyCode::KEY_2:
				_level_1->setStage(false, true);
				return true;
			case EventKeyboard::KeyCode::KEY_3:
				_level_1->reset();
				return true;
		}
		return false;
	}), this);

#endif
}

void SelectsLevel::update(float delta) {
	for (Level* level : mLevels) {
		level->update(delta);
	}
}

void SelectsLevel::goPage(bool next) {
	_pageView->scrollToItem(_pageView->getCurrentPageIndex() + (next ? 1 : -1));
}

void SelectsLevel::setLevel(int id_level, bool isStart, bool isFinish) {

	if (-1 == id_level) {
		for (Level* level : mLevels) {
			level->setStage(false, false);
		}
	}
	else {
		mLevels.at(id_level)->setStage(isStart, isFinish);
	}
}

void SelectsLevel::actionFinished() {
	_pageView->setVisible(false);
}

void SelectsLevel::setVisible(bool visible) {
	if (visible) {
		_pageView->setIndicatorEnabled(true);
		_pageView->setIndicatorIndexNodesColor(INDICATOR_COLOR);
		_pageView->setVisible(true);
		setLevel(m_curr_level, false, false);
	}
	else {
		_pageView->runAction(Sequence::create(DelayTime::create(1.f),
											  CallFunc::create([this]() { this->actionFinished(); })
											  , nullptr));
	}
	_pageView->setEnabled(visible);
}

void SelectsLevel::loadImages(float scale_ui, const Size& size) {
	_pageView->setContentSize(size);

	int pageCount = m_levels.size();
	int i = -1;
	for (std::string level : m_levels) {
		i++;
		Layout* layout = dynamic_cast<Layout*>(_pageView->getItem(i));
		if (!layout) {
			layout = Layout::create();
			_pageView->insertCustomItem(layout, i);
		}


		layout->setContentSize(size);
		Button* button = dynamic_cast<Button*>(layout->getChildByName(level));
		if (!button) {
			button = Button::create(level + IMAGE_STROKE);
			button->setOpacity(0);

			button->setUserData((void*)(i));
			button->setScale(.8f*scale_ui);
			button->setName(level);
			//########################################
			Level* t_new_level = new Level(level);
			mLevels.push_back(t_new_level);

			button->addChild(t_new_level->getImage());
			//########################################
			//button->setContentSize(image->getContentSize());
			layout->addChild(button);

			button->addTouchEventListener([this](Ref* parent, Widget::TouchEventType type) {
				Button* button = (Button*)parent;
				EffectSprite* sprite = (EffectSprite*)button->getChildren().at(0);
				float zoomScale = 1.1f;
				sprite->stopAllActions();
				sprite->setScale(1.f);
				switch (type) {
					case cocos2d::ui::Widget::TouchEventType::BEGAN:
					{
						sprite->runAction(ScaleTo::create(0.05f, zoomScale, zoomScale));
					}
					break;
					case cocos2d::ui::Widget::TouchEventType::MOVED:
						break;
					case cocos2d::ui::Widget::TouchEventType::ENDED:
						break;
					case cocos2d::ui::Widget::TouchEventType::CANCELED:
						break;
				}
			});

			button->addClickEventListener(([this](Ref* parent) {

				Button* button = ((Button*)parent);
				m_curr_level = (int)(size_t)(button->getUserData());
				setLevel(m_curr_level, true, false);
				if (_callback)
					_callback(m_curr_level);
			}));
		}
		button->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));


#if 0
		Text* label = dynamic_cast<Text*>(layout->getChildByTag(1));
		if (!label) {
			label = Text::create(StringUtils::format("mPage %d", (i + 1)), "fonts/Marker Felt.ttf", 30);
			layout->addChild(label, 2, 1);
		}
		label->setColor(Color3B(192, 192, 192));
		label->setPosition(Vec2(size.width / 2.0f, size.height / 2.0f));
#endif
	}

	setLevel(-1, false, false);
	_pageView->setCurrentPageIndex(0);
}

void SelectsLevel::pageViewEvent(Ref *pSender, PageView::EventType type) {
	return;
	switch (type) {
		case PageView::EventType::TURNING:
		{
			PageView* pageView = dynamic_cast<PageView*>(pSender);

			//_displayValueLabel->setString(StringUtils::format("mPage = %ld", static_cast<long>( pageView->getCurrentPageIndex() + 1 )));

		}
		break;

		default:
			break;
	}
}

SelectsLevel::~SelectsLevel() {
	for (auto it : mLevels) {
		delete it;
	}
	_pageView->removeFromParent();
}
