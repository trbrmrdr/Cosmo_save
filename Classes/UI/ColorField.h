#pragma once
#include "stdafx.h"


class ColorField : public ControlColourPicker
{
public:

	typedef std::function<void(Ref*, Control::EventType controlEvent)> addEventListener;

	ColorField();
	virtual ~ColorField() {};

	static ColorField* create();

	void addAddEventListener( const addEventListener& callback );

private:
	addEventListener _callback;

	void colourValueChanged( cocos2d::Ref* sender, cocos2d::extension::Control::EventType controlEvent );
};