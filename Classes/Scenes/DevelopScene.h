#pragma once
#include "stdafx.h"
#include "GamesCamera.h"
#include "DevelopMenu.h"
#include "World/Cosmo/World_cosmo.h"

class DevelopScene: public Layer {
public:
	enum SceneNames {
		SN_First
	};

private:
	World * world;
	float _inteval_update = -1.0f;
	bool _is_play_update = false;
	bool _is_play = true;


	DrawNode * _backMesh = NULL;
	DrawNode* _colorBack = NULL;
	Button* buttonFS = NULL;


	Node* _layer;
	Vec2 mousePos;
	Vec2 startPos;

	Vec2 sceneLastPos;


	bool worldMoovie;

#if HAS_DEVELOP
	void setDevelopMode(bool enable);
#endif
	void createListeners();
	SceneNames m_name;
public:
	DevelopScene(SceneNames name);
	virtual ~DevelopScene();

	static void ChangeToScene(SceneNames toScene);

	static Scene* createScene(SceneNames name);

	virtual bool init() override;

	virtual void update(float delta);

	virtual bool updateChangeEvents(EventKeyboard::KeyCode keyCode, KeyState keystate = KeyState_NONE);

	virtual bool onMouseDown(Vec2 mousePos, EventMouse::MouseButton mouseCode);
	virtual bool onMouseUp(Vec2 mousePos, EventMouse::MouseButton mouseCode);
	virtual bool onMouseMove(Vec2 mousePos, EventMouse::MouseButton mouseCode);
	virtual void onMouseScroll(Event* event);

	void Exit();

	void setIntevalUpdate(bool isPlay, float inteval);

	cocos2d::CustomCommand _customCommand;
	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
	void drawScene(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags);

protected:
	void drawMesh();
};
