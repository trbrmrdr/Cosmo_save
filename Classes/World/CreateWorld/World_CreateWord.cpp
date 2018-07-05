#include "stdafx.h"
#include "Scenes/GamesScene.h"
#include "World_CreateWord.h"
#include "PhysicsSpline.h"


#define WORLD_FILE_NAME "world_create_word.xml"
#define FILE_NAME_SPLINE "spline_cw.txt"

#define NODE_NAME_LEVEL_BACK "level_back"

#define BUTTON_NEXT		"BUTTON_NEXT"
#define BUTTON_PREV		"BUTTON_PREV"
#define BUTTON_RELOAD	"BUTTON_RELOAD"
#define BUTTON_MENU		"BUTTON_MENU"

#define SCALE "_scale"

struct letter {
	string _let;
	Color4B _color;
	string _name;

	letter(const string& let, const Color4B& color, const string& name):
		_let(let), _color(color), _name(name) {}

};

map<string, Color4B> _color_map;// "0CFB8F" Color4B(16, 200, 146, 255)
struct level {
	std::string _path_to_level;
	std::vector<letter> _letters;
	float _scale;
	float _dix;
	Vec2 _dx;

	bool _has_complete = false;

	level() {
		_letters = {
			letter("Рђ", Color4B(16, 200, 146, 255), "0CFB8F"),
			letter("Р ", Color4B(200, 16, 200, 255), "FF1117"),
			letter("Р‘", Color4B(200, 200, 14, 255), "FCEA0D"),
			letter("РЈ", Color4B(16, 155, 200, 255), "0D98FC"),
			letter("Р—", Color4B(200, 16, 200, 255), "E30CFB")
		};
	}

	void init(const std::vector<level>& levels) {
		_letters.clear();
		vector<int> rand_colors;
		for (auto& it : levels) {
			//todo
			if (it._has_complete)continue;
			std::string num_str = StringUtils::format("%i", it._letters.size());
			bool find = false;
			for (auto& it : _letters) {
				if (0 == it._let.compare(num_str)) {
					find = true;
					break;
				}
			}
			if (!find) {
				int id_col = RAND_I(0, _color_map.size() - 1);
				bool col_valid;
				do {
					col_valid = true;
					for (auto& it : rand_colors) {
						if (it == id_col) {
							col_valid = false;
							if (++id_col >= _color_map.size())id_col = 0;
							break;
						}
					}
				}
				while (!col_valid);
				rand_colors.push_back(id_col);
				map<string, Color4B>::iterator it = _color_map.begin();
				for (; id_col > 0; --id_col, ++it);

				_letters.push_back(letter(num_str, it->second, it->first));
			}
		}
	}

	static vector<string> trim(const std::string& str, char ch) {
		vector<string> ret;
		int ic, ic_p = 0;
		while (true) {
			ic = str.find_first_of(ch, ic_p);
			if (ic >= 0) {
				ret.push_back(str.substr(ic_p, ic - ic_p));
				ic_p = ic + 1;
				continue;
			}
			break;
		}
		return ret;
	}

