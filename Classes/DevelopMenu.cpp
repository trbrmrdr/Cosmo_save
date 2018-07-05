#include "stdafx.h"
#include "DevelopMenu.h"

#define DEVELOP_FONT "fonts/Marker Felt.ttf"

#define SIZE_LEFT 0.36
#define SIZE_RIGHT 0.6
#define SIZE_TEXT 24

//###################################

DevelopMenu::DevelopMenu():
	mBtsCheck(-1),
	enable(false),
	mFocus(false),
	isEnter(false) {}

void DevelopMenu::free() {
	//after change root scene
	//all contains widget  deleted in cocos engine
	mKeysTitle = NULL;
	mPage = NULL;
	mListView = NULL;
#if 0
	for (SettingsT it : _settings) {
		LayoutRefresh* widget = it._widget;
		void* data = widget->getUserData();
		if (NULL != data)
			delete data;
		/*
		   ListView_2* tab = static_cast<ListView_2*>( widget->getParent()->getParent() );
		   if(NULL != tab){
			   tab->removeChild(widget);
			   tab->refresh();
		   }
		   widget = NULL;
		 */
	}
	_settings.clear();

#else

#endif
	//_______
	tabButtons.clear();
	_changeOnFocus.clear();
	_changeDebug.clear();
	_changeKeys.clear();
}

DevelopMenu::~DevelopMenu() {
	free();
	return;
	for (auto& item : tabButtons) {
		//item->release();
	}
}

void DevelopMenu::clearChangeKeyEvents(void* target) {
	for (std::vector<ChangingKeyState>::const_iterator it = _changeKeys.begin(), it_e = _changeKeys.end(); it != it_e; ++it) {
		if (it->target == target) {
			Text* text = it->title;
			if (text != NULL) {
				text->removeFromParent();
				mKeysTitle->forceDoLayout();
			}
			_changeKeys.erase(it);
			break;
		}
	}
}

void DevelopMenu::changeKeyEvents(const changeKeyEvent& _keyFunc, void* target, const std::string& title) {
	clearChangeKeyEvents(target);
	Text* text = NULL;
	if (!title.empty()) {
		text = Text::create(title, FontCirceBold, SIZE_TEXT);
		mKeysTitle->pushBackCustomItem(text);
	}
	_changeKeys.push_back(ChangingKeyState(_keyFunc, target, text));
}

void DevelopMenu::clearChangeDebugMode(void* parent) {
	std::vector<ChangeDebugT>::iterator it, it_e = _changeDebug.end();

	for (it = _changeDebug.begin(); it != it_e; ++it) {
		if (it->_parent == parent) {
			//it->_func(false);
			_changeDebug.erase(it);
			return;
		}
	}
	return;
}

void DevelopMenu::clearChangeFocus(void* parent) {
	std::vector<ChangeOnFocus>::iterator it, it_e = _changeOnFocus.end();
	for (it = _changeOnFocus.begin(); it != it_e; ++it) {
		if (it->_parent == parent) {
			it->_onExit();
			_changeOnFocus.erase(it);
			return;
		}
	}
	return;
}

void DevelopMenu::changeFocus(void* parent, DevelopTabs tabs, const changeEvent& _onEnter, const changeEvent& _onExit) {
	_changeOnFocus.push_back(ChangeOnFocus(parent, tabs, _onEnter, _onExit));
	if (!isFocus())return;
	for (auto& item : tabButtons) {
		if (tabs == item->getTag() && item->isCheck()) {
			_onEnter();
			break;
		}
	}
}

void DevelopMenu::changeDebugMode(void* parent, const setBool& _setBool) {
	_changeDebug.push_back(ChangeDebugT(parent, _setBool));
	_setBool(enable);
}

void DevelopMenu::refresh(void* target) {

	for (SettingsT it : _settings) {
		if (it._parent == target) {
			it._widget->Refresh();
		}
	}
}

//todo memory leak незнаю откуда((((
void DevelopMenu::eraceSettings(void* parent, int id) {
	std::vector<SettingsT>::iterator it, it_e = _settings.end();

	for (it = _settings.begin(); it != it_e;) {
		if (it->_parent == parent && (id == -1 || id == it->_groupId)) {
			bool end = (it + 1) == it_e;
			bool start = it == _settings.begin();

			std::vector<SettingsT>::iterator it_n = !start ? it - 1 : it;
			{
				LayoutRefresh* widget = it->_widget;
				void* data = widget->getUserData();
				if (NULL != data)
					delete data;


				ListView_2* tab = static_cast<ListView_2*>(widget->getParent()->getParent());
				//widget->removeFromParent();
				if (NULL != tab) {
					tab->removeChild(widget);
					tab->refresh();
				}
				widget = NULL;
			}
			_settings.erase(it);
			if (start)
				it = _settings.begin();
			else if (!end)
				it = it_n + 1;

			it_e = _settings.end();
			if (end)
				break;
			continue;
		}
		++it;
	}
	return;
}

