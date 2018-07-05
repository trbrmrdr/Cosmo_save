
#include "stdafx.h"
#include "PhysicsSpline.h"
#include "DevelopMenu.h"
#include "Effects/EffectRenderTexture.h"
#include "Scenes/GamesScene.h"
#include "WorldSpline/TSpline.h"
extern GLESDebugDraw* m_debugDraw;
//float mj_dampingRatio = 1.1;
//float mj_frequencyHz = 2.1;

int velocityIterations = 4;// 9;
int positionIterations = 7;// 9;
float timeStep = 0.01666;//1/60
b2World* m_b2world = NULL;
bool m_b2dRecreate = false;
bool m_b2dPlay = true;

b2MouseJoint* m_mouseJoint = NULL;
DataSpline* m_rect_sensor = NULL;
DataSpline* m_rect_sensor_go_up = NULL;
DataSpline* m_rect_ = NULL;

//размер пальца пересекающегося сшариком и ниткой
const float segments = 20.f;
const float m_radius = 2.f;

static float _scaleWorld = .5;
Rect _curr_rect;

PhysicsSpline::PhysicsSpline(Node* layer, Rect rect_l, Rect rect_r, float dlh, World* world):
	WorldSpline_and_Physics::WorldSpline_and_Physics(-1, DevelopTabs_Spline, true),
	//m_b2world(NULL),
	mFactorStep(10),
	mFactorStep_2(1),
	mFactorMass(1),
	m_gravity(.0f, -.8f),

	//todo
	_world(world),

	_layer(layer) {

	_rect_small = rect_l;
	_rect_small.origin.x += dlh; _rect_small.size.width -= dlh * 2.2;
	
	_rect_big = Rect(rect_l.origin, Size(rect_l.size.width + rect_r.size.width, rect_r.size.height));
	_rect_big.origin.x += dlh; _rect_big.size.width -= dlh * 1.5;
	setEnableText(false);
	//###############
	//m_isEditPhysics 
	//################
#if HAS_DEVELOP
	m_editCreateB = DevelopMenu::Instance().addBoolSetting(this, 0, DevelopTabs_Spline, "Create Bubble",
														   CC_CALLBACK_1(PhysicsSpline::setCreateBubble, this),
														   ([this](void) { return m_isCreateBubble; }), SeparatorType_Bottom);

	//------------------------------------------------------
#if 0
	DEVELOP_ADD_S_POINT(2, DevelopTabs_Settings, "_rect", _rect);
	DEVELOP_ADD_S_POINT(2, DevelopTabs_Settings, "_d_pos_rect", _d_pos_rect);
#endif
	//------------------------------------------------------

	DEVELOP_ADD_S_BOOL(2, DevelopTabs_Settings, "b2 Play", m_b2dPlay);
	DEVELOP_ADD_S_BUTTON(2, DevelopTabs_Settings, "b2 Recreate", m_b2dRecreate);

	//DEVELOP_ADD_SEPARATOR(0, DevelopTabs_Settings, SeparatorType::SeparatorType_Bottom, "PhysicsSpline");
	//DEVELOP_ADD_S_POINT(0, DevelopTabs_Spline, "gravity", m_gravity);

	//DEVELOP_ADD_S_UINT(0, DevelopTabs_Spline, "factor step", mFactorStep);
	//DEVELOP_ADD_S_UINT(0, DevelopTabs_Spline, "factor step 2", mFactorStep_2);
	//DEVELOP_ADD_S_UINT(0, DevelopTabs_Spline, "factor mass", mFactorMass);


	DevelopMenu::Instance().addFloatSettingFromMinMax(
		this, 3, DevelopTabs_Settings,
		"scaleWorld",
		([this](const float& val) {
		_scaleWorld = val;
		m_debugDraw->setRatio(_scaleWorld);
	}),
		 ([this]() {return _scaleWorld; }), .0, 10., SeparatorType_Stroke, .1f);

#endif
	createBox2d();

	const float coef = 2.0f * (float)M_PI / segments;
	GLfloat *vertices = (GLfloat*)calloc(sizeof(GLfloat) * 2 * (segments + 2), 1);
	if (!vertices)
		return;

	Vec2 t_prev;
	bool t_first = true;
	for (unsigned int i = 0; i <= segments; i++) {
		float rads = i * coef;
		GLfloat j = m_radius * cosf(rads + 360.);
		GLfloat k = m_radius * sinf(rads + 360.);
		m_tap_circle.push_back(Vec2(j, k));

	}
	Vec2 tc = Vec2(m_radius * cosf(0.7853981625), m_radius * sinf(0.7853981625));

	m_tap_cross.push_back(tc);
	m_tap_cross.push_back(-tc);

	m_tap_cross.push_back(Vec2(tc.x, -tc.y));
	m_tap_cross.push_back(Vec2(-tc.x, tc.y));
}
#if HAS_DEVELOP
void PhysicsSpline::setCreateBubble(bool enable) {
	m_isCreateBubble = enable;
	if (m_isCreateBubble) {
		stopCreate(true);
		m_editCreateSpline->Refresh();

		setEditPhysics(false);
		//m_editEditPhys->Refresh();
	}

}

