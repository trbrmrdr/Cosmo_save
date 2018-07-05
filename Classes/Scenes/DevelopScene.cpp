#include "stdafx.h"
#include "DevelopScene.h"


#define TEST_MODE 0
Sprite* testNode;


#if HAS_DEVELOP
#define IS_DEVELOP_MODE() DevelopMenu::Instance().isEnable()
#endif

DevelopScene::DevelopScene(SceneNames name):
	Layer(),
	m_name(name) {
	std::srand(HelperUtils::GetCurrTime());
	worldMoovie = false;
}

DevelopScene::~DevelopScene() {
	_eventDispatcher->removeEventListenersForTarget(this);
};

void DevelopScene::ChangeToScene(SceneNames toScene) {
	//Scene* oldScene = Director::getInstance()->getRunningScene();
	//DevelopScene* oldpRet = dynamic_cast<DevelopScene*>(oldScene->getChildByTag(TAG_LAYER_1));

	Scene* newScene = DevelopScene::createScene(toScene);
	if (NULL == Director::getInstance()->getRunningScene())
		Director::getInstance()->runWithScene(newScene);
	else
		Director::getInstance()->replaceScene(TransitionFade::create(1.5f, newScene));
}


Scene* DevelopScene::createScene(SceneNames name) {
	auto _scene = Scene::create();
	_scene->setTag(TAG_SCENE_1);

#if HAS_DEVELOP
	bool isEnableDevelop = DevelopMenu::Instance().isEnable();
	DevelopMenu::Instance().init(_scene);
#endif

	DevelopScene *pRet = new(std::nothrow) DevelopScene(name);
	if (pRet && pRet->init()) {
		pRet->autorelease();

		pRet->setTag(TAG_LAYER_1);
		pRet->setIgnoreAnchorPointForPosition(false);
		_scene->addChild(pRet);
	}

#if HAS_DEVELOP
	DevelopMenu::Instance().setEnable(isEnableDevelop);
#endif
	return _scene;
}

bool DevelopScene::init() {
	if (!Layer::init()) {
		return false;
	}
	createListeners();
	setIntevalUpdate(true, 0.0f);

	Size camera_size = VisibleRect::getVisibleRect().size;
	GamesCamera::Instance().init(camera_size, this);

	world = new World_cosmo(this);

	/*
	if (SN_Game_CreateWord == m_name) {
		camera_size = camera_size * .1f;
	}
	GamesCamera::Instance().init(camera_size, this);
	switch (m_name) {
		case SN_Game_CreateWord:
			world = new World_CreateWord(this);
			break;
		case SN_Game_DrawToDots:
			world = new World_dtd(this);
			break;
		case SN_MainMenu:
			world = new World_MainMenu(this);
			break;

	}
	world->load();
	*/

#if HAS_DEVELOP
	setDevelopMode(true);
#else
	//Director::getInstance()->setDisplayStats(true);
	Director::getInstance()->setDisplayStats(false);
#endif
	//######################

#if HAS_DEVELOP
	DevelopMenu::Instance().changeKeyEvents(
		([this](EventKeyboard::KeyCode keyCode, KeyState keyState) {
		if (!IS_DEVELOP_MODE())return false;
		bool ret = false;
		switch (keyCode) {
			case EventKeyboard::KeyCode::KEY_ALT:
				switch (keyState) {
					case KeyState_PRESS:
						worldMoovie = true;
						ret = true;
						break;
					case KeyState_RELEASE:
						worldMoovie = false;
						ret = true;
						break;
				}
				break;
			case EventKeyboard::KeyCode::KEY_Z:
				if (keyState == KeyState_RELEASE) {
					ret = true;
				}
				break;
		}
		return ret;
	}), this, "Alt = worldMoovie");
#endif
	drawMesh();
	//setOnExitCallback([this](void) { DevelopScene::free(); });

	return true;
}