	level(SettingUtil* setting, const std::string& curr_lang, float scale) {
		_path_to_level = setting->getStringForKey("folder") + "/";

		std::string s_setting = setting->getStringForKey(std::string("settings" + curr_lang).c_str());
		int ic = 0;
		int ic_p = 0;
		while (true) {
			ic = s_setting.find_first_of('\n', ic_p);
			if (ic == -1)break;
			int count = ic - ic_p;
			if (count > 0) {
				std::vector<std::string> t_letters = trim(s_setting.substr(ic_p, count), ';');
				//__________________
				if (t_letters.size() == 5) {
					pair< string, Color4B> item = pair < string, Color4B>(t_letters[4], Color4B(
						CCString(t_letters[1]).intValue(),
						CCString(t_letters[2]).intValue(),
						CCString(t_letters[3]).intValue(),
						255
					));
					_color_map.insert(item);
					_letters.push_back(letter(t_letters[0], item.second, item.first));
				}
				else {
					map<string, Color4B>::iterator it = _color_map.find(t_letters[1]);
					if (it == _color_map.end())
						_letters.push_back(letter(t_letters[0], Color4B::BLACK, t_letters[1]));
					else
						_letters.push_back(letter(t_letters[0], it->second, it->first));
				}
				//__________________
			}
			ic_p = ic + 1;
			continue;
		}
		//4 - .3
		//5 - .24
		//6 - .23
		//8 - .18
		//.3+.7
		switch (_letters.size()) {
			case 1:
			case 4:
				_scale = .3f;
				_dix = 6.4f;
				_dx = Vec2(-63.53f, -34.f);
				break;
			case 5:
				_scale = .24f;
				_dix = 5.3f;
				_dx = Vec2(-67.49f, -33.f);
				break;
			case 6:_scale = .23f;
				_dix = 5.f;
				_dx = Vec2(-75.89f, -32.f);
				break;
			case 7:_scale = .20f;
				_dix = 4.2f;
				_dx = Vec2(-75.9f, -32.5f);
				break;
			case 8:_scale = .18f;
				_dix = 3.8f;
				_dx = Vec2(-76.f, -33.f);
				break;
			default:
				_dix = _scale = 1.f;
				_dx = Vec2::ZERO;
		}
		_dix *= scale;
		_dx *= scale;
		_scale *= scale;

	}
};

float _dix_pos = 6.4f;
//float _dix2_pos;
Vec2 _d_pos(0.f, 60.f);
std::vector<level> _levels;
level _level_menu;

bool _test = false;


World_CreateWord::World_CreateWord(Layer* layer):
	_curr_level(1),
	_intricacy_level(-1),

	isExit(false),
	isMenu(false),

	World(layer),
	_has_end_game(true),
	_has_start(false),
	_has_start_menu(false),

	_start_after(-1.),
	_start_menu_after(-1.),
	_emmitter_setting(NULL),
	_puzz_img(NULL) {

	_levels.clear();
#if 0
	DevelopMenu::Instance().addPointSetting(this, 0, DevelopTabs_World, "_d_pos",
		([this](const Vec2 val) {_d_pos = val; _has_start = true; }),
											([this](void) {return _d_pos; }));

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "test_dix",
		([this](const float val) {_dix_pos = val; _has_start = true; }),
											([this](void) {return _dix_pos; }));

	DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "test_dix2",
		([this](const float val) {_dix2_pos = val; _has_start = true; }),
											([this](void) {return _dix2_pos; }));

#endif
}