void PhysicsSpline::setEditPhysics(const bool& enable) {
	m_isEditPhysics = enable;
	if (m_isEditPhysics) {
		stopCreate(true);
		m_editCreateSpline->Refresh();

		setCreateBubble(false);
		m_editCreateB->Refresh();
	}
	/*
	if(m_isEditPhysics){

	}
	else{
		DevelopMenu::Instance().eraceSettings(this, 2);
	}
	*/
}
#endif
void PhysicsSpline::createBox2d() {
	//sonyab2d = NULL;
	if (NULL != m_b2world)
		delete m_b2world;

	m_b2world = new b2World(b2Vec2(m_gravity.x, m_gravity.y));
	m_b2world->SetContactListener(this);

	//m_b2world->SetAllowSleeping(true);
	//m_b2world->SetContinuousPhysics(true);
	//m_b2world->SetWarmStarting(true);
	if (!m_debugDraw)
		m_debugDraw = new GLESDebugDraw();
	m_b2world->SetDebugDraw(m_debugDraw);

	uint32 flags = 0;
	flags += 1 * b2Draw::e_shapeBit;
	flags += 1 * b2Draw::e_jointBit;

	//flags += 1 * b2Draw::e_aabbBit;
	//flags += 1 * b2Draw::e_pairBit;

	//flags += 1 * b2Draw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);

	b2BodyDef bodyDef;
	m_groundBody = m_b2world->CreateBody(&bodyDef);
	//########//reclear physics body
	/*
	 for ( TSpline* Tspline : getPolygons() ) {
		void* data = Tspline->getData();
		if (NULL != data) {
			DataSpline* t_obj = static_cast<DataSpline*>( Tspline->getData() );
			t_obj->setBody(NULL);
		}
	}
	*/
}

void PhysicsSpline::createPhysics(bool recreate) {
	if (recreate) {
		createBox2d();

		m_tornado_bybbles.clear();
		m_check_end_bybbles.clear();
		m_go_up_bybbles.clear();
	}

	//createFixture(getPolygons(), recreate);
	//______________________

	if (!m_rect_sensor || recreate) {
		if (m_rect_sensor) {
			delete m_rect_sensor;
			delete m_rect_sensor_go_up;
			delete m_rect_;
		}

		m_rect_sensor = new DataSpline(_curr_rect.origin, _curr_rect.size, true);//sensor -> action in circle

		Vec2 d_s = _curr_rect.size*.1f;
		m_rect_sensor_go_up = new DataSpline(_curr_rect.origin - Vec2(.0f, _curr_rect.size.height*1.2f) - d_s, _curr_rect.size*1.2f, true);
		m_rect_sensor_go_up->test_check = 2;
		m_rect_ = new DataSpline(_curr_rect.origin, _curr_rect.size, false);//static -> непокинет его
	}
	//###############

	int i = 0;
	for (TSpline* t_spline : getPolygons()) {
		//if(i >= 1) 
		CreateFixture(t_spline, recreate);
		DataSpline* data = GetData(t_spline);
		continue;

		//splines.empty.add dubl no water spline
		if (i == 2) {
			data->test_check = true;
		}
		else if (i == 1) {
			data->setFilter_b();
		}
		else {
			//data->setFilter_b();
		}
		i++;
	}
	//-----------------------------------------

	for (auto& it : _world->getObjs()) {
		Soul* soul = it.second;
		SoulType type = (SoulType)soul->getType();
		switch (type) {
			case SoulType_PhysicsObject:
				static_cast<PhysicsObject*>(soul)->createPhysics(recreate);
		}
	}
}

