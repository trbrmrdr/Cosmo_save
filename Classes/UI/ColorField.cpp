#include "stdafx.h"
#include "ColorField.h"

ColorField::ColorField() :
	ControlColourPicker(),
	_callback(nullptr)
{}

ColorField* ColorField::create()
{
	ColorField *pRet = new (std::nothrow) ColorField();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}

	CC_SAFE_DELETE(pRet);
	return nullptr;
}

void ColorField::addAddEventListener(const addEventListener& callback)
{
	_callback = callback;
	addTargetWithActionForControlEvent(this, cccontrol_selector(ColorField::colourValueChanged), Control::EventType::VALUE_CHANGED);
}

void ColorField::colourValueChanged(cocos2d::Ref* sender, cocos2d::extension::Control::EventType controlEvent)
{
	if (nullptr != _callback)
		_callback(sender, controlEvent);
}
