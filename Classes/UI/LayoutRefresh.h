#pragma once
#include "stdafx.h"


class LayoutRefresh : public Layout
{
public:
	typedef std::function<void( Ref *pSender )> funcRefresh;


	LayoutRefresh();
	virtual ~LayoutRefresh();

	static LayoutRefresh* create();

	void setFuncRefresh( const funcRefresh& func ) { _func = func; };

	void Refresh();
private:
	funcRefresh _func;
};