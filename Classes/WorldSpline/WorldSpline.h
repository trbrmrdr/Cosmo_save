#pragma once
#include "stdafx.h"
#if IS_IOS
#include "LayoutRefresh.h"
#else
#include "ui/LayoutRefresh.h"
#endif
#include "DevelopMenu.h" 
#include "Vertex.h"
#include "TSpline.h"

class Line;

#define TO_SPLINE(x)		(static_cast<TSpline*>(x))

typedef std::function<void(TSpline* spline, __Dictionary* dictInSplines)> callback_load_save;


class WorldSpline
{
protected:
	const std::string NAME_CHILD_TEXT = "text";

	float _scale_from_draw;
	bool _enable_save;
	bool _enable_text;
	Color3B _color_text;
	Node* _parentWOBJ;

	Vec2 _pos;
	DevelopTabs _tabs;

	std::string _fileName;
	bool _isVisible;
	int _z;

	int _maxCountSplines;

	std::vector< TSpline* > _TSplines;
	Vertex* _isAllocated_prev = NULL;
	Vertex* _isAllocated = NULL;
	TSpline* _isEditableSpline = NULL;

	bool _splineChanged;
	bool _recreateLabel;
	bool isAutoCreate;
	Vec2 _lastPos;

	bool _isEdit;

#if HAS_DEVELOP
	LayoutRefresh* m_editCreateSpline = NULL;
	LayoutRefresh* edit_isVertexType = NULL;

	//callback_load_save _loadChild;
	//callback_load_save _saveChild;
#endif
	virtual void select(bool force = false);
	virtual void deselect();

	virtual Node* createParent(Node* layer, const Vertex& p1);
public:
	WorldSpline(int max_count_spline = -1, DevelopTabs tabs = DevelopTabs_Spline, bool createEdits = false);

	virtual ~WorldSpline();

	virtual bool changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState);

#if HAS_DEVELOP
	void changeEdits(bool create);
#endif

	std::vector<TSpline*> getPolygons();

	void free();

	virtual void save(bool force = false, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f);
	virtual void load(const std::string& levelName, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f);


	void freeClear();
	void addSpline(const std::string& levelName, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f);

	/*
	void setCallback(const callback_load_save& load, const callback_load_save& save){
		_loadChild = load;
		_saveChild = save;
	}
	*/

	virtual void loadChild(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f) {};
	virtual void saveChild(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f) {};

	virtual bool mouseDown(const Vec2& mousePos, bool isCreated = true);
	virtual bool mouseMovie(const Vec2& mousePos, Vec2* dPos = NULL);
	virtual bool mouseUp(const Vec2& mousePos);

	bool checkIntersectVertex();



	bool stopCreate(bool force = false);

	virtual bool setChangedSpline(bool splineChanged);
	bool getChangedSpline() { return _splineChanged; };

	virtual void draw();

	void setScaleFromDraw(float scale);

	virtual Node* printText(Node* parent, Vertex* p1, int id, bool forceClear = false);

	virtual void update(float delta) {};

	virtual bool isChangedSpline();


	virtual int getZ() { return _z; };
	virtual void setZ(int zOrder) { _z = zOrder; };

	virtual void setVisible(bool visible, bool force = false) { _isVisible = visible; }
	virtual bool getVisible() { return _isVisible; }


	void setEnableText(bool enable) { _enable_text = enable; };

	void setColorText(Color3B color, bool reset = false);
	Color3B getColorText() { return _color_text; };

	void setEnableSave(bool enable) { _enable_save = enable; }

	void setDPos(const Vec2& dPos) {
		for (TSpline* _tspline : _TSplines) {
			for (TSpline* _tspline : _TSplines) {
				for (Vertex* v1 : _tspline->getControl()) {
					v1->setPos(v1->getPos() - dPos, true);
				}

				//todo для быстрой отрисовки (а так по сути ненадо)
				for (Vertex* v1 : _tspline->getSpline()) {
					v1->setPos(v1->getPos() - dPos, true);
				}
			}
		}
	}

};
