#pragma once
#include "stdafx.h"
#include "World/World.h"
#include "PhysicsSpline.h"
#include "Objects/SimpleButton.h"
#include "PuzzleImage.h"
#include "UI_cw.h"
class World_CreateWord: public World {
private:
	UI_cw * m_cw_ui;

	void createListener();
	PhysicsSpline* m_physicsSpline;
	Node* m_spline_node;

public:

	World_CreateWord(Layer* layer);

	virtual void init() {};
	virtual void load();

	virtual ~World_CreateWord(void) {
		delete m_cw_ui;
		delete m_physicsSpline;
	}

	virtual void update(float delta) override;

	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

private:

	bool consider_id_level(bool next_level);//true если есть уровень , иначе показываем меню
	void load_menu();
	void loadLevel(int id = -1);
	void create_many_obj();
	void freeLevel();

	void pressRelease(Button* button);

	void bybble_press_release(PhysicsObject* obj);

	int _curr_level;
	int _intricacy_level;

	bool _has_end_game;
	float _start_after;
	bool _has_start;


	float _start_menu_after;
	bool _has_start_menu;

	AnimButton* _prev = NULL;
	AnimButton* _next = NULL;
	AnimButton* _reload = NULL;
	AnimButton* _menu = NULL;
	Node* _right_menu = NULL;

	SettingUtil* _emmitter_setting;
	vector<PhysicsObject*> _obj_letters;
	vector<PhysicsObject*> _obj_end_box;

	vector<PhysicsObject*> _obj_tmp;

	PuzzleImage* _puzz_img;

	vector<string> _letters;

	vector<int> _history_level;

	bool isExit;
	bool isMenu;
};