LayoutRefresh*  DevelopMenu::addSelectItem(void* parent, int groupId,
										   DevelopTabs tabs, const std::string& title,
										   const std::vector<std::string> item,
										   const setUIntItems&  _setFunction,
										   const getUIntItems&  _getFunction, SeparatorType separator) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));

	//_____________
	size_t countItem = item.size();

	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(Size(sizeItem.width, sizeItem.height*(countItem + 1)));

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

	Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
	tab_name->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	tab_name->setContentSize(sizeItem);
	Rect tmp = tab_name->getBoundingBox();
	float sx = sizeItem_l.width / tmp.size.width;
	float sy = sizeItem_l.height / tmp.size.height;
	tab_name->setScale(MIN(1, MIN(sx, sy)));
	tab_name->setPosition(Vec2(sizeItem.width * 0.75, sizeItem.height * countItem));

	new_item->addChild(tab_name);
	{
		std::vector<TabButton*>* buttonItems = new std::vector<TabButton*>();
		size_t i = 0;
		for (const std::string& nameItem : item) {

			Vec2 pos = Vec2(0, sizeItem.height * (i));

			Text* name = Text::create(nameItem, FontCirceLight, SIZE_TEXT);
			name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			name->setContentSize(sizeItem_l);
			Rect tmp = name->getBoundingBox();
			float sx = sizeItem_l.width / tmp.size.width;
			float sy = sizeItem_l.height / tmp.size.height;
			name->setScale(MIN(1, MIN(sx, sy)));
			name->setPosition(pos);
			new_item->addChild(name);


			pos = Vec2(sizeItem_l.width, pos.y);
			TabButton* checkButton = TabButton::create("UI/button_normal.png", "UI/button_selected.png");
			checkButton->setScale9Enabled(true);
			checkButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			checkButton->setPosition(pos);
			checkButton->setContentSize(sizeItem_r);

			buttonItems->push_back(checkButton);
			i++;
		}
		void* data = new dFuncUIntItems(buttonItems, title, _setFunction, _getFunction);

		new_item->setUserData(data);
		new_item->setFuncRefresh([this](Ref *pSender) {
			LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);

			dFuncUIntItems* dFuncUIntItems_data = static_cast<dFuncUIntItems*>(layout->getUserData());
			std::vector<TabButton*>* buttonItems = static_cast<std::vector<TabButton*>*>(dFuncUIntItems_data->data);
			if (nullptr != dFuncUIntItems_data->getFunction) {
				size_t select_it = dFuncUIntItems_data->getFunction();
				size_t i = 0;
				for (auto& item : *buttonItems) {
					if (select_it == i)
						item->onCheck(true);
					else
						item->onCheck(false);
					i++;
				}
			}
		});

		for (auto& item : *buttonItems) {
			item->setUserData(data);

			item->addClickEventListener(([this](Ref* pSender) {
				TabButton* thisButton = dynamic_cast<TabButton*>(pSender);
				bool isCheck = thisButton->isCheck();
				if (isCheck) {
					thisButton->onCheck(true, true);
					return;
				}
				dFuncUIntItems* dFuncUIntItems_data = static_cast<dFuncUIntItems*>(thisButton->getUserData());
				std::vector<TabButton*>* buttonItems = static_cast<std::vector<TabButton*>*>(dFuncUIntItems_data->data);
				if (nullptr != dFuncUIntItems_data->setFunction) {
					size_t select_it = 0;
					for (auto& item : *buttonItems) {
						item->onCheck(false);
						if (item == thisButton) {
							dFuncUIntItems_data->setFunction(select_it);
							item->onCheck(true);
						}
						select_it++;
					}
				}
			}));
			new_item->addChild(item);
		}
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

LayoutRefresh*  DevelopMenu::addListObject(void* parent, int groupId,
										   DevelopTabs tabs, const std::string& title,
										   AddButton** addButton,
										   const addNewObj&  _addFunction,
										   const eraceObj&  _eraceFunction,
										   SeparatorType separator) {
#if 0
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getPage(idP));
	//_____________
	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

	Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
	tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tab_name->setContentSize(sizeItem_l);
	Rect tmp = tab_name->getBoundingBox();
	float sx = sizeItem_l.width / tmp.size.width;
	float sy = sizeItem_l.height / tmp.size.height;
	tab_name->setScale(MIN(1, MIN(sx, sy)));
	new_item->addChild(tab_name);

	AddButton* button = AddButton::create("UI/button_normal.png", "UI/button_selected.png");
	button->setScale9Enabled(true);
	button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	button->setPosition(Vec2(sizeItem_l.width, 0));
	button->setContentSize(sizeItem_r);
	button->setTitleText("Add");

	void* data = new dFuncObj(tabs, title, separator, _addFunction, _eraceFunction);

	/*
	new_item->setUserData( data );
	new_item->setFuncRefresh( [this]( Ref *pSender )
	{
	LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
	const dFuncObj* data = static_cast<const dFuncObj*>(layout->getUserData());

	//ListView_2* tab = data->tab;
	//
	//tab->moveDown( layout );
	} );
	*/

	(*addButton) = button;
	button->setUserData(data);

	button->addAddEventListener(([this](Ref* pSender, int id) {
		AddButton* button = dynamic_cast<AddButton*>(pSender);
		dFuncObj* data = static_cast<dFuncObj*>(button->getUserData());

		AddButton* newButton = NULL;
		if (button->getTitleText() == "Del") {
			data->eraceNewObjFunction(data->id);
			button->getParent()->removeFromParent();
		}
		else {
			button->setTitleText("Del");
			if (id == -1)
				data->setId(data->addNewObjFunction());
			else
				data->setId(id);
			addListObject(data->tabs, data->title, &newButton, data->addNewObjFunction, data->eraceNewObjFunction, data->separator);
		}
		return newButton;

		//LayoutRefresh* layout = data->layout;
		//ListView_2* tab = data->tab;
		//tab->moveDown( layout );
	}));

	new_item->addChild(button);
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
#endif
	return NULL;
}

