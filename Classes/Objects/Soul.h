#pragma once

#include "stdafx.h"
#include "UI/LayoutRefresh.h"

enum SoulType
{
	SoulType_Soul = -1,
	SoulType_StaticImage,
	SoulType_Light,
	SoulType_FinishImage_rm,
	SoulType_Button,
    SoulType_rm_obj,///old selectLevel
	SoulType_PhysicsObject,
	SoulType_PuzzleImage,
};

class Soul
{
public:
	Soul(Layer* layer) :Soul() { _layer = layer; };

	Soul();
	virtual ~Soul();
	virtual SoulType getType() { return SoulType_Soul; };
	virtual void Load(SettingUtil* setting);
	virtual SettingUtil* Save();

	virtual int getZ() { return _posZ; };
	virtual void setZ(int val) { _posZ = val; };

	virtual void setPos(const Vec2& pos) {
		_pos = pos;
		drawDebug();
	};
	virtual Vec2 getPos() { return _pos; };

	virtual void setHeight(float height) { _height = height; };
	virtual float getHeight() { return _height; };

	virtual void setWidth(float width) { _width = width; };
	virtual float getWidth() { return _width; };
	virtual void setSize(float width, float height) {
		_width = width; _height = height;
		drawDebug();
	};

	virtual bool mouseDown(const Vec2& mousePos);
	virtual bool mouseUp(const Vec2& mousePos);
	virtual bool mouseMovie(const Vec2& mousePos);

	virtual bool select(bool isParent = false);
	virtual void deselect();
	virtual void refresh();

	virtual void setEdit(bool edit);

	virtual void setDefEnableMouse(bool enable) { _defEnableMouse = enable; setEnableMouse(enable); }
	virtual bool getDefEnableMouse() { return _defEnableMouse; }

	virtual void setEnableMouse(bool enable) { isEnableMouse = enable; };
	virtual bool getEnableMouse() { return isEnableMouse; };
	virtual void update(float delta) {};

	virtual void draw() {};

	virtual void draw_debug(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) {};


	void drawDebug() {
#if HAS_DEVELOP
		if (NULL != _debug_draw) {
			_debug_draw->clear();
			Vec2 d_p = Vec2(_width, _height)*.5;
			//_debug_draw->setPosition(_pos);

			_debug_draw->drawLine(_pos + d_p, _pos - d_p, Color4F::BLACK);
			_debug_draw->drawLine(_pos - Vec2(d_p.x, -d_p.y), _pos + Vec2(d_p.x, -d_p.y), Color4F::BLACK);
		}
#endif
	};

protected:
	bool _defEnableMouse;
	bool isEnableMouse;

	bool isEdit;
	bool isVisible;

	Layer* _layer;
	Vec2 _pos;
	float _width;
	float _height;
	int _posZ;

	bool _changePos;
	Vec2 _startPos;
	Vec2 _lastPos;
	//####################
	bool isSelected;
#if HAS_DEVELOP
	LayoutRefresh* edit_z = NULL;
	LayoutRefresh* edit_pos = NULL;
	LayoutRefresh* edit_width = NULL;
	LayoutRefresh* edit_height = NULL;
	LayoutRefresh* edit_enableMouse = NULL;
	//-----------------
	DrawNode* _debug_draw = NULL;
#endif
};
