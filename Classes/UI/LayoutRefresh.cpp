#include "stdafx.h"
#include "LayoutRefresh.h"



LayoutRefresh::LayoutRefresh() :
	Layout(),
	_func(nullptr)
{}

LayoutRefresh::~LayoutRefresh()
{
	_func = nullptr;
};

LayoutRefresh* LayoutRefresh::create()
{
	LayoutRefresh* layout = new (std::nothrow) LayoutRefresh();
	if (layout && layout->init())
	{
		layout->autorelease();
		return layout;
	}
	CC_SAFE_DELETE(layout);
	return nullptr;
}

void LayoutRefresh::Refresh()
{
	if (nullptr != _func)
	{
		_func(this);
	}
}


