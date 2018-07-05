#pragma once
#include "stdafx.h"
#include "DataSpline.h"
#include "Objects/PhysicsObject.h"

void DataSpline::createShapes(std::vector<Vertex*> splines, b2Body* body, b2FixtureDef fd, const Vec2& centr){
	size_t t_vI = 0;
	b2Vec2 t_verts[b2_maxPolygonVertices];
	//t_verts[t_vI] = to_b2Vec2(p1);
	//t_vI++;
	//Vec2 centr =  getCentr();
	//for( Vertex* spline in splines ) {
	for(std::vector<Vertex*>::iterator it = splines.begin(), it_e = splines.end();; ++it){
		if(t_vI == 0){
			t_verts[t_vI] = to_b2Vec2(centr);
			t_vI++;
		}

		bool t_end = it == it_e;// NULL == ( *it )->getHead();
		if(!t_end){
			Vec2 p1 = ( *it )->getPos();
			t_verts[t_vI] = to_b2Vec2(p1);
			t_vI++;
		}
		//#############################
		if(t_end || t_vI == b2_maxPolygonVertices - 1){
			b2PolygonShape shape;
			shape.Set(t_verts, t_vI);
			fd.shape = &shape;
			body->CreateFixture(&fd);

			t_vI = 0;
		}
		//#############################
		if(t_end)break;

		//if (NULL == spline->getHead())break;;
		//Vec2 p2 = spline->getHead()->getPos();
		//t_verts[t_vI] = to_b2Vec2(p2);
		//t_vI++;


	}
}

b2Fixture* DataSpline::createFixture(b2Vec2* t_verts, size_t t_vI, b2FixtureDef fd, b2Body* body){
	b2PolygonShape shape;
	shape.Set(t_verts, t_vI);
	fd.shape = &shape;
	return body->CreateFixture(&fd);
}

std::vector<std::vector<b2Vec2>> DataSpline::createFixtres(const b2Vec2& centr, const std::vector<Vertex*>& splines, const Vec2& d_pos){
	std::vector<std::vector<b2Vec2>> ret;

	int i_fixture = 0;
	for(std::vector<Vertex*>::const_iterator it = splines.begin(), it_e = splines.end(); it != it_e; ++it){
		if(0 == ret.size()){
			ret.push_back(std::vector<b2Vec2>());
		}
		if(ret[i_fixture].size() == 0){
			ret[i_fixture].push_back(centr);
		}
		ret[i_fixture].push_back(to_b2Vec2(( *it )->getPos() + d_pos));

		//todo if min polygons or min line;
		if(ret[i_fixture].size() == b2_maxPolygonVertices - 1){
			//if(ret[i_fixture].size() == 3){
			i_fixture++;
			if(it + 1 == it_e){
				i_fixture--;
			}
			else{
				ret.push_back(std::vector<b2Vec2>());
				ret[i_fixture].push_back(centr);
				ret[i_fixture].push_back(*( ret[i_fixture - 1].end() - 1 ));
			}
		}
	}

	ret[i_fixture].push_back(*( ret[0].begin() + 1 ));

	if(ret[i_fixture].size() <= 2){
		std::vector<b2Vec2>::const_iterator tmp = ( ret[i_fixture - 1].end() - 1 );
		ret[i_fixture].insert(ret[i_fixture].begin() + 1, *tmp);
		ret[i_fixture - 1].erase(tmp);
	}

	return ret;
}