void PhysicsSpline::load(Type_World type_world) {
	//WorldSpline::load(fileName, d_pos, scale);
	_isAllocated = NULL;
	free();
	//###############
	TSpline* spline = new TSpline();

	switch (type_world) {
		case PhysicsSpline::TW_Big:
			_curr_rect = _rect_big;
			break;
		case PhysicsSpline::TW_Small:
			_curr_rect = _rect_small;
			break;
	}

	Vec2 dxy = _curr_rect.size*.2;

	Vec2 t_p = _curr_rect.origin + Vec2(_curr_rect.size.width, .0);// +Vec2(dxy.x, -dxy.y);
	std::vector<Vec2> points{
		//Vec2(-265.26, -352.46),
		//Vec2(-180.0, -134.96),
		_curr_rect.origin - Vec2(_curr_rect.size.width*2.,_curr_rect.size.height*4.),
		_curr_rect.origin - _curr_rect.size,

		_curr_rect.origin ,//- dxy,
		_curr_rect.origin + Vec2(.0f,_curr_rect.size.height*2.),// - Vec2(dxy.x,-dxy.y),

		_curr_rect.origin + Vec2(_curr_rect.size.width,_curr_rect.size.height*2.) ,//+ dxy,
		t_p,
		//Vec2(-92.45, -55.65),
		//Vec2(-91.35, 93.09),
		//Vec2(25.7, 92.7),
		//Vec2(27.53, -55.84),

		t_p + Vec2(_curr_rect.size.width,-_curr_rect.size.height),
		t_p + Vec2(_curr_rect.size.width*2.,-_curr_rect.size.height*4.)
		//Vec2(126.83, -140.59),
		//Vec2(-267.33, -336.09)
	};

	for (Vec2 point : points)
		spline->pushBackControl(new Vertex(point, TypeVertex::TypeVertex_Point), false);


	_TSplines.push_back(spline);
	setChangedSpline(true);

	m_b2dRecreate = true;
	//setEnableSave(false);
}

