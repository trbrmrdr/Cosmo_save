#include "stdafx.h"
#include "Bubble.h"
#include "PhysicsSpline.h"
#include "DataSpline.h"

void Bubble::toEnd(){
	root->runAction(MoveTo::create(delay_to_end, m_endPos));
	isEnd = true;
}

void Bubble::step(float delta){
	if(delay_spam <= 0 || !isEnd)return;

	if(NULL != m_dataSpline){
		DataSpline* data = TO_DATA_SPLINE(m_dataSpline);
		data->rmBody();
		m_dataSpline = NULL;
	}
	delay_spam -= delta;
	//Vec2 pos = root->getPosition();
	//m_radius
}

void Bubble::draw(Node* layer){

	if(isEnd)return;
#if 0
	//drawCircle
	glLineWidth(2);
	DrawColor(Color4B::GREEN);
	ccDrawCircle(m_startPos,
				 rMin, 360,
				 sizeSeg, false);

	ccDrawCircle(m_startPos,
				 rMax, 360,
				 sizeSeg, false);

	DrawColor(Color4B::BLACK);
	ccDrawCircle(m_startPos,
				 getRadius(), 360,
				 sizeSeg, false);
	//end drawCircle
#endif

	Vec2 t_realPos = m_startPos;
	float t_angle = .0;
	if(NULL != m_dataSpline){
		b2Body* body = TO_DATA_SPLINE(m_dataSpline)->getBody();
		//b2Transform xf = body->GetTransform();
		b2Vec2 tp = body->GetWorldCenter();
		t_angle = MATH_RAD_TO_DEG(-body->GetAngle());
		t_realPos = Vec2(tp.x, tp.y);
	}
	//draw Text
	if(!str.empty()){
		if(NULL == root){
			root = Node::create();
			root->retain();
			root->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			layer->addChild(root, 9000);
		}
		root->setPosition(t_realPos);

		root->setRotation(t_angle);
		/*
		Node* nodeText = root->getChildByName("text");
		if (forceClear && NULL != nodeText) {
			parent->removeChild(nodeText, true);
			nodeText = NULL;
		}
		*/
		if(NULL != nodeText){
			//nodeText->setString(str);
			return;
		}

		//nodeText->setCascadeOpacityEnabled(true);
		//char str[100]{};
		//sprintf(str, "%s", "текст");
		//auto label = Label::createWithTTF(str, FontBold, 95);
		//auto label = Label::createWithTTF(str, FontMarvin, 95);

		auto t_nodetext = Label::createWithTTF(str, "fonts/Dmitry Rastvortsev - DRAgu.ttf", 95,
											   Size::ZERO, TextHAlignment::CENTER,
											   TextVAlignment::CENTER);
		//label->setWidth(100);
		//label->setHeight(100);

		//label->setAnchorPoint(Vec2(0, 0));
		 //t_nodetext->setTextColor(Color4B::BLACK);
		t_nodetext->setTextColor(Color4B(
			CCRANDOM_0_1() * 255,
			CCRANDOM_0_1() * 255,
			CCRANDOM_0_1() * 255, 255
		));

		//#####################
		nodeText = t_nodetext;
		//-------------------
		Sprite*sprite = Sprite::create("opacity_bubble.png");
		sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sprite->setScale(.2);
		sprite->setColor(Color3B(
			CCRANDOM_0_1() * 255,
			CCRANDOM_0_1() * 255,
			CCRANDOM_0_1() * 255
		));
		root->addChild(sprite, 0, "sprite");
		//-------------------
		root->addChild(nodeText, 1, "text");
	}
	//end draw Text
}
