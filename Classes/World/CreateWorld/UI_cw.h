#pragma once
#include "stdafx.h"
#include "../UI_canvas.h"


class UI_cw: public UI_canvas {
public:
	static const std::string NAME_BACKGROUND_2_NODE;
	UI_cw(Layer* layer);
	virtual ~UI_cw();

	void getElements(Node** r_node, AnimButton**exit) {
		*r_node = _white_r_layout;
		*exit = _bt_exit;
	}

	Rect getRectL() { return _inner_rect_l; }
	Rect getRectR() { return _inner_rect_r; }
	float getDlh() { return d_lh; }
	virtual float getScaleUI() override { return _scale_ui / .1f; }


	void addBack(const std::string& filename);
	void rmBack();
};
