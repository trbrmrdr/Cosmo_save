#include "stdafx.h"
#include "StaticImage.h"
#include "DevelopMenu.h"

#define NONE_IMAGE "/UI/none_2.png"

StaticImage::StaticImage(Layer* layer) :
	Soul(layer),
	_imageName(),
	_imageNormal()
{
	_image = EffectSprite::create();
	_image->setBlendFunc(BlendFunc::ALPHA_PREMULTIPLIED);
	if (loadImage(NONE_IMAGE))
	{
		Vec2 pos_def = _layer->convertToNodeSpace(VisibleRect::center());
		setPos(pos_def);
	}

#if HAS_DEVELOP
	int t_z = -1;
	for (auto it : _layer->getChildren())
	{
		t_z = MAX(t_z, it->getLocalZOrder());
	}
	setZ(t_z + 1);
#endif
	_layer->addChild(_image, getZ());
	setLight(false);
}

bool StaticImage::select(bool isParent)
{
	if (!Soul::select(true))
		return false;
#if HAS_DEVELOP
	edit_name = DevelopMenu::Instance().addStringSetting(
		this, 0,
		DevelopTabs_World, "name ",
		([this](const std::string& path)
	{
		if (loadImage(path, ""))
		{
			edit_width->Refresh();
			edit_height->Refresh();
		}
	}),
			([this](void) { return _imageName; }), SeparatorType_Top);

	edit_isOpacity = DevelopMenu::Instance().addFloatSettingFromMinMax(
		this, 0,
		DevelopTabs_World, "opacity ",
		([this](const float opacity)
	{
		setOpacity(opacity);
	}),
			([this]() { return _image->getOpacity(); }),
		0,
		255,
		SeparatorType_Top);



	edit_setFlipX = DevelopMenu::Instance().addBoolSetting(
		this, 0, DevelopTabs_World, "flipX",
		([this](const bool& val) { setFlip(val, false); }),
		([this](void) { return  getFlipX(); }));
	//*
	edit_setNodeName = DevelopMenu::Instance().addStringSetting(
		this, 0, DevelopTabs_World, "nodeName",
		([this](const std::string& val) { setNodeName(val); }),
		([this](void) { return  getNodeName(); }));
	/**/
/*
edit_name_normal = DevelopMenu::Instance().addStringSetting(
	this, 0,
	DevelopTabs_World, "name normal ",
	([this]( const std::string& path )
{
	if (loadImage( _imageName, path ))
	{
	}
}),
	([this]( void ) { return _imageNormal; }), SeparatorType_Top );


edit_isLight = DevelopMenu::Instance().addBoolSetting(
	this, 0,
	DevelopTabs_World, "isLight",
	([this]( const bool& value ) { setLight( value ); }),
	([this]( void ) { return isLight(); }) );
	*/
#endif
	if (!isParent)
		refresh();
	return true;
}

void StaticImage::deselect()
{
	Soul::deselect();
#if HAS_DEVELOP
	edit_name = NULL;
	edit_name_normal = NULL;
	edit_isLight = NULL;
	edit_isOpacity = NULL;
	edit_setFlipX = NULL;
	edit_setNodeName = NULL;
#endif
}

void StaticImage::refresh()
{
	Soul::refresh();
	if (!isSelected)
		return;
#if HAS_DEVELOP
	CALL_VOID_METHOD(edit_name, Refresh());
	CALL_VOID_METHOD(edit_name_normal, Refresh());
	CALL_VOID_METHOD(edit_isLight, Refresh());
	CALL_VOID_METHOD(edit_isOpacity, Refresh());
	CALL_VOID_METHOD(edit_setFlipX, Refresh());
	CALL_VOID_METHOD(edit_setNodeName, Refresh());
#endif
}

void StaticImage::setZ(int val)
{
	Soul::setZ(val);
	_image->setLocalZOrder(val);
}

void StaticImage::setPos(const Vec2 & pos)
{
	Soul::setPos(pos);
	_image->setPosition(pos);
}