b2Body* PhysicsSpline::addCircle(const Vec2& pos, int countV, float radius, float dencity) {
	float t_radius = radius == -1 ? MAX(0.5f, CCRANDOM_0_1()) * 50 : radius;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

#if 0
	bodyDef.position.Set(p.x, p.y);
	b2Body *ret_body = m_b2world->CreateBody(&bodyDef);

	b2PolygonShape shape;
	//###################################
	int t_count_v = 8;
	const float coef = 2.0f * (float)M_PI / t_count_v;
	b2Vec2 *vertices = new b2Vec2[t_count_v];
	for (unsigned int i = 0; i < t_count_v; i++) {
		float rads = i * coef;
		vertices[i] = b2Vec2(t_radius * cosf(rads), t_radius * sinf(rads));
	}
	//###################################
	shape.Set(vertices, t_count_v);

	b2MassData massData;

	float space = _M_PI * pow<float>(t_radius, 2);
	massData.mass = space * (float)mFactorMass;
	t_body->SetMassData(&massData);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	//fixtureDef.density = RandomHelper::random_real<float>(.1f, 500.);
	fixtureDef.density = CCRANDOM_0_1()*1.5f;// 1;//.25;//
											 //fixtureDef.friction = .4;// CCRANDOM_0_1();
											 //fixtureDef.restitution = 0.1;

	t_body->CreateFixture(&fixtureDef);
#else

	b2Body *ret_body = m_b2world->CreateBody(&bodyDef);
	b2MassData massData;

	float space = _M_PI * pow<float>(t_radius, 2);
	massData.mass = space * (float)mFactorMass;
	massData.center = b2Vec2(pos.x, pos.y);
	ret_body->SetMassData(&massData);

	b2FixtureDef t_fd;
	t_fd.density = dencity == -1 ? 0.9 + CCRANDOM_0_1()*.1f : dencity;
	//fixtureDef.density = CCRANDOM_0_1()*1.5f;
	//fixtureDef.friction = .4;// CCRANDOM_0_1();
	//fixtureDef.restitution = 0.1;


#if 0
	b2CircleShape circle;
	circle.m_radius = t_radius * .5;
	circle.m_p = to_b2Vec2(pos);
	fixtureDef.shape = &circle;

	ret_body->CreateFixture(&fixtureDef);
#else
	const size_t t_countV = MAX(countV, 4);

#define b2_MSPV (b2_maxPolygonVertices - 1)
	int t_left = t_countV + 1;
	int size_end = (size_t)((float)t_left / (float)b2_MSPV)* b2_MSPV;
	int size_end2 = -1;
	if (size_end == 0)
		size_end = b2_MSPV;
	else if (size_end - t_left <= 3)
		size_end2 = 5;

	int t_vI = 0;
	b2Vec2* t_verts = new b2Vec2[t_countV];

	float coef = 2.0f * (float)M_PI / t_countV;

	for (unsigned int i = 0; i <= countV; i++) {
		float rads = i * coef;
		b2Vec2 t_ver = b2Vec2(pos.x, pos.y) + b2Vec2(t_radius * cosf(rads), t_radius * sinf(rads));
		//__________
		/*
		b2PolygonShape triangle;
		b2Vec2 triVerts[3];
		triVerts[0] = b2Vec2(pos.x, pos.y);
		triVerts[1] = b2Vec2(t_old.x, t_old.y);
		triVerts[2] = b2Vec2(t_p.x, t_p.y);
		triangle.Set(triVerts, 3);

		t_fd.shape = &triangle;
		ret_body->CreateFixture(&fixtureDef);

		t_old = t_p;
		*/
		//############################
		t_left--;
		t_verts[t_vI] = t_ver;
		t_vI++;
		if (t_vI == size_end || t_left <= size_end2) {
			if (t_left <= size_end2)size_end2 = -1;
			DataSpline::createFixture(t_verts, t_vI, t_fd, ret_body);
			t_vI = 0;

			t_verts[t_vI] = t_ver;
			t_vI++;
		}
	}
	if (t_vI > 0)
		DataSpline::createFixture(t_verts, t_vI, t_fd, ret_body);
	//############################
#endif
#endif
	return ret_body;
}

bool PhysicsSpline::changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState) {
	if (WorldSpline::changeKeys(keyCode, keyState))return true;
	bool ret = false;
	switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_R:
			if (keyState == KeyState_RELEASE) {
				m_b2dRecreate = true;
				ret = true;
			}
			break;
	}
	return ret;
}

//todo remove
DrawNode* t_drawNode = NULL;
Vec2 mouse_last;

PhysicsSpline::~PhysicsSpline() {
	if (t_drawNode) {
		t_drawNode->removeFromParent();
		t_drawNode = NULL;
	}

	if (m_b2world)delete m_b2world;
	m_b2world = NULL;
	m_mouseJoint = NULL;
	if (m_rect_sensor) {
		delete m_rect_sensor; m_rect_sensor = NULL;
		delete m_rect_sensor_go_up; m_rect_sensor_go_up = NULL;
		delete m_rect_; m_rect_ = NULL;
	}

}

