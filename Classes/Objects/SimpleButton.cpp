#include "stdafx.h"
#include "SimpleButton.h"
#include "DevelopMenu.h"

SimpleButton::SimpleButton(Layer* layer) :
	StaticImage(layer),
	_isVisible(true),
	isPress(false),
	isLoadedPress(false),
	_callback(nullptr),
	_prev_scale(1.f),
	m_text(NULL),
	mName() {
	_imagePress = Sprite::create();
	_layer->addChild(_imagePress);
	_imagePress->setLocalZOrder(_image->getLocalZOrder());
}

bool SimpleButton::select(bool isParent) {
	if (!StaticImage::select(true))return false;
#if HAS_DEVELOP
	edit_setName = DevelopMenu::Instance().addStringSetting(
		this, 0,
		DevelopTabs_World, "NameBut ",
		([this](const std::string& name) { setName(name); }),
		([this](void) { return getName(); }), SeparatorType_Top);
#endif
	if (!isParent)
		refresh();
	return true;
}

void SimpleButton::deselect() {
	StaticImage::deselect();
#if HAS_DEVELOP
	edit_setName = NULL;
#endif
}

void SimpleButton::refresh() {
	StaticImage::refresh();
#if HAS_DEVELOP
	if (!isSelected)return;
	edit_setName->Refresh();
#endif
}

SimpleButton::~SimpleButton() {
	//CCLOG("~SimpleButton_debug_draw");
	deselect();
	_imagePress->removeFromParent();

	//CCLOG("~SimpleButton_debug_draw_end");
}

bool SimpleButton::loadImage(const std::string& name, const std::string& name_normal) {
	if (_imageName == name)return false;
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(name);
	if (texture) {
		_imageName = name;
		_image->initWithFile(name);
		loadTexturePress();
		return true;
	}
	return false;
}

void SimpleButton::loadTexturePress() {
	int pos = _imageName.find_first_of(".");
	std::string format = _imageName.substr(pos + 1, _imageName.length() - _imageName.find_first_of(".") - 1);
	std::string namePress = _imageName.substr(0, pos) + "_press." + format;

	Texture2D *texture_press = Director::getInstance()->getTextureCache()->addImage(namePress);
	isLoadedPress = false;
	if (texture_press) {
		_imagePress->initWithFile(namePress);
		isLoadedPress = true;
		setPress(isPress);
	}
}

void SimpleButton::Load(SettingUtil* setting) {
	StaticImage::Load(setting);
	setName(setting->getStringForKey("buttonName"));
	setText(setting->getStringForKey("buttonText"));
	_defEnableMouse = getEnableMouse();
}

void SimpleButton::setSize(float width, float height) {
	Soul::setSize(width, height);
	float t_width = width / _image->getContentSize().width;
	float t_height = height / _image->getContentSize().height;
	_image->setScale(t_width, t_height);
	_imagePress->setScale(t_width, t_height);
}

SettingUtil* SimpleButton::Save() {
	if (NULL != _image)     _image->setScale(1.);
	if (NULL != _imagePress)_imagePress->setScale(1.);

	SettingUtil* setting = StaticImage::Save();
	setting->setStringForKey("buttonName", mName);
	return setting;
}

bool  SimpleButton::mouseDown(const Vec2& mousePos) {
	bool ret = StaticImage::mouseDown(mousePos);
	if (ret)
		setPress(ret);
	return ret;
}

bool  SimpleButton::mouseUp(const Vec2& mousePos) {
	bool ret = StaticImage::mouseUp(mousePos);
	if (ret) {
		setPress(false);
		if (_callback)_callback(this);
	}
	return ret;
}

void SimpleButton::setPress(bool press) {
	isPress = press;

	_image->setVisible(!isPress);
	_imagePress->setVisible(isPress);

	if (!isLoadedPress)
		_image->setVisible(true);
}

void SimpleButton::setVisible(bool visible) {
	if (_isVisible == visible)return;
	_isVisible = visible;
	if (!_isVisible) {
		Node* need_anim = _image->isVisible() ? _image : _imagePress;

		if (need_anim != _image)
			_image->setVisible(false);
		if (need_anim != _imagePress)
			_imagePress->setVisible(false);


		_prev_scale = need_anim->getScale();
		Vector<FiniteTimeAction*> movieActions;
		movieActions.pushBack(DelayTime::create(.1 + CCRANDOM_0_1()*.2));
		movieActions.pushBack(ScaleTo::create(.2, _prev_scale * 1.1));
		movieActions.pushBack(DelayTime::create(.1));
		movieActions.pushBack(ScaleTo::create(.3, .0));

		need_anim->runAction(Sequence::create(movieActions));
	}
	else {
		//setPress(false);

		Node* need_anim = _image->isVisible() ? _image : _imagePress;

		if (need_anim != _image)
			_image->setVisible(true);
		if (need_anim != _imagePress)
			_imagePress->setVisible(false);

		Vector<FiniteTimeAction*> movieActions;
		movieActions.pushBack(DelayTime::create(.1 + CCRANDOM_0_1()*.2));
		movieActions.pushBack(ScaleTo::create(.3, _prev_scale * 1.1));
		movieActions.pushBack(DelayTime::create(.1));
		movieActions.pushBack(ScaleTo::create(.2, _prev_scale));

		need_anim->runAction(Sequence::create(movieActions));
	}
	if (_defEnableMouse)
		isEnableMouse = _isVisible;
}

void SimpleButton::setText(const std::string& text) {
	if (!m_text) {
		Size t_size(getWidth(), getHeight());
		m_text = Label::createWithTTF(text, "fonts/Marvin.ttf", 25);
		
		m_text->setTextColor(Color4B(
			CCRANDOM_0_1() * 255,
			CCRANDOM_0_1() * 255,
			CCRANDOM_0_1() * 255, 255
		));
		
		_image->addChild(m_text);
	}
	else {
		m_text->setString(text);
	}
}
