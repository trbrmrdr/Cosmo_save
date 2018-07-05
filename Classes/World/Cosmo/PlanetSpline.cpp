#include "stdafx.h"
#include "PlanetSpline.h"
#include "DevelopMenu.h"
#include "WorldSpline/TSpline.h"

int m_segment = -1;
float tension = .25;
int segment = 50;
bool change_spline = false;

PlanetSpline::PlanetSpline(Node* layer):
	_layer(layer),
	WorldSpline(1, DevelopTabs_Spline, true) {

	//load("test.xml");
	create_circle(4);
#if HAS_DEVELOP


	/*
	DEVELOP_ADD_SEPARATOR(0, DevelopTabs_Settings, SeparatorType::SeparatorType_Bottom, "PlanetSpline");


	DevelopMenu::Instance().addColorSetting(this, 0, DevelopTabs_Settings,
											"color_yes",
											([this](const Color3B& color) { setColorYes(color, true); }),
											([this](void) { return  getColorYes(); }));

	DevelopMenu::Instance().addColorSetting(this, 0, DevelopTabs_Settings,
											"color_circle_start",
											([this](const Color3B& color) { setColorCircleStart(color, true); }),
											([this](void) { return  getColorCircleStart(); }));


	DevelopMenu::Instance().addColorSetting(this, 0, DevelopTabs_Settings,
											"color_text",
											([this](const Color3B& color) { setColorText(color, true); }),
											([this](void) { return  getColorText(); }));

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings,
													  "dContainsR",
													  ([this](const float& val) { _dContainsR = val; setBrushSize(); }),
													  ([this](void) { return _dContainsR; }), 5.f, 100.f);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings,
													  "dDrawR",
													  ([this](const float& val) { setParam(_dContainsR, val, _brush_scale); resetColor(); }),
													  ([this](void) { return _dDrawR; }), 1.f, 20.f);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 0, DevelopTabs_Settings,
													  "brush_scale",
													  ([this](const float& val) { _brush_scale = val; setBrushSize(); }),
													  ([this](void) { return _brush_scale; }), .1f, 2.f);
	*/

	//DevelopMenu::Instance().changeDebugMode(this, ([this](const bool& val) { if (!val)saveSetting(); }));


	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "m_brush_scale", m_brush_scale);
	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "scaleDistance", scaleDistance);
	//DEVELOP_ADD_S_FLOAT(0, DevelopTabs_Settings, "scaleInc", scaleInc);
#endif
	DevelopMenu::Instance().addFloatSliderSetting(this, 0, DevelopTabs_Spline, "count_point",
												  [this](const float& val) {create_circle((int)val); },
												  [this](void) {return m_segment; },
												  SeparatorType_Bottom,
												  [this](void) {return 3.f; }, [this](void) {return 20.f; });

	DevelopMenu::Instance().addFloatSliderSetting(this, 0, DevelopTabs_Spline, "tension",
												  [this](const float& val) {tension = val; change_spline = true; },
												  [this](void) {return tension; },
												  SeparatorType_Bottom,
												  [this](void) {return -10.f; }, [this](void) {return 10.f; });


	DevelopMenu::Instance().addFloatSliderSetting(this, 0, DevelopTabs_Spline, "segment",
												  [this](const float& val) {segment = val; change_spline = true; },
												  [this](void) {return segment; },
												  SeparatorType_Bottom,
												  [this](void) {return 1.f; }, [this](void) {return 100.f; });

}


class AnimVertex {
public:
	Vec2 pos;
	Vec2 pos_s;

	Vec2 from;
	Vec2 to;
	float delay;

public:
	AnimVertex() {}
	virtual ~AnimVertex() {}

	float radius;
	float rads;
	float coef;
	void setTmp(float _radius, float _rads, float _coef, const Vec2& _pos) {
		radius = _radius;
		rads = _rads;
		coef = _coef;
		pos_s = _pos;
	}

	AnimVertex(const Vec2& from, const Vec2& to, float delay):
		from(from),
		to(to),
		delay(delay) {
		elapsed = delay * CCRANDOM_0_1();
		sign = CCRANDOM_0_1() > .5 ? +1 : -1;
	}

	int sign = +1;
	float elapsed = .0f;
	void update(float delta) {
		if (from.isZero())return;
		elapsed += delta;
		float percent = MIN(elapsed / delay, 1.);

		pos = from + (to - from)*(sign > 0 ? percent : 1. - percent);

		if (percent >= 1.) {
			elapsed = .0;
			sign *= -1;
		}
	}

	Vec2 getPos() { return pos; }
};



Vec2 getVCircle(float radius, float rads) {
	return Vec2(radius * cosf(rads), radius * sinf(rads));
}