void PhysicsSpline::update(float delta) {
	if (!t_drawNode)_layer->addChild(t_drawNode = DrawNode::create(), 90000);
	t_drawNode->clear();
#if HAS_DEVELOP
	if (DevelopMenu::Instance().isEnable()) {
		//__________

		for (vector<Vec2>::const_iterator it_b = m_tap_circle.begin(), it = it_b, it_e = m_tap_circle.end(); it != it_e; ++it) {
			Vec2 C = (*it) + mouse_last;
			Vec2 D;
			if (it + 1 == it_e)
				D = (*it_b) + mouse_last;
			else
				D = *(it + 1) + mouse_last;

			t_drawNode->drawLine(C, D, Color4F::RED);
		}

		t_drawNode->drawLine(*(m_tap_cross.begin()) + mouse_last, *(m_tap_cross.begin() + 1) + mouse_last, Color4F::RED);
		t_drawNode->drawLine(*(m_tap_cross.begin() + 2) + mouse_last, *(m_tap_cross.end() - 1) + mouse_last, Color4F::RED);
		//________________________


		//x:-90:26= -70+70
		//y:-50 50 = -30+60


		//t_drawNode->drawRect(_d_pos_rect, _d_pos_rect + _rect, Color4F::BLUE);
		if (false) {
			Vec2 d = mouse_last - (_curr_rect.origin + _curr_rect.size * .5);

			//d.x = d.x*.3;
			if (d.lengthSquared() < FLT_EPSILON * FLT_EPSILON) {
				//continue;
			}
			else {
				d.normalize();

				float F = -100.0f;
				t_drawNode->drawLine(mouse_last, mouse_last + d * F, Color4F::GREEN);

				Vec2 d2 = to_Vec2(d).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(90.));

				t_drawNode->drawLine(mouse_last, mouse_last +
					(d.y >= 0 ? Vec2(.0, 1.) : Vec2(.0, -1.)) * F
									 , Color4F::RED);
				//t_drawNode->drawLine(mouse_last, mouse_last + d2*F, Color4F::YELLOW);

				//t_drawNode->drawLine(mouse_last, mouse_last + ( d + d2 )*F, Color4F::RED);
			}
		}
		//________________________
	}
#endif
	//___________
	if (isChangedSpline() || m_b2dRecreate) {
		createPhysics(m_b2dRecreate);
		if (m_b2dRecreate) {
			m_tornado_bybbles.clear();
			m_fixturePairs.clear();
			t_tempBody.clear();
			m_mouseJoint = NULL;
		}

		m_b2dRecreate = false;
	}


	//__________________________

	for (vector<TempBody*>::iterator it = t_tempBody.begin(); it != t_tempBody.end();) {
		TempBody* tempBody = *(it);
		tempBody->m_delay -= delta;
		if (tempBody->m_delay < 0.) {
			m_b2world->DestroyBody(tempBody->m_body);
			if (it == t_tempBody.begin()) {
				it = t_tempBody.erase(it);
				continue;
			}
			else {
				it = t_tempBody.erase(it);
			}
		}
		++it;

		if (it == t_tempBody.end()) {
			break;
		}
	}
#if 0
	Bubble::Draw(_layer, m_Bubbles);

	for (Bubble* bubble : m_Bubbles) {
		bubble->step(delta);

		if (bubble->delay_spam > 0 && bubble->isEnd) {
			Vec2 pos = bubble->root->getPosition();

			float r = bubble->m_radius*.9;
			//CCLOG("__add start");
			for (int i = 0; i < 5; ++i) {
				float t_r = 1.3f + (3.f)*CCRANDOM_0_1();
				Vec2 n_p = Vec2(
					pos.x + r * CCRANDOM_MINUS1_1(),
					pos.y + r * CCRANDOM_MINUS1_1()
				);
				t_tempBody.push_back(new TempBody(addCircle(n_p, 5, t_r)));
				//CCLOG("__add");
			}
		}
	}
