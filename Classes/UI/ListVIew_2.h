#pragma once
#include "stdafx.h"

class ListView_2 : public ListView
{
public:

	ListView_2() :ListView() {};
	virtual ~ListView_2() {};

	static ListView_2* create();

	void moveDown(Widget* widget);

	void refresh();
};