void DataSpline::createWhell(float radius){
#define NUM_SEGMENTS 12

	b2Vec2 center = b2Vec2(.0, .0);
	b2CircleShape circleShape;
	circleShape.m_radius = 1.25f;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = 0.1f;
	fixtureDef.restitution = 0.05f;
	fixtureDef.friction = 1.0f;

	// The greater the number, the more springy
	float springiness = 4.0f;

	// Delta angle to step by
	float deltaAngle = ( 2.f * M_PI ) / NUM_SEGMENTS;

	// Radius of the wheel
	//float radius = 15;

	// Need to store the bodies so that we can refer back
	// to it when we connect the joints
	vector<b2Body*>bodies;

	// For each segment...
	for(int i = 0; i < NUM_SEGMENTS; i++){
		// Current angle
		float theta = deltaAngle*i;

		// Calcualte the x and y based on theta 
		float x = radius*cosf(theta);
		float y = radius*sinf(theta);

		// Remember to divide by PTM_RATIO to convert to Box2d coordinates
		b2Vec2 circlePosition = b2Vec2(x, y);

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		// Position should be relative to the center
		bodyDef.position = ( center + circlePosition );

		// Create the body and fixture
		b2Body *body;
		body = m_b2world->CreateBody(&bodyDef);
		body->CreateFixture(&fixtureDef);

		// Add the body to the array to connect joints to it
		// later. b2Body is a C++ object, so must wrap it
		// in NSValue when inserting into it NSMutableArray

		bodies.push_back(body);
	}

	// Circle at the center (inner circle)
	b2BodyDef innerCircleBodyDef;
	innerCircleBodyDef.type = b2_dynamicBody;
	// Position is at the center
	innerCircleBodyDef.position = center;
	b2Body *innerCircleBody = m_b2world->CreateBody(&innerCircleBodyDef);
	innerCircleBody->CreateFixture(&fixtureDef);

	// Connect the joints    
	b2DistanceJointDef jointDef;
	for(int i = 0; i < NUM_SEGMENTS; i++){
		// The neighbor.
		int neighborIndex = ( i + 1 ) % NUM_SEGMENTS;

		// Get the current body and the neighbor
		b2Body *currentBody = bodies.at(i);
		b2Body *neighborBody = bodies.at(neighborIndex);

		// Connect the outer circles to each other
		jointDef.Initialize(currentBody, neighborBody,
							currentBody->GetWorldCenter(),
							neighborBody->GetWorldCenter());
		jointDef.collideConnected = true;
		jointDef.frequencyHz = springiness;
		jointDef.dampingRatio = 0.5f;

		m_b2world->CreateJoint(&jointDef);

		// Connect the center circle with other circles        
		jointDef.Initialize(currentBody, innerCircleBody, currentBody->GetWorldCenter(), center);
		jointDef.collideConnected = true;
		jointDef.frequencyHz = springiness;
		jointDef.dampingRatio = 0.5;

		m_b2world->CreateJoint(&jointDef);
	}
}


void DataSpline::create_soft_body(std::vector<Vertex*> splines){
	m_sectors.clear();

	//createWhell(15.);
	//after createFixture
	b2CircleShape circleShape;
	circleShape.m_radius = m_radiys_j;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &circleShape;
	fixtureDef.density = 0.01f;
	fixtureDef.restitution = 0.05f;
	fixtureDef.friction = 1.0f;

	Vec2 centr = m_pos + getLocalCenter() + d_c;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.gravityScale = m_gscale;

	b2Vec2 center = to_b2Vec2(centr);
	//____
	// Circle at the center (inner внутренний circle)
	b2BodyDef innerBodyDef = bodyDef;
	innerBodyDef.position = center;
	innerBody = m_b2world->CreateBody(&innerBodyDef);
	innerBody->CreateFixture(&fixtureDef);
	//_______
	//TODO to test
	if(m_body)destroy_physics();
	m_body = innerBody;
	//m_body->SetMassData(&m_massData);
	//________
	m_count_w = ( m_count_w == 0 ? 1 : m_count_w );
	m_count_h = ( m_count_h == 0 ? 1 : m_count_h );
	vector<Vec2> t_top;
	//create круга по узловым точкам
	for(std::vector<Vertex*>::const_iterator it = splines.begin(), it_e = splines.end(); it != it_e; ++it){
		//____
		Vec2 t1 = t_top.empty() ? ( *it )->getPos() : *( t_top.end() - 1 );
		Vec2 t2 = ( *( ( ( it + 1 ) == it_e ) ? splines.begin() : it + 1 ) )->getPos();

		Vec2 ts1 = t1;
		Vec2 ts2 = t2;

		Vec2 t_ni = t2 - t1;
		Vec2 d_i = t_ni.getNormalized()* ( t_ni.getLength() / float(m_count_w) );
		t2 = t1;

		//______________
#define CREATE_C_J(pos){\
		bodyDef.position = to_b2Vec2(pos);\
		b2Body *body;\
		body = m_b2world->CreateBody(&bodyDef);\
		body->CreateFixture(&fixtureDef);\
		curr_sector.push_back(body);}
		//______________
		if(t_top.empty())t_top.push_back(t1);
		//CREATE_C_J(t1);
		//CREATE_C_J(t2);
		for(int i = 0; i < m_count_w; ++i){//+1 from t1 -> t2
			t_sector curr_sector;
			Vec2 t_nj = centr - t2;
			Vec2 d_j = t_nj.getNormalized()* ( ( t_nj.getLength()*m_sc_r ) / float(m_count_h) );

			Vec2 b = t2;
			CREATE_C_J(b);
			for(int j = 0; j < m_count_h; ++j){
				b += d_j;
				CREATE_C_J(b);
			}
			t2 += d_i;
			t_top.push_back(t2);
			m_sectors.push_back(curr_sector);
		}
	}

	//#########################

	//______________________
	// Connect the joints    
	b2DistanceJointDef jointDef;
	uint count = m_sectors.size();
	if(count == 0)return;

	b2Body *tb_p = NULL;
	b2Body *bb_p = NULL;

	//b2Body *tb_1, *tb_2;
	//b2Body *bb_1, *bb_2;
	vector<t_sector>::iterator it_b = m_sectors.begin();
	static int d_hf = 11;
	int has_facke_inner = ( splines.size() - 1 - d_hf ) * m_count_w; m_body = NULL;
	for(std::vector<t_sector>::iterator it = it_b, it_e = m_sectors.end(); it != it_e; ++it){
		t_sector s1 = *( it );
		t_sector s2 = ( it + 1 == it_e ) ? *it_b : *( it + 1 );

		t_sector::iterator it1 = s1.begin();
		t_sector::iterator it_e1 = s1.end();
		t_sector::iterator it2 = s2.begin();
		b2Body* t1 = NULL;
		b2Body* t2 = NULL;

		if(!m_body && has_facke_inner-- == 0){
			m_body = *it1;
		}

		while(it1 != it_e1){
			bool has_last = it1 + 1 == it_e1;

			if(t1 == NULL){
				t1 = *it1;
				t2 = *it2;
			}
			else{
				b2Body* b1 = *it1;
				b2Body* b2 = *it2;
				//________________________
#define CREATE_JOIN(b1,b2){\
		jointDef.Initialize(b1,b2,b1->GetWorldCenter(),b2->GetWorldCenter());\
		jointDef.collideConnected = true;\
		jointDef.frequencyHz = springiness;\
		jointDef.dampingRatio = damp_rat;\
		m_b2world->CreateJoint(&jointDef);}

				CREATE_JOIN(t1, t2);
				CREATE_JOIN(t1, b1);
				if(has_last){
					CREATE_JOIN(b1, b2);
					//CREATE_JOIN(bb_1, tb_1);

					CREATE_JOIN(b1, innerBody);
				}

				CREATE_JOIN(t1, b2);
				CREATE_JOIN(t2, b1);
				//_______________________
				//_________________
				t1 = b1;
				t2 = b2;
			}
			++it1; ++it2;
		}
	}
	//
	if(!m_body)m_body = innerBody;
	return;
}

