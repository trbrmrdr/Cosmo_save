#pragma once
#include "stdafx.h"
#include "WorldSpline.h"
#include "World/CreateWorld/DataSpline.h"


using namespace std;

#if !defined(CC_ENABLE_BOX2D_INTEGRATION)
needed enable box 2d
#endif

#include "Box2D/Box2D.h"
#include "Box2d/GLES-Render.h"

#include "Box2D/boyancy/iforce2d_Buoyancy_functions.h"
using namespace b2dHelper;


#define TO_DATA_SPLINE(x)	(static_cast<DataSpline*>(x))
#define IS_WATER(body)	(NULL != body->GetUserData() && TO_DATA_SPLINE(body->GetUserData())->getType() == DSType_Water)
#define IS_DISABLEW(body)	(NULL != body->GetUserData() && TO_DATA_SPLINE(body->GetUserData())->isEnableW())

class WorldSpline_and_Physics : public WorldSpline
{
protected:
	bool m_isEditPhysics;
#if HAS_DEVELOP
	LayoutRefresh* m_editEditPhys = NULL;
	virtual void setEditPhysics(const bool& enable);
#endif
public:
	WorldSpline_and_Physics(int max_count_spline = -1, DevelopTabs tabs = DevelopTabs_Spline, bool createEdits = false);
	//WSphysics(Layer* layer);
	//virtual ~WorldSpline_and_Physics();


	virtual void loadChild(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f);
	virtual void saveChild(TSpline* spline, __Dictionary* dictInSplines, const Vec2& d_pos = Vec2::ZERO, const float& scale = 1.f);
	
	
	virtual void createPhysics(bool recreate = false);
	static void CreateFixture(const std::vector<TSpline*>& splines, bool recreate);
	static void CreateFixture(TSpline* spline, bool recreate);

	static DataSpline* GetData(TSpline* spline);

#if HAS_DEVELOP
	virtual void select(bool force = false);
	virtual void changeEdits(bool create);
#endif
	virtual void draw();
};