#pragma once
#include "stdafx.h"
#include "UI/STTextField.h"
#include "UI/TabButton.h"
#include "UI/LayoutRefresh.h"
#include "UI/ListView_2.h"
#include "UI/AddButton.h"
#include "UI/ColorField.h"

enum DevelopTabs
{
	DevelopTabs_World = 0,
	DevelopTabs_Settings,
	DevelopTabs_Spline,
	DevelopTabs_Light,
	DevelopTabs_Last
};

enum SeparatorType
{
	SeparatorType_None = -1,
	SeparatorType_Top,
	SeparatorType_Bottom,
	SeparatorType_Stroke
};

#define DEVELOP_CLEAR()									\
{														\
DevelopMenu::Instance().eraceSettings(this);			\
DevelopMenu::Instance().clearChangeKeyEvents(this);		\
DevelopMenu::Instance().clearChangeDebugMode(this);		\
DevelopMenu::Instance().clearChangeFocus(this);			\
}

#define DEVELOP_ADD_S_BUTTON(group,tabs,name,_val)		\
DevelopMenu::Instance().addBoolSetting(				\
	this, group, tabs, name,						\
	( [this](const bool& val) { _val = val; } ),	\
	( [this](void) { return  false; } ));

#define DEVELOP_ADD_S_BOOL(group,tabs,name,_val)		\
DevelopMenu::Instance().addBoolSetting(				\
	this, group, tabs, name,						\
	( [this](const bool& val) { _val = val; } ),	\
	( [this](void) { return  _val; } ));

#define DEVELOP_ADD_S_POINT(group,tabs,name,_val)	\
DevelopMenu::Instance().addPointSetting(			\
	this, group, tabs, name,						\
	( [this](const Vec2& val) { _val = val; } ),	\
	( [this](void) { return  _val; } ));


#define DEVELOP_ADD_S_FLOAT(group,tabs,name,_val)	\
DevelopMenu::Instance().addFloatSetting(			\
	this, group, tabs, name,						\
	( [this](const float& val) { _val = val; } ),	\
	( [this](void) { return _val; } ))						

#define DEVELOP_ADD_S_FLOAT_MM(group,tabs,name,_val,_max,_min)	\
DevelopMenu::Instance().addFloatSettingFromMinMax(			\
	this, group, tabs, name,						\
	( [this](const float& val) { _val = val; } ),	\
	( [this](void) { return _val; } ),_min,_max)						


#define DEVELOP_ADD_S_UINT(group,tabs,name,_val)	\
DevelopMenu::Instance().addUIntSettings(			\
	this, group, tabs, name,						\
	( [this](const size_t& val) { _val = val; } ),	\
	( [this](void) { return _val; } ))						

#define DEVELOP_ADD_SEPARATOR(group,tabs,separatorType,name)	\
DevelopMenu::Instance().addSeparatorString(			\
this,group, tabs,									\
separatorType, name);								\

#define CREATE_STRUCT(TType)																						\
struct dFunc##TType																									\
		{																												\
void *data;																										\
std::string name;																								\
set##TType setFunction; 																						\
get##TType getFunction; 																						\
dFunc##TType( void *_data, const std::string& _name, set##TType _setFunction = nullptr, get##TType _getFunction = nullptr ,get##TType _getMin = nullptr,get##TType _getMax = nullptr) : \
data( _data ), name( _name ), setFunction( _setFunction ), getFunction( _getFunction ), getMin(_getMin), getMax(_getMax){}									  \
~dFunc##TType(){if(NULL!=data)delete data;data = NULL;}																		  \
get##TType getMin;																										  \
get##TType getMax;																										  \
				}; \
//std::vector<dFunc##TType> finc##TType;


CREATE_STRUCT(String);
CREATE_STRUCT(Float);
CREATE_STRUCT(Bool);
CREATE_STRUCT(Point);
CREATE_STRUCT(UIntItems);
CREATE_STRUCT(UInt);
CREATE_STRUCT(Color);