void DataSpline::createFixture(std::vector<Vertex*> splines){
	if(!m_body) return;
	for(b2Fixture* fixture = m_body->GetFixtureList(); fixture != NULL; fixture = m_body->GetFixtureList()){
		m_body->DestroyFixture(fixture);
	}
	b2FixtureDef fd;
	fd.density = m_dencity;
	fd.restitution = m_restriction;
	//____________
	fd.filter = m_filter;
	//____________
	bool t_createFixture = false;

	switch(m_type){
		case DSType_EndBox:
            m_body->SetType(b2_staticBody);
            fd.isSensor = hasSensor();
            t_createFixture = true;
            break;
			//return;
		case DSType_Stataic:
			{
				fd.density = .0f;
				m_body->SetType(b2_staticBody);
				//createShapes(splines, fd);
				b2EdgeShape edge;
                for( Vertex* spline : splines ){
					if(NULL == spline->getHead())continue;
					Vec2 p1 = spline->getPos();
					Vec2 p2 = spline->getHead()->getPos();

					edge.Set(b2Vec2(p1.x, p1.y), b2Vec2(p2.x, p2.y));
					fd.shape = &edge;

					m_body->CreateFixture(&fd);
				}
			}break;
		case DSType_Bubble:
			//create_soft_body(splines); return;
		case DSType_Dynamic:
			{
				if(splines.size() <= 3)return;

				fd.friction = .4f;//.2f;
								  //fd.restitution = .1f;// 0.f;
				fd.isSensor = hasSensor();
				m_body->SetType(b2_dynamicBody);

				t_createFixture = true;
			}
			break;
		case DSType_Water:
			{
				fd.isSensor = hasSensor();
				m_body->SetType(b2_staticBody);
				t_createFixture = true;
			}
			break;
	}

	if(t_createFixture){
		setDataInWorld();

		size_t t_vI = 0;
		b2Vec2 t_verts[b2_maxPolygonVertices];

		//createfixture in o;o coord and after set pos and mass centr
		Vec2 centr = getLocalCenter();
		b2Vec2 b2Centr = to_b2Vec2(centr);

		std::vector<std::vector<b2Vec2>> t_fixtures = createFixtres(b2Centr, splines, -getSPos());
		for(auto it_fixture : t_fixtures){
			for(auto pos : it_fixture){
				t_verts[t_vI++] = pos;
			}
			b2Fixture* t_fixture = createFixture(t_verts, t_vI, fd, m_body);
			t_vI = 0;
			//resetDC();
		}

		//not set centr in mass data from b2PolygonShape
		//bag box2d
		//m_body->SetMassData(&m_massData);
	}
}
