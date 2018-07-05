#pragma once
#include "stdafx.h"
#include "../World.h"

#include "Objects/SimpleButton.h"
#include "Scenes/GamesScene.h"
#include "UI_mm.h"

class World_MainMenu : public World {
private:
	AnimButton * m_cw = NULL;
	AnimButton* m_dtd = NULL;
	AnimButton* m_button_release;

	UI_mm* m_mm_ui;
public:

	World_MainMenu(Layer* layer) :
		World(layer),
		m_button_release(NULL)
	{}

	virtual void init();
	virtual void load();
	virtual ~World_MainMenu(void) {}


	virtual void update(float delta);

	void pressRelease(Button* button);

};