struct dFuncObj
{
	addNewObj addNewObjFunction;
	eraceObj eraceNewObjFunction;

	DevelopTabs tabs;
	std::string title;
	SeparatorType separator;

	size_t id;

	dFuncObj(DevelopTabs _tabs, const std::string& _title, SeparatorType _separator, addNewObj _addNewObjFunction = nullptr, eraceObj _eraceNewObjFunction = nullptr) :
		id(SIZE_MAX), tabs(_tabs), title(_title), separator(_separator), addNewObjFunction(_addNewObjFunction), eraceNewObjFunction(_eraceNewObjFunction) {}

	void setId(const size_t& _id) { id = _id; }
};

struct dFuncPress
{
	changeEvent cEvent;
	dFuncPress(changeEvent _cEvent) :cEvent(_cEvent) {}
};


struct ChangingKeyState
{
	changeKeyEvent funcKeyEvent;
	void* target;
	Text* title;
	ChangingKeyState(const changeKeyEvent&  _funcKeyEvent, void* _target, Text* _title = NULL) :
		funcKeyEvent(_funcKeyEvent), target(_target), title(_title) {}
};

struct SettingsT
{
	int _groupId = 0;
	void* _parent = NULL;
	LayoutRefresh* _widget = NULL;
	SettingsT(int groupId, void* parent, LayoutRefresh* widget) :_groupId(groupId), _parent(parent), _widget(widget) {};
	~SettingsT() {}
};

struct ChangeDebugT
{
	void* _parent = NULL;
	setBool _func;
	ChangeDebugT(void* parent, const setBool& func) :_parent(parent), _func(func) {};
};

struct ChangeOnFocus
{
	void* _parent = NULL;
	DevelopTabs _tabs;
	changeEvent _onEnter;
	changeEvent _onExit;
	ChangeOnFocus(void* parent, const DevelopTabs& tabs, const changeEvent& onEnter, const changeEvent& onExit) :
		_parent(parent), _tabs(tabs), _onEnter(onEnter), _onExit(onExit) {};
};

class DevelopMenu
#if HAS_DEVELOP
	: public Singleton < DevelopMenu >