void World_CreateWord::createListener() {
	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = ([this](Touch* touch, Event* event) {
		Vec2 pos = m_spline_node->convertToNodeSpace(touch->getLocation());
		if (m_physicsSpline->mouseDown(pos)) return true;
		//if (m_finishImage->mouseDown(pos)) return true;
		return false;
	});

	listener->onTouchMoved = ([this](Touch* touch, Event* event) {
		Vec2 pos = m_spline_node->convertToNodeSpace(touch->getLocation());
		if (m_physicsSpline->mouseMovie(pos)) return;
		//if (m_finishImage->mouseMovie(pos)) return;
		return;
	});

	listener->onTouchEnded = ([this](Touch* touch, Event* event) {
		Vec2 pos = m_spline_node->convertToNodeSpace(touch->getLocation());
		if (m_physicsSpline->mouseUp(pos)) return;
		//if (m_finishImage->mouseUp(pos)) return;
		return;
	});

	m_spline_node->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_spline_node);
}
float scale_ui;
void World_CreateWord::load() {
	m_cw_ui = new UI_cw(_layer);
	m_cw_ui->setPressCallback(CC_CALLBACK_1(World_CreateWord::pressRelease, this));
	m_cw_ui->getElements(&_right_menu, &_menu);
	m_cw_ui->setVisiblePanel(false, true);
	//______________________________
	m_spline_node = Node::create();
	_layer->addChild(m_spline_node);
	//__
	createListener();
	//__
	m_physicsSpline = new PhysicsSpline(m_spline_node, m_cw_ui->getRectL(), m_cw_ui->getRectR(), m_cw_ui->getDlh(), this);
	//m_physicsSpline->load(FILE_NAME_SPLINE);
	//_______________________________________

	std::string curr_lang = "_ru";
	SettingUtil* t_slevels = SettingUtil::createIsFile("levels.xml");
	int count = t_slevels->getIntegerForKey("LevelCount");
	for (int i = 0; i < count; ++i) {
		SettingUtil* t_level = t_slevels->getSettingForKey(StringUtils::format("Level_%u", i).c_str());
		_levels.push_back(level(t_level, curr_lang, m_cw_ui->getScaleUI()));
		delete t_level;
	}
	delete t_slevels;

	_letters = level::trim(FileUtils::getInstance()->getStringFromFile("letters" + curr_lang + ".txt"), '\n');

	if (_intricacy_level == -1) {
		_has_start_menu = true;
	}
	else {
		_has_start = true;
	}
	//_______________________________________
	_puzz_img = new PuzzleImage(_right_menu, m_cw_ui->getRectR());
	//_________________________________________________________________________
	_setting = new SettingUtil(WORLD_FILE_NAME, true);//остался только шарик
	World::load();
    
    scale_ui = m_cw_ui->getScaleUI();
    
#if HAS_DEVELOP
	DevelopMenu::Instance().changeKeyEvents(

		([this](EventKeyboard::KeyCode keyCode, KeyState keyState) {

#define SET_LEVEL(id)          \
      if (keyCode == EventKeyboard::KeyCode((int)EventKeyboard::KeyCode::KEY_0 + id))\
           if (keyState == KeyState_RELEASE) {       \
             _curr_level = id;                      \
               return true;                          \
           }

		SET_LEVEL(0);
		SET_LEVEL(1);
		SET_LEVEL(2);
		SET_LEVEL(3);
		SET_LEVEL(4);
		SET_LEVEL(5);
		SET_LEVEL(6);
		SET_LEVEL(7);
		SET_LEVEL(8);

		if (keyCode == EventKeyboard::KeyCode::KEY_Z)
			if (keyState == KeyState_RELEASE) {
				_has_start = true;
				//_curr_level = 1;
				//_has_start_menu = true;
				//startLevel();
				return true;
			}
		//_________________
		if (keyCode == EventKeyboard::KeyCode::KEY_X) {
			if (keyState == KeyState::KeyState_RELEASE) {
				_has_start_menu = true;
			}
			return true;
		}
		//_________________
		return false;
	}), this);
#endif

	//_curr_level = 4;
	//_has_start = true;
	//_has_start_menu = false;
	update(.0);
}

void World_CreateWord::load_menu() {
	isMenu = true;

	m_cw_ui->setVisiblePanel(false);
	m_physicsSpline->load(PhysicsSpline::Type_World::TW_Big);
	float prev_scale = _emmitter_setting->getFloatForKey(SCALE);
	_emmitter_setting->setFloatForKey(SCALE, prev_scale*m_cw_ui->getScaleUI());

	_level_menu.init(_levels);
	if (_level_menu._letters.empty()) {
		_history_level.clear();
		for (auto& it : _levels) {
			it._has_complete = false;
		}
	}
	_level_menu.init(_levels);
	std::vector<letter> letters = _level_menu._letters;


	int i = 0;
	_obj_letters.clear();

	Rect big_rect = Rect(m_cw_ui->getRectL().origin, Size(m_cw_ui->getRectL().size.width + m_cw_ui->getRectR().size.width, m_cw_ui->getRectR().size.height));
	float d_i = big_rect.size.width / (letters.size()*1.1);//сдвиг на 1 шарик
	float d_j = big_rect.size.height*.15;//начальная позиция повыше
	float d_jh = big_rect.size.height - d_j * 2.;//сдвиг по высоте максимальный

	for (letter let : letters) {
		i++;

		PhysicsObject* obj = addSoul<PhysicsObject>(_emmitter_setting);

		obj->setPos(Vec2(big_rect.origin.x + i * d_i,
						 big_rect.origin.y + d_j + d_jh * CCRANDOM_0_1())
		);

		obj->setBubble(let._let, let._color, let._name);
		obj->active_to_button(CC_CALLBACK_1(World_CreateWord::bybble_press_release, this));
		_obj_letters.push_back(obj);
	}

	int size = _obj_letters.size();
	for (int i = 0; i < size; ++i) {
		//int r = i + rand() % ( size - i );
		int r = RAND_I(0, size - 1);
		if (i == r)continue;
		std::swap(_obj_letters[i], _obj_letters[r]);
	}

	_emmitter_setting->setFloatForKey(SCALE, prev_scale);
}

