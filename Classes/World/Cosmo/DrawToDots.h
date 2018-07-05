#pragma once
#include "stdafx.h"
#include "WorldSpline/WorldSpline.h"
#include "Effects/EffectRenderTexture.h"
//#include "Brush.h"

class Brush;
class Line;

class DrawToDots: public WorldSpline {
public:
	typedef std::function<Vec2(const Vec2&)> converterFunc;
protected:
	const std::string NAME_CHILD_CIRCLE = "circle";
private:

	EffectRenderTexture * m_rt_toEnd;//screen render in texture from finish image

	bool m_clearRT = false;
	EffectRenderTexture* m_rt_0;//white background
	Sprite* m_back_img;					//back if needed
	EffectRenderTexture* m_rt_1;//draw tmp points
	EffectRenderTexture* m_rt_2;//succes draw points

	Sprite* m_brush_sprite;

	Size  m_size;
	float m_scale;

	Node * _layer;


	Texture2D* _point_texture;
	float _scale_node_circle;

	float _dContainsR = 8;// 25;// 50;
	float _dDrawR = 2.5;// 8;
	float _brush_scale = .25f;	// =>32

	Vec2 _c_pos_rt_texture;
	float _elapsed;
	double _last_change_currPos;
	double _last_end_helper;
	uint mLastAdd;
	bool isEndGame;
	bool isEndGameAnim;

	virtual void select(bool force = false);
	void recalc();

	bool _isChanged;

	void setBrushSize();

	int _need_save_texture;
	callback_render_texture _callback_render_texture;
	bool hasStartGame;

	SettingUtil* _setting;
	Color3B _color_circle_start;
	Color3B _color_yes;

	void resetColor() {
		for (Vertex* p1 : getControlAll()) {
			Node* parent = p1->node;
			if (NULL == parent)continue;
			parent->removeFromParent();
			p1->node = NULL;
		}
	};

	converterFunc m_converter_coords;
public:


	void setColorYes(Color3B color, bool reset = false) {
		_color_yes = color;
		if (reset)resetColor();
	}

	Color3B getColorYes() { return _color_yes; };

	void setColorCircleStart(Color3B color, bool reset = false) {
		_color_circle_start = color;
		if (reset)resetColor();
	}

	Color3B getColorCircleStart() { return _color_circle_start; }


	//#######################

	DrawToDots(Node* layer, float sclae_ui, const Size& size);
	virtual ~DrawToDots();
	virtual void load(const std::string& levelName, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f) override;
	virtual void save(bool force = false, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f) override;

	void saveSetting();

	virtual bool changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState);

	virtual bool mouseDown(const Vec2& mousePos, bool isCreated = true);
	virtual bool mouseMovie(const Vec2& mousePos, Vec2* dPos = NULL);
	virtual bool mouseUp(const Vec2& mousePos);

	virtual bool setChangedSpline(bool splineChanged);
	virtual void draw();
	virtual void update(float delta = .0);

	bool flyDraw(const Vec2& pos, bool up = false);
	void startShake(int id, int type);

	virtual Node* printText(Node* layer, Vertex* p1, int id, bool forceClear = false) override;
	void drawCircle(Node* layer, Vertex* p1, bool changed, bool shake = false, int type = 0);
	uint drawBrush(const Vec2& changedPos, bool force = false);

	bool hasStart() { return hasStartGame; }
	bool isEnd() { return isEndGame; }
	bool isEndAnim() { return isEndGameAnim; }

	void finishGame();

	virtual void setVisible(bool visible, bool force = false);

	void setParam(float _dContainsR = 0.f, float _dDrawR = 0.f, float _scaleBr = 0.f);

	void setBackground(const std::string& fileName) {
		m_back_img->setTexture(fileName);
	}

	void createHelper(uint fromPosId, uint toPosId);

	void saveTexture(const callback_render_texture& callback);

	void reset();

	//################################ add asistant

	bool hasSpline() { return _TSplines.size() != 0; }
	//std::vector<Vertex*> getSpline() { return _TSplines[0]->getSpline(); }

	int m_control_size = -1;
	void controllReset() {
		m_control_size = 0;
		for (Vertex* v1 : _TSplines[0]->getControl()) {
			if (v1->hasAssistant())continue;
			m_control_size++;
		}
	}

	int getControlLSize() { return m_control_size; }

	Vertex* getControl(int id) {
		if (_TSplines.empty()) return NULL;
		int i = 0;
		for (Vertex* v1 : _TSplines[0]->getControl()) {
			if (v1->hasAssistant())continue;
			if (i == id)return v1;
			i++;
		}
		return  NULL;
	}

	std::vector<Vertex*> getControlAll() {
		if (_TSplines.empty()) return std::vector<Vertex*>();
		return _TSplines[0]->getControl();
	}


	//if count = -1 to  count from create each sector 
	Vec2* createSegments(int start_id, int end_id, int* size_segment, float* length = NULL, bool align = false);


	//__________________
	Size  getSizeContent() { return m_size; };
	float getScaleContent() { return m_scale; };
};