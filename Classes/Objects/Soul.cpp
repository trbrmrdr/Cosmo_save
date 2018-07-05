#include "stdafx.h"
#include "Soul.h"
#include "DevelopMenu.h"

#define DEF_ENABLE_MOUSE true

Soul::Soul() :
	isVisible(true),
	isEnableMouse(true),
	_defEnableMouse(DEF_ENABLE_MOUSE),
	isEdit(false),
	_changePos(false),
	_pos(Vec2::ZERO),
	_width(0),
	_height(0),
	_posZ(0),
	_layer(NULL),
	isSelected(false) {
	setSize(100, 100);
}

void Soul::setEdit(bool edit) {
	if (isEdit != edit)
		_changePos = false;
	isEdit = edit;
}

bool Soul::select(bool isParent) {
	if (isSelected)
		return false;
#if HAS_DEVELOP
	isSelected = true;
	edit_pos = DevelopMenu::Instance().addPointSetting(this, 0, DevelopTabs_World, "pos",
		([this](const Vec2& pos) { setPos(pos); }),
		([this](void) { return  getPos(); }));

	edit_z = DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "z",
		([this](const float& value) { setZ(value); }),
		([this](void) { return getZ(); }));

	edit_width = DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "width",
		([this](const float& width) { setWidth(width); }),
		([this](void) { return getWidth(); }));

	edit_height = DevelopMenu::Instance().addFloatSetting(this, 0, DevelopTabs_World, "height",
		([this](const float& height) { setHeight(height); }),
		([this](void) { return getHeight(); }),
		SeparatorType_Bottom);

	edit_enableMouse = DevelopMenu::Instance().addBoolSetting(this, 0, DevelopTabs_World, "isMouse",
		([this](const bool& val) { setDefEnableMouse(val); }),
		([this](void) { return getDefEnableMouse(); }),
		SeparatorType_Bottom);

	//if(false)
	if (NULL != _layer) {
		if (NULL == _debug_draw) {
			_debug_draw = DrawNode::create();
			_layer->addChild(_debug_draw, 9000);
			drawDebug();
		}
		_debug_draw->setVisible(true);
	}
#endif
	//only in override
	//if(!isParent)
	//Soul::refresh();
	return true;
}

void Soul::deselect() {
#if HAS_DEVELOP
	isSelected = false;
	DevelopMenu::Instance().eraceSettings(this);
	edit_pos = NULL;
	edit_width = NULL;
	edit_height = NULL;
	edit_z = NULL;
	edit_enableMouse = NULL;
	CALL_VOID_METHOD(_debug_draw, setVisible(false));
#endif
}

void Soul::refresh() {
#if HAS_DEVELOP
	if (!isSelected)
		return;
	edit_pos->Refresh();
	if (edit_width)
		edit_width->Refresh();
	if (edit_height)
		edit_height->Refresh();
	edit_z->Refresh();
	edit_enableMouse->Refresh();
#endif
}

Soul::~Soul() {
	//CCLOG("~Soul");
	//############################
	deselect();
#if HAS_DEVELOP
	if (_debug_draw)_debug_draw->removeFromParent();
	DEVELOP_CLEAR();
	//_layer = NULL;
	//CCLOG("~Soul_end");
#endif
}

bool Soul::mouseDown(const Vec2 & mousePos) {
	if (!isEdit && !isEnableMouse)return false;
	Size size = Size(getWidth(), getHeight());
	Rect rect = Rect(getPos() - size * 0.5, size);
	if (rect.containsPoint(mousePos)) {
		_changePos = true;
		_startPos = mousePos;
		_lastPos = getPos();
		if (isEdit)
			select();
		return true;
	}
	return false;
}

bool Soul::mouseUp(const Vec2 & mousePos) {
	if (!isEdit && !isEnableMouse)return false;
	if (_changePos) {
		_changePos = false;
		return true;
	}
	return false;
}

bool Soul::mouseMovie(const Vec2 & mousePos) {
	if (!isEdit && !isEnableMouse)return false;
	if (_changePos) {
		if (isEdit) {
			setPos(_lastPos - (_startPos - mousePos));
			refresh();
		}
		return true;
	}
	return false;
}

void Soul::Load(SettingUtil* setting) {
	setPos(setting->getVec2ForKey("pos"));
	setZ(setting->getFloatForKey("_z"));
	Vec2 size = setting->getVec2ForKey("size", Vec2(100, 100));
	setSize(size.x, size.y);
	setDefEnableMouse(setting->getBoolForKey("enableMouse"));
}

SettingUtil* Soul::Save() {
	SettingUtil* setting = new SettingUtil(true);
	setting->setIntegerForKey("SoulType", getType());

	setting->setVec2ForKey("pos", getPos());
	setting->setFloatForKey("_z", getZ());
	setting->setVec2ForKey("size", Vec2(getWidth(), getHeight()));
	if(DEF_ENABLE_MOUSE == getDefEnableMouse())
		setting->setBoolForKey("enableMouse", false);
	return setting;
}