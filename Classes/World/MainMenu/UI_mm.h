#pragma once
#include "stdafx.h"
#include "../UI_canvas.h"

class AnimHill;
class Cloud;

class UI_mm: public UI_canvas {
private:
	AnimButton * but_dtd;
	AnimButton* but_cw;

	Node* shake_process;
	std::vector<AnimHill*> hills;
	std::vector<Cloud*> clouds;
public:
	static int _cloudsZOreder;
	static int _hillsZOreder;

	UI_mm(Layer* layer);
	virtual ~UI_mm();

	void getButtons(AnimButton**draw_to_dots, AnimButton**create_word) {
		*create_word = but_cw;
		*draw_to_dots = but_dtd;
	}
	void show_hill();
	void startShake();

	virtual void touchEvent(Ref*ref, Widget::TouchEventType eventType) override;
};