void World_CreateWord::loadLevel(int id) {
	isMenu = false;
	_curr_level = id;
	if (id >= _levels.size() || id < 0)
		_curr_level = 0;
	level t_level = _levels[_curr_level];
	std::vector<letter> letters = t_level._letters;

	m_cw_ui->setVisiblePanel(true);
	m_physicsSpline->load(PhysicsSpline::Type_World::TW_Small);
	//m_physicsSpline->addSpline(t_level._path_to_level + "spline.txt");
	CALL_VOID_METHOD(_puzz_img, setImage(t_level._path_to_level + "finish.png"));
	m_cw_ui->addBack(t_level._path_to_level + "l_back.png");

	float prev_scale = _emmitter_setting->getFloatForKey(SCALE);
	_emmitter_setting->setFloatForKey(SCALE, t_level._scale);
	int i = -1;
	_obj_letters.clear();
	_obj_end_box.clear();

	//from bubble
	Rect big_rect = m_cw_ui->getRectL();
	float d_i = big_rect.size.width / (letters.size()*1.1);//сдвиг на 1 шарик
	float d_j = big_rect.size.height*.15;//начальная позиция повыше
	float d_jh = big_rect.size.height - d_j * 2.;//сдвиг по высоте максимальный
	//from endbox
	float d_pos_x = m_cw_ui->getRectL().size.width*.9 / letters.size();
	Vec2 s_pos = m_cw_ui->getRectL().origin;

	for (letter let : letters) {
		i++;
		PhysicsObject* obj = addSoul<PhysicsObject>(_emmitter_setting);

		obj->setPos(Vec2(big_rect.origin.x + (i + 1) * d_i,
						 big_rect.origin.y + d_j + d_jh * CCRANDOM_0_1())
		);

		obj->setBubble(let._let, let._color, let._name);

		PhysicsObject* end_box = addSoul<PhysicsObject>(_emmitter_setting);
		end_box->setEndBox(obj);

		Vec2 t_pos = s_pos + Vec2(d_pos_x, .0) * i + Vec2(d_pos_x * .5, end_box->getHeight()*.8);
		end_box->setPos(t_pos);
		_obj_end_box.push_back(end_box);

		_obj_letters.push_back(obj);
	}

	_emmitter_setting->setFloatForKey(SCALE, prev_scale);
}

