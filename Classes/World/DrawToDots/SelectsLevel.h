#pragma once
#include "stdafx.h"
#include "UI/LayoutRefresh.h"
#include "Effects/EffectSprite.h"

class Level;

class SelectsLevel {
public:
	SelectsLevel(Layer* layer, std::vector<std::string> levels, float scale_ui, const Size& size);
	~SelectsLevel();

	void loadImages(float scale_ui,const Size& size);

	void pageViewEvent(Ref *pSender, PageView::EventType type);

	void setVisible(bool visible);

	void goPage(bool next);

	void setLevel(int id_level, bool isStart = false, bool isFinish = false);

	typedef std::function<void(int id_Level)> onStartLevelCallback;

	void setStartLevelCallback(const onStartLevelCallback& callback) { _callback = callback; }

	void actionFinished();

	void update(float delta);

private:
	std::vector<std::string> m_levels;
	onStartLevelCallback _callback;
	PageView* _pageView = NULL;

	std::vector<Level*> mLevels;
	int m_curr_level;
};