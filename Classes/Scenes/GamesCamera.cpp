#include "stdafx.h"
#include "GamesCamera.h"
#include "DevelopMenu.h"

GamesCamera::GamesCamera() :
	_pos()
{
	_maxRes = VisibleRect::getVisibleRect().size;
}

void GamesCamera::init(const Size& size, Node* _layer) {

	_maxRes = Size(size);

	cameraTest = NULL;
	enable = true;
	//#################
	visibleRect = VisibleRect::getVisibleRect();
	layer = _layer;

	//_maxRes = _maxRes * 2;
	setCameraTest();
	setCamera();

#if HAS_DEVELOP
	
	DEVELOP_CLEAR();
	DEVELOP_ADD_SEPARATOR(0, DevelopTabs_Settings, SeparatorType::SeparatorType_Bottom, "GamesCamera");

	edit_enable = DEVELOP_ADD_S_BOOL(0, DevelopTabs_Settings, "CameraOn", enable);

	DevelopMenu::Instance().changeDebugMode(this, ([this](const bool& _debugMode) {
		enable = !_debugMode;
		edit_enable->Refresh();
	}));

	/*
	DEVELOP_ADD_S_POINT(0, DevelopTabs_Settings, "pos", _pos);
	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "maxW", _maxRes.width);
	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "maxH", _maxRes.height);
	*/
#endif
}

void GamesCamera::drawTestR(const Vec2& anch, const Vec2& res) {
	Vec2 testR = res;
	float scale = MIN(_maxRes.width / testR.x, _maxRes.height / testR.y);
	testR.scale(scale);
	Vec2 pT = (anch*-1); pT.scale(testR);
	cameraTest->drawRect(pT, pT + testR, Color4F::BLUE);

	//####################
	//draw original
	testR = _maxRes;
	pT = (anch*-1); pT.scale(testR);
	cameraTest->drawRect(pT, pT + testR, Color4F::RED);
	//##########
}

void GamesCamera::setCameraTest(bool clear) {
	bool forceClear = NULL == cameraTest;
	if (forceClear) {
		cameraTest = DrawNode::create();
		cameraTest->setLineWidth(3);
		layer->addChild(cameraTest, 100);
	}

	if (clear || forceClear) {
		cameraTest->clear();

		Vec2 anch = Vec2::ANCHOR_MIDDLE;
		cameraTest->setAnchorPoint(anch);

		//Vec2 pT = ( anch*-1 );
		//pT.scale(_maxRes);
		//cameraTest->drawRect(pT, pT + _maxRes, Color4F::RED);

//#define TW 1280
//#define TH 800
#ifdef TW
		drawTestR(anch, Vec2(TW, TH));
		drawTestR(anch, Vec2(TH, TW));
#endif
		drawTestR(anch, Vec2(visibleRect.size));

		//cameraTest->drawPoint(Vec2::ZERO, 14, Color4F::BLUE);
		_scale = MIN(_maxRes.width / visibleRect.size.width, _maxRes.height / visibleRect.size.height);
	}
	//cameraTest->setScale(_scale);
	cameraTest->setPosition(_pos);
	//cameraTest->setPosition(visibleRect.size*_scale*-.5);
	cameraTest->setVisible(true);
}

void GamesCamera::setCamera() {
	//MathUtils::SetAnchorFromSavePosition(layer, Vec2::ANCHOR_MIDDLE);
	//Vec2 posN = layer->getPosition();
	//layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	float nScale = 1.f / _scale;
	layer->setScale(nScale);
	Vec2 size = layer->getContentSize();
	Vec2 newAnchor = Vec2(_pos.x / size.x, _pos.y / size.y);
	MathUtils::SetAnchorFromSavePosition(layer, newAnchor);
	layer->setPosition(size*.5f);
}

void GamesCamera::update(float delta) {

	if (enable) {
		if (cameraTest->isVisible()) {
			setCamera();
			cameraTest->setVisible(false);
		}
		//setCamera();
	}
	else {
		setCameraTest(true);
		//cameraTest->clear();
		//cameraTest->drawRect(e1,e2, Color4F::RED);
		//cameraTest->drawPoint(Vec2::ZERO, 14, Color4F::BLUE);
	}
}



