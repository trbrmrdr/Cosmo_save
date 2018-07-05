#include "stdafx.h"
#include "UI_dtd.h"
#include "Scenes/GamesCamera.h"
#include "DevelopMenu.h"

UI_dtd::UI_dtd(Layer* layer):
	UI_canvas(layer, false) {

	_bt_menu->removeFromParent();
	_bt_menu = NULL;
}

UI_dtd::~UI_dtd() {}
