#pragma once
#include "stdafx.h"
#include "Soul.h"
#include "UI/LayoutRefresh.h"
#include "Effects/EffectSprite.h"
#include "StaticImage.h"

class SimpleButton: public StaticImage {
	Label* m_text;
public:
	SimpleButton(Layer* layer);
	virtual ~SimpleButton();
	virtual SoulType getType() { return SoulType_Button; };
	virtual void Load(SettingUtil* setting);
	virtual SettingUtil* Save();

	virtual bool loadImage(const std::string& name, const std::string& name_normal = "");

	virtual bool select(bool isParent = false);
	virtual void deselect();
	virtual void refresh();

	virtual void setName(const std::string& name) { mName = name; };
	virtual std::string getName() { return mName; };

	virtual bool mouseDown(const Vec2& mousePos);
	virtual bool mouseUp(const Vec2& mousePos);

	virtual void setPos(const Vec2& pos) {
		StaticImage::setPos(pos);
		_imagePress->setPosition(pos);
	}

	virtual void setFlip(bool flipX, bool flipY) {
		StaticImage::setFlip(flipX, flipY);
		_imagePress->setFlippedX(flipX);
		_imagePress->setFlippedY(flipY);
	}

	virtual void setZ(int val) {
		StaticImage::setZ(val);
		_imagePress->setLocalZOrder(val);
	}

	void setPress(bool press);

    typedef std::function<void(SimpleButton* button)> onPressRelease;

    void setPressCallback(const onPressRelease& callback) { _callback = callback; }

    void setVisible(bool visible);

	void setSize(float width, float height);

	void setText(const std::string& text);
protected:
    onPressRelease _callback;

	Sprite* _imagePress;
	std::string mName;
	bool isPress;
#if HAS_DEVELOP
	LayoutRefresh* edit_setName = NULL;
#endif
private:
    bool _isVisible;
	bool isLoadedPress;
	void loadTexturePress();
    float _prev_scale;
};