LayoutRefresh*  DevelopMenu::addButton(void* parent, int groupId,
									   DevelopTabs tabs, const std::string& title,
									   const std::string& _nameButton,
									   const changeEvent&  _pressButton,
									   SeparatorType separator) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________
	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

	Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
	tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tab_name->setContentSize(sizeItem_l);
	Rect tmp = tab_name->getBoundingBox();
	float sx = sizeItem_l.width / tmp.size.width;
	float sy = sizeItem_l.height / tmp.size.height;
	tab_name->setScale(MIN(1, MIN(sx, sy)));
	new_item->addChild(tab_name);

	AddButton* button = AddButton::create("UI/button_normal.png", "UI/button_selected.png");
	button->setScale9Enabled(true);
	button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	button->setPosition(Vec2(sizeItem_l.width, 0));
	button->setContentSize(sizeItem_r);
	button->setTitleText(_nameButton);

	void* data = new dFuncPress(_pressButton);

	button->setUserData(data);
	button->addAddEventListener(([this](Ref* pSender, int id) {
		AddButton* button = dynamic_cast<AddButton*>(pSender);
		dFuncPress* data = static_cast<dFuncPress*>(button->getUserData());

		//if (button->getTitleText() == "Add")
		{
			data->cEvent();
		}
		return button;
	}));

	new_item->addChild(button);
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

LayoutRefresh* DevelopMenu::addBoolSetting(void* parent, int groupId,
										   DevelopTabs tabs, const std::string& title,
										   const setBool&  _setFunction,
										   const getBool&  _getFunction,
										   SeparatorType separator) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

	{
		Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
		tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		tab_name->setContentSize(sizeItem_l);
		Rect tmp = tab_name->getBoundingBox();
		float sx = sizeItem_l.width / tmp.size.width;
		float sy = sizeItem_l.height / tmp.size.height;
		tab_name->setScale(MIN(1, MIN(sx, sy)));
		new_item->addChild(tab_name);

		TabButton* checkButton = TabButton::create("UI/button_normal.png", "UI/button_selected.png");
		checkButton->setScale9Enabled(true);
		checkButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		checkButton->setPosition(Vec2(sizeItem_l.width, 0));
		checkButton->setContentSize(sizeItem_r);

		void* data = new dFuncBool(checkButton, title, _setFunction, _getFunction);

		new_item->setUserData(data);
		new_item->setFuncRefresh([this](Ref *pSender) {
			LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
			const dFuncBool* data = static_cast<const dFuncBool*>(layout->getUserData());

			if (nullptr != data->getFunction) {
				TabButton* checkButton = static_cast<TabButton*>(data->data);
				checkButton->onCheck(data->getFunction());
			}
		});

		checkButton->setUserData(data);
		checkButton->addClickEventListener(([this](Ref* pSender) {
			TabButton* checkBut = dynamic_cast<TabButton*>(pSender);
			bool isCheck = !checkBut->isCheck();
			checkBut->onCheck(isCheck);
			const dFuncBool* data = static_cast<const dFuncBool*>(checkBut->getUserData());
			if (nullptr != data->setFunction) {
				data->setFunction(isCheck);
			}

			if (nullptr != data->getFunction) {
				if (data->getFunction() != isCheck)
					checkBut->onCheck(!isCheck);
			}
		}));

		new_item->addChild(checkButton);
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

LayoutRefresh* DevelopMenu::addColorSetting(void* parent, int groupId,
											DevelopTabs tabs, const std::string & title,
											const setColor & _setFunction,
											const getColor & _getFunction,
											SeparatorType separator) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

	{
		Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
		tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		tab_name->setContentSize(sizeItem_l);
		Rect tmp = tab_name->getBoundingBox();
		float sx = sizeItem_l.width / tmp.size.width;
		float sy = sizeItem_l.height / tmp.size.height;
		tab_name->setScale(MIN(1, MIN(sx, sy)));
		new_item->addChild(tab_name);

		ColorField* colorField = ColorField::create();
		colorField->setColor(Color3B::WHITE);

		// nerabotat colorField->setAnchorPoint( Vec2::ANCHOR_MIDDLE_RIGHT );
		colorField->setPosition(Vec2(sizeItem_l.width + 100, colorField->getContentSize().height *0.5));
		//colorField->setContentSize( sizeItem_r );

		new_item->setContentSize(Size(sizeItem.width, colorField->getContentSize().height));

		void* data = new dFuncColor(colorField, title, _setFunction, _getFunction);

		new_item->setUserData(data);
		new_item->setFuncRefresh([this](Ref *pSender) {
			LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
			const dFuncColor* data = static_cast<const dFuncColor*>(layout->getUserData());

			if (nullptr != data->getFunction) {
				ControlColourPicker* colorField = static_cast<ControlColourPicker*>(data->data);
				colorField->setColor(data->getFunction());
			}
		});

		colorField->setUserData(data);

		colorField->addAddEventListener(
			([this](Ref *pSender, Control::EventType type) {
			ColorField* colorField = dynamic_cast<ColorField*>(pSender);
			const dFuncColor* data = static_cast<const dFuncColor*>(colorField->getUserData());
			if (nullptr != data->setFunction) {
				data->setFunction(colorField->getColor());
			}
		})
		);
		new_item->addChild(colorField);
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator, new_item->getContentSize().width, new_item->getContentSize().height));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

