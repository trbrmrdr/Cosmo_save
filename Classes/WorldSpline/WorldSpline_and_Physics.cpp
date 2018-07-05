#include "stdafx.h"
#include "WorldSpline_and_Physics.h"
#include "DevelopMenu.h"


WorldSpline_and_Physics::WorldSpline_and_Physics(int max_count_spline, DevelopTabs _tabs, bool createEdits) :
	WorldSpline(max_count_spline, _tabs, false),
	m_isEditPhysics(false) {
#if HAS_DEVELOP
	changeEdits(createEdits);
#endif
}

void WorldSpline_and_Physics::loadChild(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos /*= Vec2::ZERO*/, const float& scale /*= 1.f*/) {
	DataSpline* data_obj = new DataSpline(spline, dictInSplines, d_pos, scale);
	spline->setData(data_obj);
}

void WorldSpline_and_Physics::saveChild(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos /*= Vec2::ZERO*/, const float& scale /*= 1.f*/) {
	DataSpline* data_obj = static_cast<DataSpline*>(spline->getData());
	if (NULL == data_obj)return;
	data_obj->save(dictInSplines, scale, d_pos);
}

#if HAS_DEVELOP

void WorldSpline_and_Physics::changeEdits(bool create) {
	WorldSpline::changeEdits(create);

	if (create && NULL == m_editEditPhys) {
		m_editEditPhys = DevelopMenu::Instance().addBoolSetting(this, 0, _tabs, "Edit physics",
			CC_CALLBACK_1(WorldSpline_and_Physics::setEditPhysics, this),
			([this](void) { return m_isEditPhysics; }), SeparatorType_Bottom);
	}
	if (!create && NULL != m_editEditPhys) {
		m_editEditPhys = NULL;
	}
}

void WorldSpline_and_Physics::setEditPhysics(const bool& enable) {
	m_isEditPhysics = enable;
	if (m_isEditPhysics) {
		stopCreate(true);
		m_editCreateSpline->Refresh();
	}
	else {
		DevelopMenu::Instance().eraceSettings(this, 2);
	}
}

void WorldSpline_and_Physics::select(bool force) {
	if (!m_isEditPhysics) {
		WorldSpline::select(force);
		return;
	}

	if (!force) {
		if (_isAllocated_prev == _isAllocated)
			return;
		deselect();
		if (_isAllocated == NULL)
			return;
		_isAllocated_prev = _isAllocated;
	}

	//################

	DevelopMenu::Instance().addFloatSetting(this, 1, _tabs, "setGScale",
		([this](const float& val) {
		TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setGscale(val);
		setChangedSpline(true);
	}),
			([this](void) {
		DataSpline* t_data = TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData());
		if (NULL != t_data)return t_data->getGScale();
		return -1.f;
	}), SeparatorType_None, .9f);

	//################

	DevelopMenu::Instance().addPointSetting(this, 1, _tabs, "setLocalCentr",
		([this](const Vec2& pos) {
		TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setLocalCentr(pos);
		setChangedSpline(true);

	}),
			([this](void) {
		return TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->getLocalCenter();

	}), SeparatorType_None, .9f, .9f);

	DevelopMenu::Instance().addFloatSetting(this, 1, _tabs, "setMass",
		([this](const float& val) {
		TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setMass(val);
		setChangedSpline(true);
	}),
			([this](void) {
		return TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->getMassData().mass;

	}), SeparatorType_None, .9f);

	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 1, _tabs, "setDencity",
		([this](const float& val) {
		TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setDencity(val);
		//setChangedSpline(true);
	}),
			([this](void) {
		return TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->getDencity();

	}), .01f, 100.f, SeparatorType_None, .05f);
	//___________________________
	DevelopMenu::Instance().addFloatSettingFromMinMax(this, 1, _tabs, "setRestriction",
		([this](const float& val) {
		TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setRestriction(val);
		//setChangedSpline(true);
	}),
			([this](void) {
		return TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->getRestriction();

	}), .0f, 1.f, SeparatorType_None, .05f);
	//___________________________

	//ToDo
	const std::vector<std::string> namesType{ "Static","Dynamic","Water" };

	DevelopMenu::Instance().addSelectItem(this, 1, _tabs, "Object type", namesType,
		([this](const size_t& item) {
		if (item == 0)
			TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setType(DataSplineType::DSType_Stataic);
		else if (item == 1)
			TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setType(DataSplineType::DSType_Dynamic);
		else if (item == 2)
			TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->setType(DataSplineType::DSType_Water);
		setChangedSpline(true);
	}),
			([this](void) {
		DataSplineType type = TO_DATA_SPLINE(TO_SPLINE(_isAllocated->getParent())->getData())->getType();
		if (type == DataSplineType::DSType_Stataic)
			return 0;
		else if (type == DataSplineType::DSType_Dynamic)
			return 1;
		else if (type == DataSplineType::DSType_Water)
			return 2;
	})
		, SeparatorType_Bottom
		);
}
#endif

