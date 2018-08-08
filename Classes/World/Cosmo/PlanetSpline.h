#pragma once
#include "stdafx.h"
#include "WorldSpline/WorldSpline.h"
#include "Effects/EffectRenderTexture.h"

class PlanetSpline: public WorldSpline {
private:
	Node * _layer;
	SettingUtil* _setting;
	Vec2 m_pos;

public:


	PlanetSpline(Node* layer);
	virtual ~PlanetSpline();
	virtual void load(const std::string& levelName, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f) override;
	virtual void save(bool force = false, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f) override;	

	virtual bool changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState);

	virtual bool mouseDown(const Vec2& mousePos, bool isCreated = true);
	virtual bool mouseMovie(const Vec2& mousePos, Vec2* dPos = NULL);
	virtual bool mouseUp(const Vec2& mousePos);

	virtual bool setChangedSpline(bool splineChanged);
	virtual void draw();
	virtual void update(float delta = .0);

	void create_circle(int segment);

	void setPos(const Vec2& pos) { m_pos = pos; }
};