#pragma once
#include "stdafx.h"

class AnimButton: public Button {
private:
	float _prev_scale = -1.;
	bool _visible_from_process;

public:
	virtual void setVisible(bool visible) override;
	void setVisible(bool visible,float delay, bool force);
	void shake();
	void hide();
	AnimButton():_visible_from_process(true), Button() {}

	static AnimButton* create(const std::string &normalImage, const std::string& selectedImage) {
		AnimButton *btn = new (std::nothrow) AnimButton;
		if (btn && btn->init(normalImage, selectedImage)) {
			btn->autorelease();
			return btn;
		}
		CC_SAFE_DELETE(btn);
		return nullptr;
	}


	void virtual setScale(float scale) { Button::setScale(scale); _prev_scale = scale; }

};

class UI_canvas {
public:
	typedef std::function<void(Button* button)> onPressRelease;

	static int _backgroundZOreder;
	static int _selectLevelZOreder;
	static int _canvasZOrder;
	static int _buttonZOrder;

	static const std::string NAME_BACKGROUND_NODE;
	static const std::string NAME_CANVAS_NODE;

protected:
	Layer * _layer;
	onPressRelease _callback;

	AnimButton* _bt_menu;
	AnimButton* _bt_exit;
	AnimButton* _bt_next;
	AnimButton* _bt_prev;

	Size _size;
	Size _inner_size;
	Rect _inner_rect_l;
	Rect _inner_rect_r;
	float d_lh;

	Sprite* _background;
	//Node* _canvasNode;
	float _scale_ui;



	Node* _c;
	Vec2 _pos_c;
	ClippingRectangleNode* _clip_node_c;
	DrawNode* _white_r_layout;
	Vec2 _d_pos_from_white_r_l;
public:
	UI_canvas(Layer* layer);
	UI_canvas(Layer* layer, bool hasC);
	virtual ~UI_canvas();

	void setPressCallback(const onPressRelease& callback) { _callback = callback; }

	Size getInnerSize() { return _inner_size; };
	Size getSize() { return _size; };

	virtual void touchEvent(Ref*ref, Widget::TouchEventType eventType);

	virtual float getScaleUI() { return _scale_ui; }

	void setVisiblePanel(bool visible, bool force = false);
protected:
	AnimButton * createBtn(Node* layer, float scale, const std::string name, bool flipX = false);
	Node* createCanvas(Node* layer, const Size& size, float scale, bool hasC = false);
};