LayoutRefresh* DevelopMenu::addPointSetting(void* parent, int groupId,
											DevelopTabs tabs, const std::string& title,
											const setPoint&  _setFunction,
											const getPoint&  _getFunction,
											SeparatorType separator,
											float dx, float dy) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

	{
		Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
		tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		tab_name->setContentSize(sizeItem_l);
		Rect tmp = tab_name->getBoundingBox();
		float sx = sizeItem_l.width / tmp.size.width;
		float sy = sizeItem_l.height / tmp.size.height;
		tab_name->setScale(MIN(1, MIN(sx, sy)));
		new_item->addChild(tab_name);

		STTextField* textField = STTextField::create("x;y", FontCirceBold, SIZE_TEXT, dx, dy);
		textField->setType(Type_Point);
		textField->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		textField->setPosition(Vec2(sizeItem_l.width, 0));
		textField->setContentSize(sizeItem_r);
		textField->setTextHorizontalAlignment(TextHAlignment::LEFT);
		textField->setTextVerticalAlignment(TextVAlignment::CENTER);

		void* data = new dFuncPoint(textField, title, _setFunction, _getFunction);


		new_item->setUserData(data);
		new_item->setFuncRefresh([this](Ref *pSender) {
			LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
			const dFuncPoint* data = static_cast<const dFuncPoint*>(layout->getUserData());

			if (nullptr != data->getFunction) {
				STTextField* textField = static_cast<STTextField*>(data->data);
				textField->setValue(data->getFunction());
			}
		});

		textField->setUserData(data);
		textField->addEventListener(
			([this](Ref *pSender, TextField::EventType type) {
			STTextField* textField = dynamic_cast<STTextField*>(pSender);
			const dFuncPoint* data = static_cast<const dFuncPoint*>(textField->getUserData());
			if (nullptr != data->setFunction) {
				data->setFunction(textField->getStringToVec2());
			}
		})
		);

		new_item->addChild(textField);
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}


LayoutRefresh* DevelopMenu::addUIntSettings(void* parent, int groupId,
											DevelopTabs tabs, const std::string& title,
											const setUInt&  _setFunction,
											const getUInt&  _getFunction,
											SeparatorType separator,
											float dx) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	{
		new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

		Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
		Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

		{
			Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
			tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			tab_name->setContentSize(sizeItem_l);
			Rect tmp = tab_name->getBoundingBox();
			float sx = sizeItem_l.width / tmp.size.width;
			float sy = sizeItem_l.height / tmp.size.height;
			tab_name->setScale(MIN(1, MIN(sx, sy)));
			new_item->addChild(tab_name);

			STTextField* textField = STTextField::create("UInt", FontCirceBold, 24, dx);
			textField->setType(Type_UInt);
			textField->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			textField->setPosition(Vec2(sizeItem_l.width, 0));
			textField->setContentSize(sizeItem_r);
			textField->setTextHorizontalAlignment(TextHAlignment::LEFT);
			textField->setTextVerticalAlignment(TextVAlignment::CENTER);

			void* data = new dFuncUInt(textField, title, _setFunction, _getFunction);


			new_item->setUserData(data);
			new_item->setFuncRefresh([this](Ref *pSender) {
				LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
				const dFuncUInt* data = static_cast<const dFuncUInt*>(layout->getUserData());

				if (nullptr != data->getFunction) {
					STTextField* textField = static_cast<STTextField*>(data->data);
					textField->setValue(data->getFunction());
				}
			});


			textField->setUserData(data);
			textField->addEventListener(
				([this](Ref *pSender, TextField::EventType type) {
				STTextField* textField = dynamic_cast<STTextField*>(pSender);
				const dFuncUInt* data = static_cast<const dFuncUInt*>(textField->getUserData());
				if (nullptr != data->setFunction) {
					data->setFunction(textField->getStringToSizeT());
				}

				if (nullptr != data->getFunction) {
					textField->setValue(data->getFunction());
				}
			})
			);

			new_item->addChild(textField);
		}
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}


