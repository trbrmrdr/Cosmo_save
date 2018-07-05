#pragma once
#include "stdafx.h"


class AddButton : public Button
{
public:
	typedef std::function<AddButton*(Ref*, int id)> addEventListener;

	AddButton();
	virtual ~AddButton() {};

	static AddButton* create( const std::string& normalImage, const std::string& selectedImage );

	void addAddEventListener( const addEventListener &callback );

	AddButton* addEvent(size_t id);

private:
	addEventListener _addEventListener;
};