#endif

	float del_factor = 1.;
	if (m_mouseJoint) {
		Vec2 p1 = to_Vec2(m_mouseJoint->GetAnchorA());
		Vec2 p2 = to_Vec2(m_mouseJoint->GetAnchorB());
		float len = p1.distance(p2);
		if (len >= 20)
			del_factor = 3.;
		else if (len >= 5.)
			del_factor = 1. + len / 10.;

		velocityIterations = 12;
		positionIterations = 12;
	}
	else {
		velocityIterations = 8;
		positionIterations = 7;
	}

	//tested maybe
	//b2dStep(delta);
	b2dStep(del_factor);

	//if(t_drawNode){ t_drawNode->removeFromParent(); t_drawNode = NULL; }
}

class QueryCallback: public b2QueryCallback {
public:
	const PhysicsSpline* m_wphysics;
	QueryCallback(const PhysicsSpline* wphysics, const Vec2& point, bool isEditPhysics):
		m_wphysics(wphysics) {
		isEP = isEditPhysics;
		m_point = point;
		m_fixture = NULL;
	}

	bool hasIntersec(b2Fixture* fixture, const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D) {
		float S, T;

		if (Vec2::isLineIntersect(A, B, C, D, &S, &T) &&
			(S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f)) {

			// Vec2 of intersection

			m_intersec.x = A.x + S * (B.x - A.x);
			m_intersec.y = A.y + S * (B.y - A.y);
			m_intersec += m_point;

			m_fixture = fixture;
			// We are done, terminate the query.
			return true;
		}
		return false;
	}

	bool ReportFixture(b2Fixture* fixture) {
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody ||
			(isEP && IS_WATER(body))
			) {

			bool inside = fixture->TestPoint(to_b2Vec2(m_point));
			if (inside) {
				m_intersec = m_point;
				m_fixture = fixture;
				// We are done, terminate the query.
				return false;
			}
			/**/
			//_______________________
			///*
			b2Body* body = fixture->GetBody();
			const b2Transform& xf = body->GetTransform();
			for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
				if (b2Shape::Type::e_circle == fixture->GetType()) {
					b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

					//Vec2 t_pos = to_Vec2(circle->m_p); неа
					Vec2 t_pos = to_Vec2(body->GetPosition());
					if (m_point.distance(t_pos) <= (circle->m_radius + m_radius)) {
						m_intersec = t_pos;
						m_fixture = fixture;
						return false;
					}
				}
				else if (b2Shape::Type::e_polygon == fixture->GetType()) {
					b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
					Vec2 A, B;
					bool second = false;
					for (int32 i = 0; i < poly->m_count - 1; ++i) {
						if (second) {
							A = B;
						}
						else {
							second = true;
							A = to_Vec2(b2Mul(xf, poly->m_vertices[i])) - m_point;
						}
						B = to_Vec2(b2Mul(xf, poly->m_vertices[i + 1])) - m_point;

						if (hasIntersec(fixture, A, B,
										*(m_wphysics->m_tap_cross.begin()),
										*(m_wphysics->m_tap_cross.begin() + 1)) ||
							hasIntersec(fixture, A, B,
										*(m_wphysics->m_tap_cross.begin() + 2),
										*(m_wphysics->m_tap_cross.end() - 1)))
							return false;

						//__________
						for (vector<Vec2>::const_iterator it_b = m_wphysics->m_tap_circle.begin(), it = it_b, it_e = m_wphysics->m_tap_circle.end(); it != it_e; ++it) {
							Vec2 C = (*it);
							Vec2 D;
							if (it + 1 == it_e)
								D = (*it_b);
							else
								D = *(it + 1);
							//drawNode->drawLine(pos_e0, pos_e1, Color4F::RED);
							//_________________________________
							if (hasIntersec(fixture, A, B, C, D))
								return false;
						}
						//__________
					}
					//__________
				}
			}
		}
		// Continue the query.
		return true;
	}
	bool isEP;
	Vec2 m_point;
	b2Fixture* m_fixture;
	Vec2 m_intersec;
};