LayoutRefresh*  DevelopMenu::addFloatSliderSetting(void* parent, int groupId,
												   DevelopTabs tabs, const std::string& title,
												   const setFloat&  _setFunction, const getFloat&  _getFunction,
												   SeparatorType separator,
												   const getFloat&  _getMin, const getFloat&  _getMax) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	{
		new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		new_item->setContentSize(Size(sizeItem.width, sizeItem.height * 1.5));

		Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
		Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

		{
			Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
			tab_name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			tab_name->setContentSize(sizeItem_l);
			tab_name->setPosition(Vec2(sizeItem.width * 0.5, sizeItem.height * 1.5));
			Rect tmp = tab_name->getBoundingBox();
			float sx = sizeItem_l.width / tmp.size.width;
			float sy = sizeItem_l.height / tmp.size.height;
			tab_name->setScale(MIN(1, MIN(sx, sy)));
			new_item->addChild(tab_name);

			// Create the slider
			Slider* slider = Slider::create();
			slider->loadBarTexture("UI/sliderTrack.png");
			slider->loadSlidBallTextures("UI/sliderThumb.png", "UI/sliderThumb.png", "");
			slider->loadProgressBarTexture("UI/sliderProgress.png");
			//slider->setMaxPercent( 100 );

			slider->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			//slider->setPosition( Vec2( sizeItem_l.width, 0 ) );
			slider->setPosition(Vec2(sizeItem.width * 0.6, sizeItem.height));
			slider->setContentSize(Size(sizeItem.width, sizeItem.height));

			dFuncFloat* data = new dFuncFloat(slider, title, _setFunction, _getFunction, _getMin, _getMax);

			slider->setUserData(data);
			slider->addEventListener([this](Ref* pSender, Slider::EventType type) {
				if (type != Slider::EventType::ON_PERCENTAGE_CHANGED)
					return;

				Slider* slider = dynamic_cast<Slider*>(pSender);
				const dFuncFloat* data = static_cast<const dFuncFloat*>(slider->getUserData());

				if (nullptr != data->setFunction) {
					Slider* slider = static_cast<Slider*>(data->data);

					float min = NULL == data->getMin ? 0 : data->getMin();
					float max = NULL == data->getMax ? 1 : data->getMax();

					float percent = slider->getPercent();

					percent = min + (max - min) * (percent / 100);

					data->setFunction(percent);
				}
			});


			new_item->setUserData(data);
			new_item->setFuncRefresh([this](Ref *pSender) {
				LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
				const dFuncFloat* data = static_cast<const dFuncFloat*>(layout->getUserData());

				if (nullptr != data->getFunction) {
					Slider* slider = static_cast<Slider*>(data->data);

					float min = NULL == data->getMin ? 0 : data->getMin();
					float max = NULL == data->getMax ? 1 : data->getMax();

					float percent = data->getFunction();

					if (percent <= min)percent = 0;
					else if (percent >= max)percent = 100;
					else percent = (percent - min) / (max - min) * 100;

					slider->setPercent(percent);
				}
			});


			new_item->addChild(slider);
		}
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

LayoutRefresh* DevelopMenu::addFloatSetting(void* parent, int groupId,
											DevelopTabs tabs, const std::string& title,
											const setFloat&  _setFunction,
											const getFloat&  _getFunction,
											SeparatorType separator,
											float dx) {
	return addFloatSettingFromMinMaxT(parent, groupId, tabs, title, _setFunction, _getFunction, NULL, NULL, separator, dx);
}

LayoutRefresh* DevelopMenu::addFloatSettingFromMinMax(void* parent, int groupId,
													  DevelopTabs tabs, const std::string& title,
													  const setFloat&  _setFunction,
													  const getFloat&  _getFunction,
													  float minValue, float maxValue,
													  SeparatorType separator,
													  float dx) {
	float t_min = minValue;
	float t_max = maxValue;
	return addFloatSettingFromMinMaxT(parent, groupId, tabs, title, _setFunction, _getFunction, &t_min, &t_max, separator, dx);
}

LayoutRefresh* DevelopMenu::addFloatSettingFromMinMaxT(void* parent, int groupId,
													   DevelopTabs tabs, const std::string& title,
													   const setFloat&  _setFunction, const getFloat&  _getFunction,
													   float* minValue, float* maxValue,
													   SeparatorType separator,
													   float dx) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	{
		new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		new_item->setContentSize(Size(sizeItem.width, sizeItem.height));

		Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
		Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

		{
			Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
			tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			tab_name->setContentSize(sizeItem_l);
			Rect tmp = tab_name->getBoundingBox();
			float sx = sizeItem_l.width / tmp.size.width;
			float sy = sizeItem_l.height / tmp.size.height;
			tab_name->setScale(MIN(1, MIN(sx, sy)));
			new_item->addChild(tab_name);

			STTextField* textField = STTextField::create("float", FontCirceBold, 24, dx);
			textField->setType(Type_Float);
			textField->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			textField->setPosition(Vec2(sizeItem_l.width, 0));
			textField->setContentSize(sizeItem_r);
			textField->setTextHorizontalAlignment(TextHAlignment::LEFT);
			textField->setTextVerticalAlignment(TextVAlignment::CENTER);

			textField->setMinMax(
				minValue == NULL ? "" :
				StringUtils::format("%f", *minValue),
				maxValue == NULL ? "" :
				StringUtils::format("%f", *maxValue)
			);
			void* data = new dFuncFloat(textField, title, _setFunction, _getFunction);


			new_item->setUserData(data);
			new_item->setFuncRefresh([this](Ref *pSender) {
				LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
				const dFuncFloat* data = static_cast<const dFuncFloat*>(layout->getUserData());

				if (nullptr != data->getFunction) {
					STTextField* textField = static_cast<STTextField*>(data->data);
					textField->setValue(data->getFunction());
				}
			});


			textField->setUserData(data);
			textField->addEventListener(
				([this](Ref *pSender, TextField::EventType type) {
				STTextField* textField = dynamic_cast<STTextField*>(pSender);
				const dFuncFloat* data = static_cast<const dFuncFloat*>(textField->getUserData());
				if (nullptr != data->setFunction) {
					data->setFunction(textField->getStringToFloat());
				}
			})
			);

			new_item->addChild(textField);
		}
	}
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();
	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}