void PlanetSpline::create_circle(int segment) {

	if (segment == m_segment)return;

	free();
	m_segment = segment;
	for (int i = 0; i < 1; ++i) {
		TSpline* spline = new TSpline();

		Vec2 center = Vec2::ZERO;
		float radius = 100;
		const float coef = 2.0f * (float)M_PI / m_segment;

		for (unsigned int i = 0; i < m_segment; i++) {
			float rads = i * coef;

			Vec2 pos = center + getVCircle(radius, rads);


			float coef_2 = coef * .5;
			Vec2 from = pos + getVCircle(radius * .5, rads - coef_2 + 2.f*coef_2*CCRANDOM_0_1());
			Vec2 to = pos + getVCircle(radius * .5, rads + M_PI - coef_2 + 2.f*coef_2*CCRANDOM_0_1());
			float delay = 1. + 2.*CCRANDOM_0_1();

			Vertex* vertex = new Vertex(from,
										//TypeVertex_Reflected
										TypeVertex_Point
			);

			AnimVertex* anim = new AnimVertex(from, to, delay);
			vertex->setData(anim);
			anim->setTmp(radius*.5, rads, coef_2, pos);

			vertex->setCountfromBezier(10);
			//vertex->setAssistant(true);
			spline->pushBackControl(vertex, false);
		}

		spline->setClosed(true, false);
		//___
		_TSplines.push_back(spline);
	}
	setChangedSpline(true);
}

void PlanetSpline::save(bool force /*= false*/, const Vec2& d_pos/* = Vec2::ZERO*/, const float& scale /*= 1.f*/) {
	WorldSpline::save(force, d_pos, scale);
}

void PlanetSpline::load(const std::string& levelName, const Vec2& d_pos/* = Vec2::ZERO*/, const float& scale /*= 1.f*/) {
	//_setting = new SettingUtil(levelName + ".setting");
	//setColorYes(_setting->getColor3BForKey("_color_yes"));
	//setColorCircleStart(_setting->getColor3BForKey("_color_circle_start"));
	//setColorText(_setting->getColor3BForKey("_color_text"));

	WorldSpline::load(levelName, d_pos, scale);

}

bool PlanetSpline::changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState) {
#if HAS_DEVELOP
	if (WorldSpline::changeKeys(keyCode, keyState))return true;
	//if (!DevelopMenu::Instance().isEnable())return false;
	bool ret = false;
	switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_Z:
			if (keyState == KeyState_RELEASE) {

				ret = true;
			}
			break;

		case EventKeyboard::KeyCode::KEY_R:
			if (keyState == KeyState_RELEASE) {

				ret = true;
			}
			break;
			/*
			case EventKeyboard::KeyCode::KEY_T:
			if (keyState == KeyState_RELEASE) {
			setBrushSize();
			ret = true;
			}
			break;
			/**/
	}
	return ret;
#endif
	return false;
}

PlanetSpline::~PlanetSpline() {}

void PlanetSpline::update(float delta) { //if isLast delta ==0
	if (!change_spline) {
		change_spline = isChangedSpline();
	}

	for (TSpline* _tspline : _TSplines) {
		for (Vertex* v1 : _tspline->getControl()) {
			AnimVertex* v = (AnimVertex*)(v1->getData());
			v->update(delta);

			v1->setPos(v->getPos());
		}
	}
	setChangedSpline(true);
}

bool PlanetSpline::mouseDown(const Vec2& mousePos, bool isCreated) {
	if (!_isVisible) return false;
	if (WorldSpline::mouseDown(mousePos, isCreated))	return true;
	return true;
}

bool PlanetSpline::mouseMovie(const Vec2& mousePos, Vec2* dPos) {
	if (!_isVisible) return false;
	if (WorldSpline::mouseMovie(mousePos, dPos))return true;
	return	true;
}

bool PlanetSpline::mouseUp(const Vec2& mousePos) {
	if (!_isVisible) return false;
	if (WorldSpline::mouseUp(mousePos))
		return true;
	return true;
}

bool PlanetSpline::setChangedSpline(bool splineChanged) {
	if (splineChanged) {
		//controllReset();
		//recreate excude assistant
	}
	//change_spline = splineChanged;
	return WorldSpline::setChangedSpline(splineChanged);
}