//todo remove
float t_lastDencity = .0f;

bool PhysicsSpline::mouseDown(const Vec2& mousePos, bool isCreated) {
	bool ret = WorldSpline::mouseDown(mousePos, !m_isEditPhysics);
	if (ret) {
		return true;
	}
	if (m_mouseJoint != NULL) {
		return true;
	}
	b2Vec2 p = b2Vec2(mousePos.x, mousePos.y);
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(1.f, 1.f);
	aabb.lowerBound = p - d;
	aabb.upperBound = p + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(this, mousePos, m_isEditPhysics);
	m_b2world->QueryAABB(&callback, aabb);

	if (callback.m_fixture) {
		b2Body* body = callback.m_fixture->GetBody();
		b2MouseJointDef md;
		md.bodyA = m_groundBody;
		md.bodyB = body;
		//md.target = p;
		md.target = to_b2Vec2(callback.m_intersec);
		md.maxForce = 100.*body->GetMass();//9 * 10e+10;//

		md.dampingRatio = 1.;
		//md.dampingRatio = mj_dampingRatio;// 1.0;
		//md.frequencyHz = mj_frequencyHz;// .1;

		m_mouseJoint = (b2MouseJoint*)m_b2world->CreateJoint(&md);
		body->SetAwake(true);
		m_selectedBody = body;

		t_lastDencity = m_selectedBody->GetFixtureList()->GetDensity();
		//m_selectedBody->GetFixtureList()->SetDensity(0.0001);
		if (m_isEditPhysics) {
			DataSpline* data_obj = TO_DATA_SPLINE(body->GetUserData());
			if (NULL != data_obj) {
				_isEditableSpline = data_obj->getParentSpline();
				if (NULL != _isEditableSpline) {
					_isAllocated = _isEditableSpline->getControl()[0];
					deselect();
					select();
					//data_obj->setCentr(Vec2(callback.m_point.x, callback.m_point.y));
				}
			}
		}

		return true;
	}
	//##############
	if (m_isEditPhysics)                       return true;

	//##############
	return false;
}

bool PhysicsSpline::mouseMovie(const Vec2& mousePos, Vec2* dPos) {
	mouse_last = mousePos;
	bool ret = WorldSpline::mouseMovie(mousePos, dPos);
	if (ret && m_isEditPhysics) {
		TSpline* parent = static_cast<TSpline*>(_isAllocated->getParent());
		//TSpline* parent = _isEditableSpline;
		if (NULL != parent) {
			for (Vertex* vertex : parent->getControl()) {
				if (vertex == _isAllocated)
					continue;
				vertex->ChangePos(*dPos);
			}
		}
		return true;
	}

	if (m_mouseJoint) {
		b2Vec2 p = b2Vec2(mousePos.x, mousePos.y);
		m_mouseJoint->SetTarget(p);
		if (!IS_WATER(m_selectedBody)) {
			if (NULL != m_selectedBody->GetUserData()) {
				DataSpline* t_data = TO_DATA_SPLINE(m_selectedBody->GetUserData());
				t_data->enableWater(false);
			}
		}
		return true;
	}

	if (m_isEditPhysics)                       return true;
	return	false;
}

bool PhysicsSpline::mouseUp(const Vec2& mousePos) {
	bool ret = WorldSpline::mouseUp(mousePos);
	//if (ret && m_isEditPhysics)return true;
	if (m_mouseJoint) {
		if (!IS_WATER(m_selectedBody)) {
			if (NULL != m_selectedBody->GetUserData()) {
				DataSpline* t_data = TO_DATA_SPLINE(m_selectedBody->GetUserData());
				t_data->enableWater(true);
			}
		}
		//m_selectedBody->GetFixtureList()->SetDensity(t_lastDencity);
		m_b2world->DestroyJoint(m_mouseJoint);
		m_mouseJoint = NULL;
		return true;
	}


	if (ret && !m_isEditPhysics || _splineChanged)//
		return true;
#if 0
	if (m_isCreateBubble) {
		addBubble(mousePos);
		return true;
}
#endif
	//addCircle(mousePos);
	return true;
}