LayoutRefresh* DevelopMenu::addStringSetting(void* parent, int groupId,
											 DevelopTabs tabs, const std::string& title,
											 const setString&  _setFunction, const getString&  _getFunction,
											 SeparatorType separator) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(sizeItem);

	Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
	Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);


	Text* tab_name = Text::create(title, FontCirceLight, SIZE_TEXT);
	tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tab_name->setContentSize(sizeItem_l);
	Rect tmp = tab_name->getBoundingBox();
	float sx = sizeItem_l.width / tmp.size.width;
	float sy = sizeItem_l.height / tmp.size.height;
	tab_name->setScale(MIN(1, MIN(sx, sy)));
	new_item->addChild(tab_name);

	STTextField* textField = STTextField::create("name", FontCirceBold, SIZE_TEXT);
	textField->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	textField->setPosition(Vec2(sizeItem_l.width, 0));
	textField->setContentSize(sizeItem_r);
	textField->setTextHorizontalAlignment(TextHAlignment::LEFT);
	textField->setTextVerticalAlignment(TextVAlignment::CENTER);

	void* data = new dFuncString(textField, title, _setFunction, _getFunction);

	new_item->setUserData(data);
	new_item->setFuncRefresh([this](Ref *pSender) {
		LayoutRefresh* layout = dynamic_cast<LayoutRefresh*>(pSender);
		const dFuncString* data = static_cast<const dFuncString*>(layout->getUserData());

		if (nullptr != data->getFunction) {
			STTextField* textField = static_cast<STTextField*>(data->data);
			textField->setValue(data->getFunction());
		}
	});


	textField->setUserData(data);

	textField->addEventListener(
		([this](Ref *pSender, TextField::EventType type) {
		STTextField* textField = dynamic_cast<STTextField*>(pSender);
		const dFuncString* data = static_cast<const dFuncString*>(textField->getUserData());

		if (nullptr != data->setFunction)
			data->setFunction(textField->getString());
	})
	);
	new_item->addChild(textField);
	//_______________________
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();

	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

LayoutRefresh* DevelopMenu::addSeparatorString(void* parent, int groupId,
											   DevelopTabs tabs,
											   SeparatorType separator,
											   const std::string& title) {
	size_t idP = tabs;
	ListView_2* tab = static_cast<ListView_2*>(mPage->getItem(idP));
	//_____________

	LayoutRefresh* new_item = LayoutRefresh::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	new_item->setContentSize(sizeItem);

	if (!title.empty()) {
		Size sizeItem_l = Size(sizeItem.width * SIZE_LEFT, sizeItem.height);
		Size sizeItem_r = Size(sizeItem.width * SIZE_RIGHT, sizeItem.height);

		Text* tab_name = Text::create(title, FontCirceBold, SIZE_TEXT);
		tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		tab_name->setContentSize(sizeItem_l);
		Rect tmp = tab_name->getBoundingBox();
		float sx = sizeItem_l.width / tmp.size.width;
		float sy = sizeItem_l.height / tmp.size.height;
		tab_name->setScale(MIN(1, MIN(sx, sy)));

		new_item->addChild(tab_name);
	}
	if (SeparatorType_None != separator)
		new_item->addChild(addSeparator(separator));

	tab->pushBackCustomItem(new_item);
	new_item->Refresh();

	_settings.push_back(SettingsT(groupId, parent, new_item));
	return new_item;
}

Layout* DevelopMenu::addSeparator(SeparatorType separator, float sizwWidth, float sizeHeight) {
	Layout* new_item = Layout::create();
	new_item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	Size size = Size(sizwWidth == 0 ? sizeItem.width : sizwWidth, sizeHeight == 0 ? sizeItem.height : sizeHeight);
	new_item->setContentSize(size);

	std::string sep;

	size_t count = (size_t)(sizeItem.width / SIZE_TEXT) * 2;
	for (size_t i = 0; i < count; ++i)
		sep += "_";
	Text* delimer = Text::create(sep, FontCirceBold, SIZE_TEXT);
	delimer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	switch (separator) {
		case SeparatorType_Top:
			delimer->setPosition(Vec2(0, sizeItem.height));
			break;
		case SeparatorType_Bottom:
			delimer->setPosition(Vec2::ZERO);
			break;
		case SeparatorType_Stroke:
			delimer->setPosition(Vec2(0, sizeItem.height));

			Text* delimer_2 = Text::create(sep, FontCirceBold, 24);
			delimer_2->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			new_item->addChild(delimer_2);
			break;
	}

	new_item->addChild(delimer);
	return new_item;
}

std::string DevelopTabs_Names[]{ "World", "Settings", "Spline","Light", "Test" };

