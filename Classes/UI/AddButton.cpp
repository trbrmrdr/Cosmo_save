#include "stdafx.h"
#include "AddButton.h"



AddButton::AddButton() :
	Button(),
	_addEventListener(nullptr)
{}

AddButton* AddButton::create(const std::string& normalImage, const std::string& selectedImage)
{
	AddButton* button = new (std::nothrow) AddButton();
	if (button && button->init(normalImage, selectedImage))
	{
		button->autorelease();
		return button;
	}
	CC_SAFE_DELETE(button);
	return nullptr;
}


void AddButton::addAddEventListener(const addEventListener &callback)
{
	_addEventListener = callback;
	addClickEventListener(([this](Ref* data) { _addEventListener(data, -1); }));
}

AddButton* AddButton::addEvent(size_t id)
{
	if (nullptr == _addEventListener)
		return NULL;
	return _addEventListener(this, id);
}



