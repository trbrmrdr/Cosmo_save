#pragma once
#include "stdafx.h"
#include <Box2D/Box2D.h>

using namespace std;

class Bubble {
public:
	void* m_dataSpline = NULL;
	string str;
	Vec2 m_startPos;
	Vec2 m_endPos;
	int sizeSeg = 8;
	float rMin = 10.f;
	float rMax = 20.f;
	float m_radius;
	float getRadius() { return rMin + ( rMax - rMin )*CCRANDOM_0_1(); }
	Node* nodeText = NULL;
	Node* root = NULL;

	float delay_to_end = .5f;
	float delay_spam = .3f;
	bool isEnd = false;

	Bubble(const Vec2& pos):
		m_startPos(pos) {
		m_radius = getRadius();
	}

	~Bubble() {
		if (NULL != root) {
            root->removeAllChildren();
			root->release();
			root = NULL;
		}
	}

	void draw(Node* layer);

	void toEnd();
	void step(float delta);


	/*
	static void Step(Node* layer, float delta, const vector<Bubble*>& bubbles) {
		bool t_develop_disable = !DevelopMenu::Instance().isEnable();
     for ( Bubble* bubble : bubbles ) {
			bubble->step(delta);
			//bubble->draw(layer);
		}
	}

	*/

	static void Draw(Node* layer, const vector<Bubble*>& bubbles) {
        for ( Bubble* bubble : bubbles ) {
			bubble->draw(layer);
		}
	}
};
