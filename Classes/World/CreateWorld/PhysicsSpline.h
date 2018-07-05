#pragma once
#include "stdafx.h"
#include "WorldSpline/WorldSpline_and_Physics.h"
#include "DataSpline.h"
#include <set>
#include <vector>
#include "World/World.h"
#include "Objects/Soul.h"
#include "Objects/PhysicsObject.h"

using namespace std;

#if !defined(CC_ENABLE_BOX2D_INTEGRATION)
needed enable box2d
#endif


#include "Box2D/Box2D.h"
#include "Box2d/GLES-Render.h"

#include "Box2D/boyancy/iforce2d_Buoyancy_functions.h"
using namespace b2dHelper;

class Line;

extern int velocityIterations;
extern int positionIterations;
extern float timeStep;
extern b2World* m_b2world;
extern bool m_b2dPlay;
extern Rect _curr_rect;

class PhysicsSpline: public WorldSpline_and_Physics, b2ContactListener {
public:
	vector<Vec2> m_tap_circle;//this contain tap cross 
	vector<Vec2> m_tap_cross;


	Rect _rect_small;
	Rect _rect_big;
private:
	struct TempBody {
		b2Body* m_body = NULL;
		float m_delay = 5.f;
		TempBody(b2Body* body):m_body(body) {}
	};
	vector<TempBody*> t_tempBody;
#if HAS_DEVELOP
	LayoutRefresh* m_editCreateB;
	LayoutRefresh* m_editEditPhys;
	bool m_isCreateBubble = false;
	void setCreateBubble(bool enable);
#endif

	b2Body* addCircle(const Vec2& pos, int countV = 8, float radius = -1, float dencity = -1);
#if HAS_DEVELOP
	virtual void setEditPhysics(const bool& enable);
#endif

	//---------------------------------
	Vec2 m_gravity;
	b2Body* m_selectedBody;

	b2Body* m_groundBody;

	size_t mFactorStep;
	size_t mFactorStep_2;
	size_t mFactorMass;
	//---------------------------
	World* _world;
	Node* _layer;
	void createBox2d();
	virtual void createPhysics(bool recreate = false);

public:
	PhysicsSpline(Node* layer, Rect rect_l, Rect rect_r,float dlh, World* world);
	virtual ~PhysicsSpline();

