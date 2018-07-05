#include "stdafx.h"
#include "World_MainMenu.h"

#define WORLD_MM_FILE_NAME "world_main_menu.xml" 

#define BUTTON_CW "CREATE_WORLD"
#define BUTTON_PTP "PAINT_TO_POINT"

//#########################################

void World_MainMenu::init() {
	_setting = new SettingUtil(WORLD_MM_FILE_NAME, true);
}

void World_MainMenu::load() {
	//World::load();

	m_mm_ui = new UI_mm(_layer);
	m_mm_ui->setPressCallback(CC_CALLBACK_1(World_MainMenu::pressRelease, this));
	m_mm_ui->getButtons(&m_dtd, &m_cw);
}

void World_MainMenu::pressRelease(Button* button) {
	m_button_release = dynamic_cast<AnimButton*>(button);
}

void World_MainMenu::update(float delta) {
	//isChangedSpline();
	//World::update(delta);
#if 0
	if (!m_cw) {
		for (auto& it : _listObj) {
			Soul* soul = it.second;

			if (soul->getType() == SoulType_Button) {
				SimpleButton* tmp = (SimpleButton*)soul;
				tmp->setPressCallback(CC_CALLBACK_1(World_MainMenu::pressRelease, this));
				if (tmp->getName() == BUTTON_CW) {
					m_cw = tmp;
				}
				else if (tmp->getName() == BUTTON_PTP) {
					m_ptp = tmp;
				}
				continue;
			}
		}
	}
	//______________________
	if (!m_button_release)return;
#if 0
	GamesScene::ChangeToScene(GamesScene::SceneNames::SN_MainMenu);

	m_button_release = NULL;
	return;
#endif

#endif
	if (m_cw == m_button_release) {
		GamesScene::ChangeToScene(GamesScene::SceneNames::SN_Game_CreateWord);
	}
	else if (m_dtd == m_button_release) {
		GamesScene::ChangeToScene(GamesScene::SceneNames::SN_Game_DrawToDots);
	}
	m_button_release = NULL;

}