float StaticImage::getHeight()
{
	return _image->getContentSize().height * _image->getScaleY();
}

void StaticImage::setHeight(float height)
{
	Soul::setHeight(height);
	_image->setScale(_image->getScaleX(), height / _image->getContentSize().height);
}

float StaticImage::getWidth()
{
	return _image->getContentSize().width *  _image->getScaleX();
}

void StaticImage::setWidth(float width)
{
	Soul::setWidth(width);
	_image->setScale(width / _image->getContentSize().width, _image->getScaleY());
}

void StaticImage::setSize(float width, float height)
{
	Soul::setSize(width, height);
	_image->setScale(width / _image->getContentSize().width,
		height / _image->getContentSize().height);
}

void StaticImage::setOpacity(float opacity)
{
	_image->setOpacity(opacity);
}

void StaticImage::setFlip(bool flipX, bool flipY)
{
	_image->setFlippedX(flipX);
	_image->setFlippedY(flipY);
}
///*
std::string StaticImage::getNodeName()
{
	return _image->getName();
}

void StaticImage::setNodeName(const std::string& name)
{
	if (name.empty())return;
	_image->setName(name);
}
/**/
StaticImage::~StaticImage()
{
	//CCLOG("~StaticImage");
	deselect();
	_image->removeFromParent();

	//CCLOG("~StaticImage_end");
}

void StaticImage::setLight(bool value)
{
	if (_enableLight == value)
		return;
#if LIGHT_ENABLE
	_enableLight = value;
	if (_enableLight)
		LightSystem::Instance().setLightEffect(_image);
	else
		_image->disableEffect();
#endif
};

#if IS_PC || IS_IOS
#define  LOGD(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "CCFileUtils-stage"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif

bool StaticImage::loadImage(const std::string& name, const std::string& name_normal)
{
#if LIGHT_ENABLE
	//LOGD("name == %s    name_normal = %s", name.c_str(), name_normal.c_str());
	bool isNormal = !name_normal.empty(); //&& isLight();
	if (_imageName == name && !isNormal)
		return false;
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(name);
	if (texture)
	{
		_imageName = name;
		_image->initWithFile(name);
		//_image->setAnchorPoint( Vec2::ZERO );
		if (!isNormal)
			return true;
	}
	if (isNormal)
	{
		if (_imageNormal == name_normal)
			return false;
		if (_image->setTexture2(name_normal))
			_imageNormal = name_normal;
	}
#else
	if (_imageName == name)return false;
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(name);
	if (texture) {
		_imageName = name;
		_image->initWithFile(name);
		//loadTexturePress();
		return true;
	}
#endif
	return false;
}

void StaticImage::Load(SettingUtil* setting)
{
	Soul::Load(setting);
	loadImage(setting->getStringForKey("backName", NONE_IMAGE)
#if LIGHT_ENABLE
		, setting->getStringForKey("normalName")
#endif
	);

	Vec2 size = setting->getVec2ForKey("size", Vec2(_image->getContentSize().width, _image->getContentSize().height));

	setSize(size.x, size.y);
#if LIGHT_ENABLE
	setLight(setting->getBoolForKey("isLight"));
#endif
	setOpacity(setting->getFloatForKey("opacity"));

	setFlip(setting->getBoolForKey("flipX"),
		setting->getBoolForKey("flipY"));

	setNodeName(setting->getStringForKey("nodeName"));
}

SettingUtil* StaticImage::Save()
{
	SettingUtil* setting = Soul::Save();
	setting->setStringForKey("backName", _imageName);
	setting->setStringForKey("normalName", _imageNormal);
#if LIGHT_ENABLE
	setting->setBoolForKey("isLight", isLight());
#endif
	setting->setFloatForKey("opacity", _image->getOpacity());
	setting->setBoolForKey("flipX", _image->isFlippedX());
	setting->setBoolForKey("flipY", _image->isFlippedY());

	//setting->setStringForKey("nodeName", _image->getName());
	return setting;
}
