#pragma once
#include "stdafx.h"
#include "../UI_canvas.h"


class UI_dtd: public UI_canvas {

public:
	UI_dtd(Layer* layer);
	virtual ~UI_dtd();

	void getButtons(AnimButton**left, AnimButton**right, AnimButton**menu, AnimButton**exit) {
		*left = _bt_prev;
		*right = _bt_next;
		*menu = _bt_menu;
		*exit = _bt_exit;
	}
};
