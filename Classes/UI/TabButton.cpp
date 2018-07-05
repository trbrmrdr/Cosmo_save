#include "stdafx.h"
#include "TabButton.h"

TabButton::TabButton() :
	Button(),

	isChecked(false),
	titleName()
{
	setPressedActionEnabled(false);
}

TabButton* TabButton::create(const std::string& normalImage, const std::string& selectedImage, const std::string& disableImage, TextureResType texType, bool check)
{
	TabButton *btn = new (std::nothrow) TabButton;
	if (btn && btn->init(normalImage, selectedImage, disableImage, texType))
	{
		btn->onCheck(check, true);

		btn->autorelease();
		return btn;
	}
	CC_SAFE_DELETE(btn);
	return nullptr;
};

bool TabButton::isCheck()
{
	return isChecked;
}

void TabButton::setText(const std::string& name)
{
	titleName = name;
	setTitleText(titleName);
}

void TabButton::onCheck(bool pressed, bool force)
{
	if (!force)
	{
		if (isChecked == pressed)
			return;
	}
	isChecked = pressed;
	if (pressed)
	{
		Button::onPressStateChangedToPressed();
		if (titleName.empty())
			setTitleText("On");
	}
	else
	{
		Button::onPressStateChangedToNormal();
		if (titleName.empty())
			setTitleText("Off");
	}
}