	virtual bool changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState);

	virtual bool mouseDown(const Vec2& mousePos, bool isCreated = true);
	virtual bool mouseMovie(const Vec2& mousePos, Vec2* dPos = NULL);
	virtual bool mouseUp(const Vec2& mousePos);

	virtual void draw();
	virtual void update(float delta);
	enum Type_World { TW_Big, TW_Small };
	void load(Type_World type_world);

	float dragMod = 0.25f;//adjust as desired
	float liftMod = 0.25f;//adjust as desired
	float maxDrag = 2000.f;//adjust as desired
	float maxLift = 500.f;//adjust as desired


	void b2dStep(float factor) {
		if (!m_b2dPlay)return;
		//_______________________________________________
		m_b2world->Step(
			//delay
			//(delay) *10.
			timeStep*10. * factor
			, velocityIterations, positionIterations);
		//m_b2world->Step(( delay*timeStep )*1000., velocityIterations, positionIterations);

		//##########################
		{
			std::set<bodyPair>::iterator it = m_check_end_bybbles.begin();
			std::set<bodyPair>::iterator end = m_check_end_bybbles.end();
			while (it != end) {
				b2Body* bybble = it->second;

				DataSpline* data_bybble = TO_DATA_SPLINE(bybble->GetUserData());
				PhysicsObject* bybble_obj = data_bybble->getParent();
				if (!bybble_obj->hasSelect() || bybble_obj->hasTemp()) { ++it; continue; }

				b2Body* end_box = it->first;
				DataSpline* data_end_box = TO_DATA_SPLINE(end_box->GetUserData());

				PhysicsObject* end_box_obj = data_end_box->getParent();
				PhysicsObject* let_obj = end_box_obj->getLetObj();
				//end_box_obj->toEnd(let_obj == bybble_obj);

				if (!end_box_obj->hasFinish()) {
					bool succes = let_obj == bybble_obj || 0 == let_obj->getLet().compare(bybble_obj->getLet());
					//if (succes)
					bybble_obj->toEnd(end_box_obj, succes);
				}
				++it;
			}
		}
		//##########################
		//if (false) 
		{
			std::set<b2Body*>::iterator it = m_go_up_bybbles.begin();
			std::set<b2Body*>::iterator end = m_go_up_bybbles.end();
			while (it != end) {
				b2Body* bybble = *it;
				b2Vec2 position = bybble->GetPosition();
				float fy = bybble->GetMass() * .5;
				b2Vec2 F = b2Vec2(.0, fy);
				bybble->ApplyForce(F, position, false);

				//todo удалить просчёт ускорения - поставить флаг на только что созданный
				if (to_Vec2(bybble->GetLinearVelocity()).getLength() <= .5f) {
					float force_y = bybble->GetMass()*RAND_F(1, 2);
					Vec2 t_force = Vec2(.0f, bybble->GetMass()*RAND_F(100, 200));
					t_force.rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(RAND_F(80, 110)));
					bybble->ApplyForceToCenter(to_b2Vec2(t_force), true);
					bybble->ApplyForce(b2Vec2(.0f, force_y), b2Vec2(RAND_F(-1, +1), RAND_F(-1, +1)), true);
				}
				++it;
			}
		}
		//########################
		//if(false) 
		{
			std::set<bodyPair>::iterator it = m_tornado_bybbles.begin();
			std::set<bodyPair>::iterator end = m_tornado_bybbles.end();
			while (it != end) {
				//b2Body* rect = it->first;

				b2Body* bybble = it->second;

				DataSpline* data_bybble = TO_DATA_SPLINE(bybble->GetUserData());
				if (data_bybble->getParent()->hasSelect()) { ++it; continue; }
				{
					b2Vec2 position = bybble->GetPosition();

					b2Vec2 d = position - to_b2Vec2(_curr_rect.origin + _curr_rect.size * .5);
					//d.x = d.x*.3;
					if (d.LengthSquared() < FLT_EPSILON * FLT_EPSILON) {
						//continue;
					}
					else {
						d.Normalize();


						DataSpline* data = TO_DATA_SPLINE(bybble->GetUserData());
						int i = data == NULL ? 1 : data->m_di;
						//b2Vec2 F = ( bybble->GetMass()  * -2.0f) * d ;
						Vec2 d2 = to_Vec2(d).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(90.f*i));

						b2Vec2 F2 = to_b2Vec2(bybble->GetMass() * d2 * -.05f);

						b2Vec2 F = (bybble->GetMass()*-.08f)*
							(d.y >= .3f ? b2Vec2(.0f, 1.3f) :
							 d.y <= -.3f ?
							 b2Vec2(.0f, -1.f) :
							 b2Vec2(0.f, 0.f));
						//( d + to_b2Vec2(d2) );

						//extern DrawNode* t_drawNode;
#define ACTION(F){\
						bybble->ApplyForce(F, position, false);\
                        bybble->ApplyForce(F, position + to_b2Vec2(d2), false);\
						Vec2 p = to_Vec2(position);\
						Vec2 pF = to_Vec2(1.*F);}
						//t_drawNode->drawLine(p, p + pF, Color4F::BLUE);\
						t_drawNode->drawLine(p + d2, p + d2 + pF, Color4F::BLUE);\
						}

						ACTION(F);
						ACTION(F2);
						//ACTION(-.2f*bybble->GetMass()*d);

						//bybble->ApplyForce(F2, position, false);
					}
				}
				++it;
			}
		}
		//________________
