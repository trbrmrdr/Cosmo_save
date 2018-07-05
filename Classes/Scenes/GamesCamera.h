#pragma once
#include "stdafx.h"
#include "UI/LayoutRefresh.h"

class GamesCamera : public Singleton < GamesCamera >
{
private:
	Node* layer;

	Rect visibleRect;
	bool enable;

	DrawNode* cameraTest;

	float _scale;
	Vec2 _pos;
	Size _maxRes;
	//############
#if HAS_DEVELOP
	LayoutRefresh* edit_enable;
#endif
	void setCameraTest(bool clear = false);


	void drawTestR(const Vec2& anch, const Vec2& res);
public:
	GamesCamera();

	void init(const Size& size, Node* _layer);
	void update(float delta);

	void setCamera();

	const Size& getSize() { return _maxRes; }
};