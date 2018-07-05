#include "stdafx.h"
#include "ListView_2.h"


ListView_2* ListView_2::create()
{
	ListView_2* widget = new (std::nothrow) ListView_2();
	if (widget && widget->init())
	{
		widget->autorelease();
		return widget;
	}
	CC_SAFE_DELETE( widget );
	return nullptr;
}

void ListView_2::refresh()
{
	//_refreshViewDirty = true;
	forceDoLayout();
}

void ListView_2::moveDown( Widget* widget )
{
	size_t id = _items.getIndex( widget );
	if (-1 != id)
	{
		widget->retain();
		_items.erase( id );

		_items.pushBack( widget );

		widget->release();

		remedyLayoutParameter( widget );
		//_refreshViewDirty = true;
	}

}