#if 0
		//this water
		//go through all buoyancy fixture pairs and apply necessary forces
		std::set<fixturePair>::iterator it = m_fixturePairs.begin();
		std::set<fixturePair>::iterator end = m_fixturePairs.end();
		while (it != end) {

			//fixtureA is the fluid
			b2Fixture* fixtureA = it->first;
			b2Fixture* fixtureB = it->second;

			float tmp_scale = 1.f;
			if (IS_DISABLEW(fixtureB->GetBody())) {
				//++it;
				//continue;
				//tmp_scale = .5;
			}

			float density = fixtureA->GetDensity();

			std::vector<b2Vec2> intersectionPoints;
			if (findIntersectionOfFixtures(fixtureA, fixtureB, intersectionPoints)) {
				//find centroid
				float area = 0.f;
				b2Vec2 centroid = ComputeCentroid(intersectionPoints, area);

				//apply buoyancy force
				float displacedMass = fixtureA->GetDensity() * area;
				b2Vec2 gravity(m_gravity.x, 10 * m_gravity.y * tmp_scale);
				b2Vec2 buoyancyForce = displacedMass * -gravity;
				fixtureB->GetBody()->ApplyForce(buoyancyForce, centroid, true);

				/*
				//simple drag
				//find relative velocity between object and fluid
				b2Vec2 velDir = fixtureB->GetBody()->GetLinearVelocityFromWorldPoint( centroid ) -
				fixtureA->GetBody()->GetLinearVelocityFromWorldPoint( centroid );
				float vel = velDir.Normalize();

				float dragMod = 1;//adjust as desired
				float dragMag = fixtureA->GetDensity() * vel * vel;
				b2Vec2 dragForce = dragMod * dragMag * -velDir;
				fixtureB->GetBody()->ApplyForce( dragForce, centroid );
				float angularDrag = area * -fixtureB->GetBody()->GetAngularVelocity();
				fixtureB->GetBody()->ApplyTorque( angularDrag );
				/**/

				//apply complex drag
				for (uint i = 0; i < intersectionPoints.size(); i++) {
					b2Vec2 v0 = intersectionPoints[i];
					b2Vec2 v1 = intersectionPoints[(i + 1) % intersectionPoints.size()];
					b2Vec2 midPoint = 0.5f * (v0 + v1);

					//find relative velocity between object and fluid at edge midpoint
					b2Vec2 velDir = fixtureB->GetBody()->GetLinearVelocityFromWorldPoint(midPoint) -
						fixtureA->GetBody()->GetLinearVelocityFromWorldPoint(midPoint);
					float vel = velDir.Normalize();

					b2Vec2 edge = v1 - v0;
					float edgeLength = edge.Normalize();
					b2Vec2 normal = b2Cross(-1, edge);
					float dragDot = b2Dot(normal, velDir);
					if (dragDot < 0)
						continue;//normal points backwards - this is not a leading edge

								 //apply drag
					float dragMag = dragDot * dragMod * edgeLength * density * vel * vel;
					dragMag = b2Min(dragMag, maxDrag);
					b2Vec2 dragForce = dragMag * -velDir;
					fixtureB->GetBody()->ApplyForce(dragForce, midPoint, true);

					//apply lift
					float liftDot = b2Dot(edge, velDir);
					float liftMag = dragDot * liftDot * liftMod * edgeLength * density * vel * vel;
					liftMag = b2Min(liftMag, maxLift);
					b2Vec2 liftDir = b2Cross(1, velDir);
					b2Vec2 liftForce = liftMag * liftDir;
					fixtureB->GetBody()->ApplyForce(liftForce, midPoint, true);
				}

				/*
				//draw debug info
				glColor3f(0, 1, 1);
				glLineWidth(2);
				glBegin(GL_LINE_LOOP);
				for(int i = 0; i < intersectionPoints.size(); i++)
					glVertex2f(intersectionPoints[i].x, intersectionPoints[i].y);
				glEnd();
				glLineWidth(1);

				//line showing buoyancy force
				if(area > 0){
					glBegin(GL_LINES);
					glVertex2f(centroid.x, centroid.y);
					glVertex2f(centroid.x, centroid.y + area);
					glEnd();
				}
				/**/
			}

			++it;
		}
#endif
	}

	// Callbacks for derived classes.
	std::set<fixturePair> m_fixturePairs;

	typedef std::pair<b2Body*, b2Body*> bodyPair;
	std::set<bodyPair> m_tornado_bybbles;
	std::set<bodyPair> m_check_end_bybbles;
	std::set<b2Body*> m_go_up_bybbles;

	virtual void BeginContact(b2Contact* contact) {
		Contact(true, contact);
	}

	virtual void EndContact(b2Contact* contact) {
		Contact(false, contact);
	}


	bool actionBodys(bool begin, b2Body* bodyA, b2Body* bodyB) {
		DataSpline* dataA = TO_DATA_SPLINE(bodyA->GetUserData());
		DataSpline* dataB = TO_DATA_SPLINE(bodyB->GetUserData());

		if (NULL == dataA || NULL == dataB) return false;

		if (DSType_Water == dataA->getType() && dataA->test_check > 0 &&
			DSType_Bubble == dataB->getType()) {

			if (dataA->test_check == 1) {
				if (begin) m_tornado_bybbles.insert(bodyPair(bodyA, bodyB));
				else m_tornado_bybbles.erase(make_pair(bodyA, bodyB));
			}
			else if (dataA->test_check == 2) {
				if (begin) m_go_up_bybbles.insert(bodyB);
				else m_go_up_bybbles.erase(bodyB);
			}
			return true;
		}
		//_________
		if (DSType_EndBox == dataA->getType() && DSType_Bubble == dataB->getType()) {
			if (begin) m_check_end_bybbles.insert(bodyPair(bodyA, bodyB));
			else m_check_end_bybbles.erase(make_pair(bodyA, bodyB));
			return true;
		}
		else if (DSType_Bubble == dataA->getType() && DSType_EndBox == dataB->getType()) {
			if (begin) m_check_end_bybbles.insert(bodyPair(bodyB, bodyA));
			else m_check_end_bybbles.erase(make_pair(bodyB, bodyA));
			return true;
		}

		return true;
	}

	void Contact(bool begin, b2Contact* contact) {
		b2Body* bodyA = contact->GetFixtureA()->GetBody();
		b2Body* bodyB = contact->GetFixtureB()->GetBody();
		if (!actionBodys(begin, bodyA, bodyB))
			actionBodys(begin, bodyB, bodyA);
	}

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

	}

	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}
};
