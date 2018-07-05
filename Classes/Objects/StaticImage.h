#pragma once
#include "stdafx.h"
#include "Soul.h"
#include "UI/LayoutRefresh.h"
#include "Effects/EffectSprite.h"
#include "LightSystem/LightSystem.h"

class StaticImage : public Soul
{
public:
	StaticImage(Layer* layer);
	virtual ~StaticImage();
	virtual SoulType getType() { return SoulType_StaticImage; };
	virtual void Load(SettingUtil* setting);
	virtual SettingUtil* Save();

	virtual bool loadImage(const std::string& name, const std::string& name_normal = "");

	virtual bool select(bool isParent = false);
	virtual void deselect();
	virtual void refresh();

	void setLight(bool value);
	bool isLight() { return _enableLight; };

	virtual float getHeight();
	virtual float getWidth();
	virtual bool getFlipX() { return _image->isFlippedX(); };

	virtual void setZ(int val);
	virtual void setPos(const Vec2& pos);
	virtual void setHeight(float height);
	virtual void setWidth(float width);
	virtual void setSize(float width, float height);
	virtual void setOpacity(float opacity);

	virtual void setFlip(bool flipX, bool flipY);

	///*
	virtual void setNodeName(const std::string& name);
	virtual std::string getNodeName();
	/**/
	virtual EffectSprite* getImage() { return _image; };

protected:

	EffectSprite* _image;
	std::string _imageName;

	std::string _imageNormal;
	bool _enableLight;
#if HAS_DEVELOP
	LayoutRefresh* edit_name = NULL;
	LayoutRefresh* edit_name_normal = NULL;
	LayoutRefresh* edit_isLight = NULL;
	LayoutRefresh* edit_isOpacity = NULL;

	LayoutRefresh* edit_setFlipX = NULL;
	LayoutRefresh* edit_setNodeName = NULL;
	//LayoutRefresh* edit_setFlipY = NULL;
#endif
};