#define FIRST_SELECTED_TAB 2
void DevelopMenu::init(Scene* _root) {
	enable = false;
	mFocus = false;
	isEnter = false;
	free();
	Rect _window = VisibleRect::getVisibleRect();
	Rect _sizeMenu = Rect(Vec2(_window.origin.x + _window.size.width, _window.origin.y + _window.size.height),// - Vec2( 10, 10 ),
						  Size(_window.size.width * 0.24, _window.size.height));
	showPos = _sizeMenu.origin;
	hidePos = _sizeMenu.origin + Vec2(_sizeMenu.size.width * 2, 0);

	mListView = ListView::create();
	mListView->setAnchorPoint(Vec2(1, 1));
	mListView->setPosition(hidePos);

	mListView->setContentSize(_sizeMenu.size);
	mListView->setBackGroundImageScale9Enabled(true);
	mListView->setBackGroundImage("UI/background_small.png");

	mListView->setOpacity(125);
	//mListView->setBackGroundColorType( LayoutBackGroundColorType::SOLID );
	//mListView->setBackGroundColorOpacity( 125 );

	mListView->setTouchEnabled(false);
	mListView->setDirection(ui::ScrollView::Direction::VERTICAL);
	mListView->setBounceEnabled(false);
	mListView->setItemsMargin(2.0f);
	mListView->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
	//____________ top name
	size_t countTab = DevelopTabs_Last + 1;
	Size tab_size = Size(_sizeMenu.size.width / countTab, 50);
	Size tabs_size = Size(_sizeMenu.size.width, 50);
	//_______________________
	Text* _name = Text::create("Scene 1", DEVELOP_FONT, 24);
	//_name->setTextHorizontalAlignment( TextHAlignment::CENTER );
	//_name->setContentSize( _size );
	//_name->setPosition( Vec2( _sizeMenu.size.width * 0.5f, _sizeMenu.size.height - 30 ) );
	mListView->pushBackCustomItem(_name);
	///Tab
	{

		ListView_2* tabs = ListView_2::create();
		tabs->setDirection(ui::ScrollView::Direction::HORIZONTAL);
		tabs->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);

		tabs->setScrollBarEnabled(false);
		tabs->setTouchEnabled(false);
		tabs->setItemsMargin(1.0f);
		tabs->setContentSize(tabs_size);
		for (size_t i = 0; i < countTab; ++i) {
			TabButton* tab = TabButton::create("UI/button_normal.png", "UI/button_selected.png");
			tab->setScale9Enabled(true);
			tab->setTag(i);
			//ToDo
			if (0 == FIRST_SELECTED_TAB) {
				tab->onCheck(true);
				mBtsCheck = i;
			}

			tab->setText(DevelopTabs_Names[i]);
			tab->setContentSize(tab_size);
			tab->addTouchEventListener(CC_CALLBACK_2(DevelopMenu::tab_touchEvent, this));
			//tab->addCCSEventListener
			//tab->setName( StringUtils::format( "tabbutton_%i", i ) );
			tabButtons.push_back(tab);
			//tab->retain();

			tabs->pushBackCustomItem(tab);
		}
		mListView->pushBackCustomItem(tabs);
	}
	//_______________________
	{
		mPage = PageView::create();
		mPage->setContentSize(Size(_sizeMenu.size.width, _sizeMenu.size.height - (tabs_size.height + _name->getContentSize().height + 10)));
		for (size_t i = 0; i < countTab; ++i) {
			ListView_2* tabs_ = ListView_2::create();
			tabs_->setDirection(ui::ScrollView::Direction::VERTICAL);
			tabs_->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
			tabs_->setItemsMargin(1.0f);
			//tabs_->setContentSize( Size( _sizeMenu.size.width, 100 ) );
			//ToDo
			if (i < DevelopTabs_Last) {
				mPage->addPage(tabs_);
				continue;
			}
			size_t countTab = 20;
			sizeItem = Size(_sizeMenu.size.width, 50);
			for (size_t j = 0; j < countTab; ++j) {
				Layout* item = Layout::create();
				item->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				item->setContentSize(sizeItem);

				Text* tab_name = Text::create(StringUtils::format("Scene %i", j), DEVELOP_FONT, 24);
				tab_name->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

				Button* tab_button = Button::create("UI/button_normal.png", "UI/button_selected.png");
				tab_button->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				tab_button->setScale9Enabled(true);
				tab_button->setTitleText(StringUtils::format("test_%i-%i", i, j));
				//tab->addTouchEventListener( CC_CALLBACK_2( DevelopMenu::tab_touchEvent, this ) );
				tab_button->setContentSize(Size(sizeItem.width*0.5, sizeItem.height));
				tab_button->setPosition(Vec2(sizeItem.width*0.5, 0));
				item->addChild(tab_name);
				item->addChild(tab_button);
				tabs_->pushBackCustomItem(item);
			}
			mPage->addPage(tabs_);
		}
		mListView->pushBackCustomItem(mPage);
	}

	//_______________________
	_root->addChild(mListView, 10000);


	mKeysTitle = ListView_2::create();
	mKeysTitle->setTouchEnabled(false);
	mKeysTitle->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	mKeysTitle->setPosition(VisibleRect::leftTop());
	mKeysTitle->setContentSize(_window.size);

	mKeysTitle->pushBackCustomItem(Text::create("F1 - DevMode On/Off", FontCirceBold, 24));
	mKeysTitle->setVisible(false);
	_root->addChild(mKeysTitle, 10000);


	mPage->jumpToItem(FIRST_SELECTED_TAB, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
}