void PhysicsSpline::draw() {

	//###########################
	m_b2world->DrawDebugData();
	//####################

	std::set<bodyPair>::iterator it = m_tornado_bybbles.begin();
	std::set<bodyPair>::iterator end = m_tornado_bybbles.end();
	b2Color c;

	while (it != end) {
		b2Vec2 pos = it->second->GetPosition();
		m_debugDraw->DrawPoint(pos, 4., b2Color(1., .0, .0));

		Vec2 lc = to_Vec2(it->second->GetLocalCenter());
		lc.rotate(Vec2::ZERO, it->second->GetAngle());
		m_debugDraw->DrawPoint(pos + to_b2Vec2(lc), 4., b2Color(.0, 1., .0));

		//b2MassData mass_data;
		//it->second->GetMassData(&mass_data);
		//m_debugDraw->DrawPoint(it->second->GetPosition() + mass_data.center, 2., b2Color(.0, .0, 1.0));
		it++;
	}
	//####################
	if (m_mouseJoint) {
		b2Vec2 p1 = m_mouseJoint->GetAnchorB();
		b2Vec2 p2 = m_mouseJoint->GetTarget();

		m_debugDraw->DrawPoint(p1, 4.0f, b2Color(.0f, 1.f, .0f));
		m_debugDraw->DrawPoint(p2, 4.0f, b2Color(.0f, 0.f, 1.f));

		m_debugDraw->DrawSegment(p1, p2, b2Color(0.8f, 0.8f, 0.8f));
	}

	//Bubble::Draw(_layer, m_Bubbles);

	//WorldSpline::draw(parent);
	for (TSpline* _tspline : _TSplines) {
		Vertex* v2 = NULL;
		Vertex* first = NULL;
#if 1
		for (Vertex* v1 : _tspline->getSpline()) {
			if (NULL == first)first = v1;
			v1->Draw(false, _isEdit || !m_isEditPhysics);
			v2 = v1;
		}
#endif
		//###################################
		v2 = NULL;
		first = NULL;
		int i = 0;
		for (Vertex* v1 : _tspline->getControl()) {
			i++;
			if (NULL == first)
				first = v1;
			v1->Draw(true, _isEdit);
			if (NULL != v2) {
				Vertex::Draw(v2, v1, false, false, _isEdit);
			}
			v2 = v1;
			//printText(_layer, v1, i, _recreateLabel);
#if 0
			DrawColor(Color4B::GREEN);
			glLineWidth(1);
			Vec2 dp = Vec2(dContainsR, dContainsR);
			Vec2 p1 = v1->getPos() - dp;
			Vec2 p2 = v1->getPos() + dp;
			ccDrawRect(p1, p2);
#endif
		}

		if (_tspline->IsClosed()) {
			Vertex::Draw(v2, first, false, false, _isEdit);
		}

		//Centr
		if (_isEdit) {
			DataSpline* data_obj = static_cast<DataSpline*>(_tspline->getData());
			if (data_obj != NULL) {
				Vec2 centr = data_obj->getWorldCenter();
				if (data_obj->getType() == b2_dynamicBody)
					DrawColor(Color4B::RED);
				else
					DrawColor(Color4B::GREEN);
				glLineWidth(1);
				ccDrawLine(Vec2(centr.x - 20, centr.y), Vec2(centr.x + 20, centr.y));
				ccDrawLine(Vec2(centr.x, centr.y - 20), Vec2(centr.x, centr.y + 20));
			}
		}
	}
	//#######e c

	if (!_TSplines.empty()) {
		_recreateLabel = false;
	}

}