#endif
{
private:
	int mBtsCheck;

	ListView_2* mKeysTitle;

	ListView* mListView;
	PageView* mPage;
	Size sizeItem;

	bool mFocus;
	bool isEnter;
	void setFocus(bool focus);

	//cocos engine needed remove all
	std::vector<TabButton*> tabButtons;

	std::vector<ChangingKeyState> _changeKeys;
	std::vector<ChangeDebugT> _changeDebug;
	std::vector <ChangeOnFocus> _changeOnFocus;
	std::vector<SettingsT> _settings;
	bool enable;
	Vec2 showPos;
	Vec2 hidePos;
	void tab_touchEvent(Ref *pSender, Widget::TouchEventType type);

	Layout* addSeparator(SeparatorType separator, float sizeWidth = 0, float sizeHeight = 0);

	LayoutRefresh* addFloatSettingFromMinMaxT(void* parent, int groupId,
											  DevelopTabs tabs, const std::string& title,
											  const setFloat&  _setFunction = nullptr,
											  const getFloat&  _getFunction = nullptr,
											  float* minValue = NULL, float* maxValue = NULL,
											  SeparatorType separator = SeparatorType_None,
											  float dx = 0.);


public:
	DevelopMenu();
	void free();

	~DevelopMenu();

	void init(Scene* _root);

	bool onMouseDown(Vec2 mousePos);
	bool onMouseUp(Vec2 mousePos);
	bool onMouseMove(Vec2 mousePos);
	bool onMouseScroll(Event* event);

	void setEnable(bool enable);
	bool isEnable() { return enable; };
	bool isFocus() { return mFocus; }

	bool updateChangeEvents(EventKeyboard::KeyCode keyCode, KeyState keyState);

	void eraceSettings(void* parent, int groupId = -1);

	LayoutRefresh* addSeparatorString(void* parent, int groupId,
									  DevelopTabs tabs,
									  SeparatorType separator = SeparatorType::SeparatorType_Stroke,
									  const std::string& title = std::string());

	LayoutRefresh* addStringSetting(void* parent, int groupId,
									DevelopTabs tabs, const std::string& title,
									const setString&  _setFunction = nullptr,
									const getString&  _getFunction = nullptr,
									SeparatorType separator = SeparatorType_None);

	LayoutRefresh* addFloatSetting(void* parent, int groupId,
								   DevelopTabs tabs, const std::string& title,
								   const setFloat&  _setFunction = nullptr,
								   const getFloat&  _getFunction = nullptr,
								   SeparatorType separator = SeparatorType_None,
								   float dx = 0.);

	LayoutRefresh* addFloatSettingFromMinMax(void* parent, int groupId,
											 DevelopTabs tabs, const std::string& title,
											 const setFloat&  _setFunction = nullptr,
											 const getFloat&  _getFunction = nullptr,
											 float minValue = .0, float maxValue = 100.,
											 SeparatorType separator = SeparatorType_None,
											 float dx = 0.);


	LayoutRefresh* addUIntSettings(void* parent, int groupId,
								   DevelopTabs tabs, const std::string& title,
								   const setUInt&  _setFunction = nullptr,
								   const getUInt&  _getFunction = nullptr,
								   SeparatorType separator = SeparatorType_None,
								   float dx = 0.);

	LayoutRefresh* addPointSetting(void* parent, int groupId,
								   DevelopTabs tabs, const std::string& title,
								   const setPoint&  _setFunction,
								   const getPoint&  _getFunction,
								   SeparatorType separator = SeparatorType_None,
								   float dx = 0., float dy = 0.);

	LayoutRefresh* addBoolSetting(void* parent, int groupId,
								  DevelopTabs tabs, const std::string& title,
								  const setBool&  _setFunction = nullptr,
								  const getBool&  _getFunction = nullptr,
								  SeparatorType separator = SeparatorType_None);

	LayoutRefresh* addColorSetting(void* parent, int groupId,
								   DevelopTabs tabs, const std::string& title,
								   const setColor&  _setFunction = nullptr,
								   const getColor&  _getFunction = nullptr,
								   SeparatorType separator = SeparatorType_None);

	LayoutRefresh* addListObject(void* parent, int groupId,
								 DevelopTabs tabs, const std::string& title,
								 AddButton** addButton,
								 const addNewObj&  _addFunction = nullptr,
								 const eraceObj&  _eraceFunction = nullptr,
								 SeparatorType separator = SeparatorType_None);

	LayoutRefresh* addButton(void* parent, int groupId,
							 DevelopTabs tabs, const std::string& title,
							 const std::string& nameButton,
							 const changeEvent&  _pressButton,
							 SeparatorType separator = SeparatorType_None);

	void refresh(void* target);

	void clearChangeKeyEvents(void* target);

	void changeKeyEvents(const changeKeyEvent& _keyFunc, void* parent = NULL, const std::string& title = "");

	void clearChangeDebugMode(void* parent);

	void changeDebugMode(void* parent, const setBool& _setBool);

	void clearChangeFocus(void* parent);
	void changeFocus(void* parent, DevelopTabs tabs, const changeEvent& _onEnter, const changeEvent& _onExit);


	LayoutRefresh* addSelectItem(void* parent, int groupId,
								 DevelopTabs tabs, const std::string& title,
								 const std::vector<std::string> item,
								 const setUIntItems&  _setFunction,
								 const getUIntItems&  _getFunction,
								 SeparatorType separator = SeparatorType_None);

	LayoutRefresh* addFloatSliderSetting(void* parent, int groupId,
										 DevelopTabs tabs, const std::string& title,
										 const setFloat&  _setFunction,
										 const getFloat&  _getFunction,
										 SeparatorType separator = SeparatorType_None,
										 const getFloat&  _getMin = nullptr,
										 const getFloat&  _getMax = nullptr
	);
};