bool DevelopMenu::onMouseDown(Vec2 mousePos) {
	if (!enable)
		return false;

	auto camera = Camera::getDefaultCamera();
	if (mListView->hitTest(mousePos, camera, nullptr)) {
		if (!mFocus) {
			mFocus = true;
			if (!isEnter) {
				isEnter = true;
				mListView->onEnter();
			}
		}
		return true;
	}

	return false;
}

bool DevelopMenu::onMouseUp(Vec2 mousePos) {
	if (!enable)
		return false;

	auto camera = Camera::getDefaultCamera();
	if (mListView->hitTest(mousePos, camera, nullptr)) {
		if (!mFocus) {
			mFocus = true;
			if (!isEnter) {
				isEnter = true;
				mListView->onEnter();
			}
		}
		return true;
	}
	mFocus = false;
	if (isEnter) {
		isEnter = false;
		mListView->onExit();
	}
	return false;
}

bool DevelopMenu::onMouseMove(Vec2 mousePos) {
	if (!enable)
		return false;

	auto camera = Camera::getDefaultCamera();
	if (mListView->hitTest(mousePos, camera, nullptr)) {
		if (!mFocus) {
			mFocus = true;
			if (!isEnter) {
				isEnter = true;
				mListView->onEnter();
			}
		}
		return true;
	}
	mFocus = false;
	if (isEnter) {
		isEnter = false;
		mListView->onExit();
	}
	return false;
}

bool DevelopMenu::onMouseScroll(Event * event) {
	if (!enable)
		return false;

	return false;
}

TabButton* startCheck(NULL);
void DevelopMenu::tab_touchEvent(Ref *pSender, Widget::TouchEventType type) {
	switch (type) {
		case Widget::TouchEventType::BEGAN:
		{
			//_displayValueLabel->setString( StringUtils::format( "Touch Down" ) );
			startCheck = ((TabButton*)pSender);
		}
		break;


		case Widget::TouchEventType::MOVED:
		{
			//_displayValueLabel->setString( StringUtils::format( "Touch Move" ) );
			auto btn = ((TabButton*)pSender);
			if (btn != startCheck)
				startCheck = NULL;
		}
		break;

		case Widget::TouchEventType::ENDED:
		{
			//_displayValueLabel->setString( StringUtils::format( "Touch Up" ) );
			TabButton* btn = ((TabButton*)pSender);
			if (btn != startCheck)
				return;
			startCheck = NULL;
			size_t finded_id = -1, id = 0;
			TabButton* oldBtn = NULL;
			for (auto& item : tabButtons) {
				if (NULL == oldBtn && item->isCheck())
					oldBtn = item;
				item->onCheck(false);
				if (finded_id == -1 && btn == item)
					finded_id = id;
				else
					id++;
			}
			//mPage->scrollToPage(finded_id);
			mPage->jumpToItem(finded_id, Vec2::ANCHOR_MIDDLE, Vec2::ANCHOR_MIDDLE);
			//btn->setScale9Enabled( !btn->isScale9Enabled() );
			//btn->setContentSize( Size( 200, 100 ) );
			btn->onCheck(true);
			mBtsCheck = btn->getTag();

			for (ChangeOnFocus it : _changeOnFocus) {
				if (it._tabs == btn->getTag()) {
					it._onEnter();
					continue;
				}
				if (it._tabs == oldBtn->getTag()) {
					it._onExit();
					continue;
				}
			}

		}
		break;

		case Widget::TouchEventType::CANCELED:
			//_displayValueLabel->setString( StringUtils::format( "Touch Cancelled" ) );
			break;

		default:
			break;
	}
}

void DevelopMenu::setFocus(bool focus) {
	if (mFocus == focus)
		return;
	mFocus = focus;
	if (!mFocus)
		mListView->onExit();
}

bool DevelopMenu::updateChangeEvents(EventKeyboard::KeyCode keyCode, KeyState keyState) {
	if (mFocus)return true;
	bool ret = false;
	//if (enable)
	{
		std::vector<EventKeyboard::KeyCode> keyAction;
		for (auto& iter : _changeKeys) {
			bool isAction = true;
			if (keyState == KeyState::KeyState_PRESS) {
				std::vector<EventKeyboard::KeyCode>::iterator finded = std::find(keyAction.begin(), keyAction.end(), keyCode);
				if (finded != keyAction.end())
					isAction = false;
			}

			if (isAction && iter.funcKeyEvent(keyCode, keyState)) {
				keyAction.push_back(keyCode);
				ret = true;
			}
		}
	}
	return ret;
}

void DevelopMenu::setEnable(bool _enable) {
	if (enable == _enable) return;

	enable = _enable;
	float delay = 0.5f;
	if (enable) {
		mListView->setPosition(showPos);
		//mListView->runAction( MoveTo::create( delay, showPos ) );
		//mListView->runAction( FadeIn::create( delay ) );
	}
	else {
		mFocus = false;
		//mListView->runAction( MoveTo::create( delay, hidePos ) );
		//mListView->runAction( FadeOut::create( delay ) );
	}
	mListView->setVisible(enable);
	mKeysTitle->setVisible(enable);
	for (auto& iter : _changeDebug)
		iter._func(enable);

	for (ChangeOnFocus it : _changeOnFocus) {
		if (it._tabs == mBtsCheck)
			if (enable)
				it._onEnter();
			else
				it._onExit();
	}
}