Vec2 createCardinalSplineAt(const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, float tension, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	/*
	* Formula: s(-ttt + 2tt - t)P1 + s(-ttt + tt)P2 + (2ttt - 3tt + 1)P2 + s(ttt - 2tt + t)P3 + (-2ttt + 3tt)P3 + s(ttt - tt)P4
	*/
	float s = (1 - tension) / 2;

	float b1 = s * ((-t3 + (2 * t2)) - t);                      // s(-t3 + 2 t2 - t)P1
	float b2 = s * (-t3 + t2) + (2 * t3 - 3 * t2 + 1);          // s(-t3 + t2)P2 + (2 t3 - 3 t2 + 1)P2
	float b3 = s * (t3 - 2 * t2 + t) + (-2 * t3 + 3 * t2);      // s(t3 - 2 t2 + t)P3 + (-2 t3 + 3 t2)P3
	float b4 = s * (t3 - t2);                                   // s(t3 - t2)P4

	float x = (p0.x*b1 + p1.x*b2 + p2.x*b3 + p3.x*b4);
	float y = (p0.y*b1 + p1.y*b2 + p2.y*b3 + p3.y*b4);

	return Vec2(x, y);
}

Vec2 createSplineAt(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float tension, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	return
		(-.5f * tension * t3 + tension * t2 - .5f * tension * t) * p0 +
		(1 + .5f * t2 * (tension - 6) + .5f * t3 * (4 - tension)) * p1 +
		(.5f * t3 * (tension - 4) + .5f * tension * t - (tension - 3) * t2) * p2 +
		(-.5f * tension * t2 + .5f * tension * t3) * p3;
}

#include "ExtendedNode/DrawNode2.h"
DrawNode2* drawNode(NULL);

void PlanetSpline::draw() {
	//WorldSpline::draw();

	if (!drawNode)_layer->addChild(drawNode = DrawNode2::create());
	if (change_spline)
		drawNode->clear();
	//________________________

	for (TSpline* _tspline : _TSplines) {
		{

			Vertex* v2 = NULL;
			Vertex* first = NULL;

			for (Vertex* v1 : _tspline->getSpline()) {
				///*
				if (!first)first = v1;
				v1->Draw(false, _isEdit);
				if (v2) Vertex::Draw(v2, v1, false, true, _isEdit);
				v2 = v1;
				/**/
			}


			if (_tspline->IsClosed()) {
				Vertex::Draw(v2, first, false, true, _isEdit);
			}

		}
		//###################################
#if 1
		{
			Vertex* v2 = NULL;
			Vertex* first = NULL;
			int i = 0;


			PointArray2* pointArray = new PointArray2();
			pointArray->autorelease();

			for (Vertex* v1 : _tspline->getControl()) {
				i++;
				if (NULL == first) {
					first = v1;
				}
				v1->Draw(true, _isEdit);
				if (v2) Vertex::Draw(v2, v1, false, false, _isEdit);
				v2 = v1;
				//_____________
				if (change_spline) {
					pointArray->addControlPoint(v1->getPos());
					AnimVertex* v = (AnimVertex*)(v1->getData());

					drawNode->drawLine(v->from, v->to, Color4F::RED);

#if 0
					drawNode->drawLine(v->pos_s, v->pos_s + getVCircle(v->radius, v->rads - v->coef), Color4F::BLACK);
					drawNode->drawLine(v->pos_s, v->pos_s + getVCircle(v->radius, v->rads + v->coef), Color4F::BLACK);

					drawNode->drawLine(v->pos_s, v->pos_s + getVCircle(v->radius, v->rads + M_PI - v->coef), Color4F::BLACK);
					drawNode->drawLine(v->pos_s, v->pos_s + getVCircle(v->radius, v->rads + M_PI + v->coef), Color4F::BLACK);
#endif
				}
				/*
				if (change_spline) {
					///*
					if (!v1->getTail())continue;
					const Vec2 prev = v1->getTail()->getPos();
					Vec2 currStart = v1->getPos();
					if (!v1->getHead())continue;
					const Vec2 currEnd = v1->getHead()->getPos();
					if (!v1->getHead()->getHead())continue;
					const Vec2 next = v1->getHead()->getPos();
					int step = 0;
					Vec2 t_prev;
					Vec2 interpolatedPoint;
					for (int step = 0; step <= segment; step++) {
						float t = (float)step / segment;
						//______________________________________________
						interpolatedPoint = createSplineAt(prev, currStart, currEnd, next, tension, t);

						//if (0 == step)
						//	drawNode->drawLine(v1->getPos(), interpolatedPoint, Color4F::BLUE);
						//else
						if (0 != step)
							drawNode->drawLine(t_prev, interpolatedPoint, Color4F::BLUE);
						t_prev = interpolatedPoint;
						//result.Add(interpolatedPoint);
					}
					//drawNode->drawLine(interpolatedPoint, v2->getPos(), Color4F::BLUE);
				}
				/**/
			}

			if (_tspline->IsClosed()) {
				Vertex::Draw(v2, first, false, false, _isEdit);
				//pointArray->addControlPoint(first->getPos());
			}



			if (change_spline) {
				drawNode->drawCardinalSpline(pointArray, tension, segment, Color4F::GREEN);
				change_spline = false;
			}
		}
#endif
	}

	if (!_TSplines.empty())
		_recreateLabel = false;
}