void World_CreateWord::create_many_obj() {
	level t_level = _levels[_curr_level];
	float def_scale = t_level._scale;
	//scale .1 == 5 inc in width
	//_emmitter_setting->setFloatForKey(SCALE, def_scale + RandomHelper::random_real(-.1, +.1));
	float max_height = 29;

	int count_i = t_level._letters.size();

	t_level._dix *= m_cw_ui->getScaleUI();
	t_level._dx *= m_cw_ui->getScaleUI();
	vector<PhysicsObject*>::iterator t_obj_it = _obj_letters.begin();
	max_height *= m_cw_ui->getScaleUI();

	//from endbox
	float d_pos_x = m_cw_ui->getRectL().size.width*.9 / count_i;
	Vec2 s_pos = m_cw_ui->getRectL().origin;

	float prev_scale = _emmitter_setting->getFloatForKey(SCALE);
	_obj_tmp.clear();
	for (int i = 0; i < count_i; i++) {
		int c_i_let = t_obj_it == _obj_letters.end() ? -1 : RandomHelper::random_int(1, 3);
		for (int j = 1; j <= 4; j++) {
			PhysicsObject* tmp_bybble = NULL;
			bool hasActive = c_i_let-- == 0;
			if (hasActive) {
				tmp_bybble = (*t_obj_it++);
			}
			else {
				string t_let = _letters[RandomHelper::random_int(0, 30)];
				map<string, Color4B>::iterator t_col = _color_map.begin();
				for (int i = RandomHelper::random_int(0, (int)_color_map.size() - 1); i > 0; i--, t_col++);

				_emmitter_setting->setFloatForKey(SCALE, (def_scale + RandomHelper::random_real(-.1, +.1))* m_cw_ui->getScaleUI());

				tmp_bybble = addSoul<PhysicsObject>(_emmitter_setting);
				tmp_bybble->setBubble(t_let, t_col->second, t_col->first);

				_obj_tmp.push_back(tmp_bybble);
			}
			tmp_bybble->setTemplate(hasActive, RandomHelper::random_real(4., 8.));

			Vec2 t_pos = s_pos + Vec2(d_pos_x * i, max_height * -j) + Vec2(d_pos_x * .5, .0);
			tmp_bybble->setPos(t_pos);
		}
	}

	_emmitter_setting->setFloatForKey(SCALE, prev_scale);
}

void World_CreateWord::freeLevel() {
	_disable_save = true;

	_obj_tmp.clear();
	_obj_end_box.clear();
	_obj_letters.clear();
	for (std::vector<std::pair<size_t, Soul*> >::iterator it = _listObj.begin(); it != _listObj.end();) {
		Soul* soul = (*it).second;
		SoulType type = (SoulType)soul->getType();
		if (type == SoulType_PhysicsObject) {
			if (!_emmitter_setting) _emmitter_setting = soul->Save();
			it = _listObj.erase(it);
			delete soul;
			if (it == _listObj.end())break;
			continue;
		}
		++it;
	}

	m_cw_ui->rmBack();
}

void World_CreateWord::pressRelease(Button* button) {
	float hide = .0;
	bool not_level = false;
	if (_menu == button) {
		if (isMenu) {
			isExit = true;
		}
		else {
			hide = _start_menu_after = .5;
			//_menu->setForceVisible(false);
			_menu->setVisible(false, .0, true);

			for (auto& it : _obj_end_box)it->play_anim_hide(.0);
			for (auto& it : _obj_letters)it->play_anim_prev();
			for (auto& it : _obj_tmp)it->play_anim_prev();

			CALL_VOID_METHOD(_puzz_img, hide());
		}
	}

#if 0//old todo
	if (button == _prev) {
		_start_after = 1.5;
		for (auto& it : _obj_end_box)it->play_anim_hide(.0);
		for (auto& it : _obj_letters)it->play_anim_prev();
		for (auto& it : _obj_tmp)it->play_anim_prev();

		not_level = !consider_id_level(false);
	}
	else if (button == _next) {
		_start_after = 1.5;
		for (auto& it : _obj_end_box)it->play_anim_hide(.0);
		for (auto& it : _obj_letters)it->play_anim_next();
		for (auto& it : _obj_tmp)it->play_anim_next();
		not_level = !consider_id_level(true);
	}
	else if (button == _reload) {
		hide = _start_after = 1.5;
	}
	else if (button == _menu) {
		if (isMenu) {
			isExit = true;
		}
		else {
			hide = _start_menu_after = 1.5;
			_finish_image->hide();
		}
	}

	if (not_level) {
		_start_after = -1.;
		_start_menu_after = 1.5;
		_finish_image->hide();
	}
	else if (hide > .0) {
		for (auto& it : _obj_end_box)it->play_anim_hide(hide);
		for (auto& it : _obj_letters)it->play_anim_hide(hide);
		for (auto& it : _obj_tmp)it->play_anim_hide(hide);
	}
#endif
}

