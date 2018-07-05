#include "stdafx.h"
#include "UI_cw.h"
#include "Scenes/GamesCamera.h"
#include "DevelopMenu.h"

const std::string UI_cw::NAME_BACKGROUND_2_NODE = "background_2";

UI_cw::UI_cw(Layer* layer):
	UI_canvas(layer, true) {

	//этот экран под в  
	//Size(192.0, 108.0)

#define	DEL_BTN(node) {node->removeFromParent(); node= NULL;}
	DEL_BTN(_bt_next);
	DEL_BTN(_bt_prev);

	DEL_BTN(_bt_menu);
	//(dynamic_cast<AnimButton*>(_bt_menu))->setForceVisible(false);
}

UI_cw::~UI_cw() {}


void UI_cw::addBack(const std::string& filename) {
	Sprite* t_sprite = Sprite::create(filename);
	if (t_sprite) {
		t_sprite->setScale(_scale_ui);
		_layer->addChild(t_sprite, _backgroundZOreder + 1, NAME_BACKGROUND_2_NODE);
	}
}

void UI_cw::rmBack() {
	CALL_VOID_METHOD(_layer->getChildByName(NAME_BACKGROUND_2_NODE), removeFromParent());
}