void WorldSpline_and_Physics::createPhysics(bool recreate) {
	CreateFixture(getPolygons(), recreate);
}

void WorldSpline_and_Physics::CreateFixture(const std::vector<TSpline*>& splines, bool recreate) {
	for (TSpline* t_spline : splines) {
		CreateFixture(t_spline, recreate);
	}
}

DataSpline* WorldSpline_and_Physics::GetData(TSpline* spline) {
	DataSpline* data_obj = TO_DATA_SPLINE(spline->getData());
	if (NULL == data_obj) {
		data_obj = new DataSpline(spline);
		spline->setData(data_obj);
	}
	return data_obj;
}

void WorldSpline_and_Physics::CreateFixture(TSpline* spline, bool recreate) {
	DataSpline* data_obj = GetData(spline);
	if (recreate) data_obj->createBody();
	data_obj->createFixture(spline->getSpline());
}

void WorldSpline_and_Physics::draw() {
	if (true) {
		Vec2 t_g = Vec2(m_b2world->GetGravity().x, m_b2world->GetGravity().y);

		//for (b2Body* b = m_b2world->GetBodyList(); b; b = b->GetNext()) {
		for (TSpline* spline : _TSplines) {
			if (NULL == spline->getData())continue;
			DataSpline* data_obj = static_cast<DataSpline*>(spline->getData());
			//if (data_obj->getType() != b2_dynamicBody)continue;
			b2Body* b = data_obj->getBody();
			if (b == NULL)continue;


			b2Transform xf = b->GetTransform();
			//xf.p = b->GetWorldCenter();
			//xf.p = b->GetLocalCenter();
			xf.p = b->GetPosition();

			float angle = b->GetAngle();
			b2Vec2 t_v = b->GetLinearVelocity();
			//float angV = b->GetAngularVelocity();
			float t_gs = b->GetGravityScale();
			float t_i = b->GetInertia();
			//b2Log("  bd.linearDamping = %.15lef;\n", m_linearDamping);
			//b2Log("  bd.angularDamping = %.15lef;\n", m_angularDamping);
			//b2Log("  bd.gravityScale = %.15lef;\n", m_gravityScale);

			{
				float scale = 10;
				Vec2 dt = Vec2(2, 2);
				//dt.rotate(Vec2::ZERO, angle);
				const Vec2 c = Vec2(xf.p.x, xf.p.y);
				//________v
				Vec2 v = Vec2(t_v.x, t_v.y);
				DrawColor(Color4B::GREEN);
				glLineWidth(2);
				ccDrawLine(c, c + v * scale);
#if 0
				//_____i
				Vec2 i(t_i, t_i);
				i.rotate(Vec2::ZERO, angle);
				i = c + i;
				DrawColor(Color4B::MAGENTA);
				ccDrawLine(c + dt, dt + v * scale);
#endif
				//_____g
				glLineWidth(4 * scale);
				DrawColor(Color4B::WHITE);
				ccDrawLine(c + dt, c + dt + t_g * scale);
				//continue;
				Vec2 l_g = t_g * t_gs;
				glLineWidth(1 * scale);
				DrawColor(Color4B::BLACK);
				ccDrawLine(c + dt, c + dt + l_g * scale);
			}

			/*
			b2Vec2 p1 = xf.p, p2;
			const float32 k_axisScale = 10.4f;
			p2 = p1 + k_axisScale * xf.q.GetXAxis();
			DrawSegment(p1, p2, b2Color(1, 0, 0));

			p2 = p1 + k_axisScale * xf.q.GetYAxis();
			DrawSegment(p1, p2, b2Color(0, 1, 0));
			m_debugDraw.DrawTransform(xf);
			*/

		}

		glLineWidth(1);
	}
	for (TSpline* _tspline : _TSplines) {
		Vertex* v2 = NULL;
		Vertex* first = NULL;
#if 1
		for (Vertex* v1 : _tspline->getSpline()) {
			if (NULL == first)first = v1;
			v1->Draw(false, _isEdit || m_isEditPhysics);
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
}