bool World_CreateWord::consider_id_level(bool next_level) {
	if (next_level) {
		int i = 0;
		for (auto& it : _levels) {
			if (!it._has_complete &&
				_intricacy_level == it._letters.size()) {
				_curr_level = i;
				return true;
			}
			++i;
		}
		return false;
	}
	else {
		if (_history_level.empty()) {
			return false;
		}
		else {
			vector<int>::iterator it = _history_level.end() - 1;
			_curr_level = *it;
			_history_level.erase(it);
			if (_curr_level == -1)
				return false;
		}
	}
	return true;
}

void World_CreateWord::bybble_press_release(PhysicsObject* obj) {
	_start_after = 1.5;
	//_menu->setForceVisible(false);
	_menu->setVisible(false, .0, true);
	for (auto& it : _obj_letters) {
		if (obj == it)continue;
		it->play_anim_hide(_start_after);
	}
	_intricacy_level = String(obj->getLet()).intValue();
	//_level_menu
	if (!consider_id_level(true)) {
		DebugBreak();
	}
}

void World_CreateWord::update(float delta) {
	//___________
	CALL_VOID_METHOD(_puzz_img, update(delta));
	//________
	m_physicsSpline->update(delta);
	m_physicsSpline->setScaleFromDraw(1. / _layer->getScale());
	//_______________________________________  ожидаем сброс и начало на сборку игры
	if (_start_after > .0 && (_start_after -= delta) <= .0) {
		_start_after = -1.;
		_has_start = true;
	}

	if (_start_menu_after > .0 && (_start_menu_after -= delta) <= .0) {
		_start_menu_after = -1.;
		_has_start_menu = true;
	}
	//_______________________________________ создаём меню
	if (_has_start_menu) {
		_has_start = _has_start_menu = false;
		freeLevel();
		load_menu();
		_menu->setVisible(true, .0, false);
	}
	//_______________________________________ запускаем сборку игры
	if (_has_start) {
		_has_start_menu = _has_start = false;
		_has_end_game = false;
		freeLevel();
		loadLevel(_curr_level);
		_menu->setVisible(true, .0, false);

		create_many_obj();
		CALL_VOID_METHOD(_puzz_img, play_anim(3.5));
		//надо пересоздать мир, для смены позиции под экран
		m_physicsSpline->update(.0);
	}
	//_______________________________________ скрытие временных если игра готова к старту
	if (!_obj_tmp.empty()) {
		bool all_active = true;
		for (auto& it : _obj_letters) {
			if (it->hasCheckOn()) {
				all_active = false;
				break;
			}
		}
		if (all_active) {
			for (auto& it : _obj_tmp)
				it->setAStopPhysics(true);
			_obj_tmp.clear();
		}
	}
	//_______________________________________ окончание игры
	if (!_has_end_game || _test) {
		bool all_active = false;
		for (auto& it : _obj_letters) {
			if (!it->hasFinish()) {
				all_active = true;
				break;
			}
		}
		if (!all_active || _test) {
			_has_end_game = true;
			float i = 0.;
			for (auto& it : _obj_letters) {
				it->play_anim(i, true);
				i += .2f;
			}
			_history_level.push_back(_curr_level);
			_levels[_curr_level]._has_complete = true;
		}

		if (_has_end_game) {
			_menu->setVisible(true);
		}
	}
	if (isExit) {
		GamesScene::ChangeToScene(GamesScene::SceneNames::SN_MainMenu);
		isExit = false;
		return;
	}

	World::update(delta);
}


void World_CreateWord::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) {
#if HAS_DEVELOP
	if (!DevelopMenu::Instance().isEnable())return;
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	CALL_VOID_METHOD(m_physicsSpline, draw());

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	World::draw(renderer, transform, flags);
#endif
}