void DevelopScene::createListeners() {

	EventListener* mMouseListener;
#if defined(IS_PC)
	auto mouseListener = EventListenerMouse::create();

	mouseListener->onMouseMove = ([this](Event* event) {
		EventMouse* e = (EventMouse*)event;
		this->onMouseMove(e->getLocationInView(), e->getMouseButton());
	});

	mouseListener->onMouseUp = ([this](Event* event) {
		EventMouse* e = (EventMouse*)event;
		this->onMouseUp(e->getLocationInView(), e->getMouseButton());
	});

	mouseListener->onMouseDown = ([this](Event* event) {
		EventMouse* e = (EventMouse*)event;
		this->onMouseDown(e->getLocationInView(), e->getMouseButton());
	});

	//setTouchMode(Touch::DispatchMode::ONE_BY_ONE);
	//setTouchEnabled(true);
	mouseListener->onMouseScroll = CC_CALLBACK_1(DevelopScene::onMouseScroll, this);
	mMouseListener = mouseListener;

	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = ([this](EventKeyboard::KeyCode keyCode, Event* event) { updateChangeEvents(keyCode, KeyState_PRESS); });
	keyboardListener->onKeyReleased = ([this](EventKeyboard::KeyCode keyCode, Event* event) { updateChangeEvents(keyCode, KeyState_RELEASE); });

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);
#else
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = ([this](Touch* touch, Event* event) {
		return onMouseDown(touch->getLocation(), EventMouse::MouseButton::BUTTON_LEFT);
	});

	touchListener->onTouchMoved = ([this](Touch* touch, Event* event) {
		onMouseMove(touch->getLocation(), EventMouse::MouseButton::BUTTON_LEFT);
	});

	touchListener->onTouchEnded = ([this](Touch* touch, Event* event) {
		onMouseUp(touch->getLocation(), EventMouse::MouseButton::BUTTON_LEFT);
	});

	touchListener->onTouchCancelled = ([this](Touch* touch, Event* event) {
		//onMouseDown(touch->getLocation(), EventMouse::MouseButton::BUTTON_LEFT);
	});
	//_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);

	mMouseListener = touchListener;
#endif
	//_eventDispatcher->addEventListenerWithFixedPriority(mMouseListener, 100);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(mMouseListener, this);
}

void DevelopScene::update(float delta) {

#if IS_PC
	if (!worldMoovie)
		GamesCamera::Instance().update(delta);
#endif

#if TEST_MODE
	Vec2 pos = this->convertToNodeSpace(VisibleRect::center());
	testNode->setPosition(pos);
#endif

	world->update(delta);
}

const std::string _key_update = "update";
const float _d_iu = .01f;


void DevelopScene::setIntevalUpdate(bool isPlay, float inteval) {
	if (isPlay && !_is_play_update) {
		schedule(([this](float delay) { update(delay); }), _inteval_update, _key_update);
	}
	else if (!isPlay && _is_play_update) {
		unschedule(_key_update);
	}
	else if (isPlay && _is_play_update && _inteval_update != inteval) {
		unschedule(_key_update);
		schedule(([this](float delay) { update(delay); }), inteval, _key_update);
	}
	_inteval_update = inteval;
	_is_play_update = isPlay;
}

bool DevelopScene::updateChangeEvents(EventKeyboard::KeyCode _keyCode, KeyState keyState) {
#if HAS_DEVELOP
	if (DevelopMenu::Instance().updateChangeEvents(_keyCode, keyState))
		return true;
#endif

	switch (_keyCode) {
#if HAS_DEVELOP
		case EventKeyboard::KeyCode::KEY_SPACE:
			if (keyState == KeyState_RELEASE) {
				_is_play = !_is_play;
				setIntevalUpdate(_is_play, _inteval_update);
			}
			return true;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
			if (keyState == KeyState_RELEASE)setIntevalUpdate(_is_play_update, _inteval_update + _d_iu);
			return true;
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
			if (keyState == KeyState_RELEASE)setIntevalUpdate(_is_play_update, MAX(.0, _inteval_update - _d_iu));
			return true;
		case EventKeyboard::KeyCode::KEY_0:
			if (keyState == KeyState_RELEASE)setIntevalUpdate(_is_play_update, .0);
			return true;
		case EventKeyboard::KeyCode::KEY_F1:
			if (keyState == KeyState_PRESS)
				setDevelopMode(!IS_DEVELOP_MODE());
			return true;
#endif
		case EventKeyboard::KeyCode::KEY_ESCAPE:
			this->Exit();
			return true;
	}

	return false;
}

bool DevelopScene::onMouseDown(Vec2 _mousePos, EventMouse::MouseButton mouseCode) {
#if HAS_DEVELOP
	if (IS_DEVELOP_MODE() && DevelopMenu::Instance().onMouseDown(_mousePos)) {
		return true;
	}
#endif
	mousePos = _mousePos;
	startPos = _mousePos;
	if (worldMoovie && mouseCode == EventMouse::MouseButton::BUTTON_LEFT) {
		sceneLastPos = this->getPosition();
		return true;
	}
	return false;
}

bool DevelopScene::onMouseUp(Vec2 _mousePos, EventMouse::MouseButton mouseCode) {
#if HAS_DEVELOP
	if (IS_DEVELOP_MODE() && DevelopMenu::Instance().onMouseUp(_mousePos)) {
		return true;
	}
#endif
	mousePos = _mousePos;
	if (worldMoovie && mouseCode == EventMouse::MouseButton::BUTTON_LEFT) {
		//((Scene*)this->getParent())->setPosition( _mouseLastPos + _mouseOriginal );
		return true;
	}

	return false;
}

bool DevelopScene::onMouseMove(Vec2 _mousePos, EventMouse::MouseButton mouseCode) {
#if HAS_DEVELOP
	if (IS_DEVELOP_MODE() && DevelopMenu::Instance().onMouseMove(_mousePos)) {
		return false;
	}
#endif
	mousePos = _mousePos;
	Vec2 dPos = startPos - _mousePos;
	if (worldMoovie && mouseCode == EventMouse::MouseButton::BUTTON_LEFT) {
		Layer::setPosition(sceneLastPos - dPos);
		return true;
	}

	return false;

}

void DevelopScene::onMouseScroll(Event *event) {
#if HAS_DEVELOP
	if (!IS_DEVELOP_MODE()) return;
	if (DevelopMenu::Instance().onMouseScroll(event)) return;

	EventMouse* e = (EventMouse*)event;
	float scale = MAX(0.01f, this->getScale() + ((int)this->getScale() + 1.)*(e->getScrollY() * 0.025));

	Vec2 mousePos_fromScene = convertToNodeSpace(mousePos);

	Vec2 size = this->getContentSize();
	Vec2 newAnchor = Vec2(mousePos_fromScene.x / size.x, mousePos_fromScene.y / size.y);

	MathUtils::SetAnchorFromSavePosition(this, newAnchor);

	this->setScale(scale);
#endif
}

void DevelopScene::drawMesh() {

	Color4F colorLine = Color4F::WHITE;
	Color4F colorBack = Color4F(Color3B(133, 183, 96));//BLACK;
	int tmin = -100000;
	int tmax = 100000;

#if HAS_DEVELOP
	bool isBack = true;
	bool isMesh = true;
#else
	colorBack = Color4F::WHITE;
	bool isBack = true;
	bool isMesh = false;
#endif
	if (isMesh) {
		if (_backMesh) this->removeChild(_backMesh);

		_backMesh = DrawNode::create();
		_backMesh->setLineWidth(4);
		_backMesh->drawLine(Vec2(tmin, 0), Vec2(tmax, 0), colorLine);
		_backMesh->drawLine(Vec2(0, tmin), Vec2(0, tmax), colorLine);

		_backMesh->setLineWidth(1);
		for (int i = -10000; i <= 10000; i += 100) {
			_backMesh->drawLine(Vec2(tmin, i), Vec2(tmax, i), Color4F(colorLine.r, colorLine.g, colorLine.b, 0.5f));
			_backMesh->drawLine(Vec2(i, tmin), Vec2(i, tmax), Color4F(colorLine.r, colorLine.g, colorLine.b, 0.5f));
		}

		//*
		_backMesh->setLineWidth(2);

		//Vec2 posC = Vec2(this->getContentSize().width* this->getAnchorPoint().x, this->getContentSize().height* this->getAnchorPoint().y);
		//_backMesh->drawCircle(posC, 100, 360, 10, true, Color4F::WHITE);

		//mousePos_fromScene = convertToNodeSpace(mousePos);
		//_backMesh->drawCircle(mousePos_fromScene, 99, 360, 20, true, Color4F::RED);

		_backMesh->drawRect(Vec2::ZERO, Vec2(this->getContentSize().width, this->getContentSize().height), Color4F::WHITE);
		/**/
		_backMesh->setPosition(Vec2::ZERO);
		this->addChild(_backMesh, 100000);
	}
	if (isBack) {
		if (_colorBack)this->removeChild(_colorBack);
		_colorBack = DrawNode::create();
		_colorBack->drawSolidRect(Vec2(tmin, tmin), Vec2(tmax, tmax), colorBack);
		this->addChild(_colorBack, -100000);
	}
}

void DevelopScene::Exit() {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

#if HAS_DEVELOP

void DevelopScene::setDevelopMode(bool enable) {
	Director::getInstance()->setDisplayStats(enable);
	if (!DevelopMenu::Instance().isEnable()) {

		//sonya->mouseUp(mousePos_fromScene, MOUSE_RIGHT);

		//setAnchorFromSavePosition( this, Vec2( 0.5f, 0.5f ) );
		//this->setScale(1);
	}
	DevelopMenu::Instance().setEnable(enable);
}

void DevelopScene::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
	_customCommand.init(_globalZOrder + 100000);
	_customCommand.func = CC_CALLBACK_0(DevelopScene::drawScene, this, renderer, transform, flags);
	renderer->addCommand(&_customCommand);
}


#pragma warning (push)
#pragma warning (disable:4996)

void DevelopScene::drawScene(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) {
	world->draw(renderer, transform, flags);
}
#endif
