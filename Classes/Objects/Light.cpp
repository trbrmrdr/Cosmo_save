#include "stdafx.h"
#include "Light.h"
#include "DevelopMenu.h"
#include "LightSystem/LightSystem.h"
const float _angle_softness = 0.43625f;//25.0   - angle blyr shadow light

Light::Light(Layer* layer, size_t id) :
	Soul(),
	_id(id),
	_updateShadow(false),
	_debugDraw(false),
	_cover(-1),
	_direct(-1) {
	_blurAngle = M_PI_2 / 40.f;
	_blurStrenght = 0.5f;

	_methodBlur = 1;
	_brightnessShadow = 0.6f;

	_blurRadius = 20.0;//32
	_blurSample = 10.0;
	_upscale = 1;

	_layer = layer;
	_lamp = Sprite::create("lightbulb.png");

	_layer->addChild(_lamp);

	LightSystem::Instance().addLight(this);

	Vec2 pos_def = _layer->convertToNodeSpace(VisibleRect::center());
	setPos(pos_def);
	setZ(100);
	setColor(Color3B::WHITE);
	setBrightness(0);
	setCutoffRadius(100);
	setHalfRadius(0.9f);
	setDirect(0);
	setCover(_M_2PI);

#if HAS_DEVELOP
	DevelopMenu::Instance().changeDebugMode(this, ([this](const bool& _debugMode) {
		_debugDraw = _debugMode;
		_lamp->setVisible(_debugMode);
		if (_debugDraw)
			_updateShadow = true;
	}));
#endif
}

bool Light::select() {
#if HAS_DEVELOP
	if (isSelected)
		return false;
	isSelected = true;
	{
		edit_pos = DevelopMenu::Instance().addPointSetting(
			this, 0,
			DevelopTabs_World, "pos",
			([this](const Vec2& pos) { setPos(pos); }),
			([this](void) { return  getPos(); }));

		edit_z = DevelopMenu::Instance().addFloatSetting(
			this, 0,
			DevelopTabs_World, "z",
			([this](const float& value) { setZ(value); }),
			([this](void) { return getZ(); }));
		/**/
		edit_cutoffRadius = DevelopMenu::Instance().addFloatSetting(
			this, 0,
			DevelopTabs_World, "Edit CutoffRadius",
			([this](const float& value) { setCutoffRadius(value); }),
			([this](void) { return getCutoffRadius(); }));


		edit_halfRadius = DevelopMenu::Instance().addFloatSliderSetting(
			this, 0,
			DevelopTabs_World, "Edit HalfRadius",
			([this](const float& val) { setHalfRadius(val); }),
			([this](void) { return getHalfRadius(); }),
			SeparatorType_Bottom,
			([this](void) { return 0; }),
			([this](void) { return 1.f; }));

		edit_brightness = DevelopMenu::Instance().addFloatSetting(
			this, 0,
			DevelopTabs_World, "Edit Brightness",
			([this](const float& value) { setBrightness(value); }),
			([this](void) { return getBrightness(); }));

		edit_direct = DevelopMenu::Instance().addFloatSliderSetting(
			this, 0,
			DevelopTabs_World, "Edit _direct",
			([this](const float& value) { setDirect(value); }),
			([this](void) { return getDirect(); }),
			SeparatorType_None,
			([this](void) { return 0; }),
			([this](void) { return _M_2PI; }));

		edit_cover = DevelopMenu::Instance().addFloatSliderSetting(
			this, 0,
			DevelopTabs_World, "Edit _cover",
			([this](const float& value) { setCover(value); }),
			([this](void) { return getCover(); }),
			SeparatorType_None,
			([this](void) { return 0; }),
			([this](void) { return _M_2PI; }));

		///*
		edit_color = DevelopMenu::Instance().addColorSetting(
			this, 0,
			DevelopTabs_World, "Edit Color",
			([this](const Color3B& color) { setColor(color); }),
			([this](void) { return getColor(); }),
			SeparatorType_Bottom);
		/**/
	}

	//edit_ambientColor = DevelopMenu::Instance().addColorSetting( DevelopTabs_Light, "Light AmbientColor",
	//													  ([this]( const Color3B& color ) { setColor( color ); }),
	//													  ([this]( void ) { return getColor(); }),
	//													  SeparatorType_Bottom );

	edit_brightnessShadow = DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit _brightnessShadow",
		([this](const float& value) {

		_brightnessShadow = value;
		_updateShadow = true;
	}),
			([this](void) { return _brightnessShadow; }),
		SeparatorType_None,
		([this](void) { return 0; }),
		([this](void) { return 1; }));

#if SHADOW_METHOD == 1

	DevelopMenu::Instance().addBoolSetting(
		this, 0,
		DevelopTabs_World, "MethodBlur",
		([this](const bool& value) {
		_methodBlur = (value ? 1 : 2);
		_updateShadow = true;

	}),
			([this](void) { return _methodBlur == 1; }),
		SeparatorType_Bottom);

	//____________________________________________________

	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit _blurAngle (0 to 15 rad)",
		([this](const float& value) {

		_blurAngle = value;
		_updateShadow = true;
	}),
			([this](void) { return _blurAngle; }),
		SeparatorType_None,
		([this](void) { return 0; }),
		([this](void) { return _M_PI2 / 6.0f; }));

	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit _blurStrenght",
		([this](const float& value) {

		_blurStrenght = value;
		_updateShadow = true;
	}),
			([this](void) { return _blurStrenght; }),
		SeparatorType_Bottom,
		([this](void) { return 0; }),
		([this](void) { return 1; }));

	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit _softness",
		([this](const float& value) {

		_softness = value;
		_updateShadow = true;
	}),
			([this](void) { return _softness; }),
		SeparatorType_Bottom,
		([this](void) { return 0; }),
		([this](void) { return 2; }));

	//____________________________________________________

	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit blurRadius",
		([this](const float& value) {

		_blurRadius = value;
		_updateShadow = true;
	}),
			([this](void) { return _blurRadius; }),
		SeparatorType_None,
		([this](void) { return 0.1; }),
		([this](void) { return 100; }));
	///*
	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit blurSample",
		([this](const float& value) {

		_blurSample = value;
		_updateShadow = true;
	}),
			([this](void) { return _blurSample; }),
		SeparatorType_Bottom,
		([this](void) { return 0; }),
		([this](void) { return 100; }));
	/**/
#elif SHADOW_METHOD ==2
	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit CountHShadow",
		([this](const float& value) {

		_countHeightSahdow = (size_t)value;
		setCutoffRadius(getCutoffRadius(), true);
	}),
			([this](void) { return _countHeightSahdow; }),
		SeparatorType_Bottom,
		([this](void) { return 1; }),
		([this](void) { return 10; }));

#endif
	//____________________________________________________

	DevelopMenu::Instance().addFloatSliderSetting(
		this, 0,
		DevelopTabs_World, "Edit Upscale",
		([this](const float& value) {

		_upscale = value;
		setCutoffRadius(getCutoffRadius(), true);
	}),
			([this](void) { return _upscale; }),
		SeparatorType_Bottom,
		([this](void) { return 0.1; }),
		([this](void) { return 4; }));

	refresh();
#endif
	return true;
}

void Light::deselect() {
	isSelected = false;
#if HAS_DEVELOP
	DevelopMenu::Instance().eraceSettings(this);
	edit_size = NULL;
	edit_ep = NULL;
	//edit_ambientColor = NULL;
	edit_brightnessShadow = NULL;

	edit_color = NULL;
	edit_brightness = NULL;
	edit_halfRadius = NULL;
	edit_cutoffRadius = NULL;
	edit_pos = NULL;
	edit_z = NULL;
#endif
}

void Light::refresh() {
	if (!isSelected)
		return;
#if HAS_DEVELOP
	edit_pos->Refresh();
	edit_cutoffRadius->Refresh();
	edit_halfRadius->Refresh();
	edit_brightness->Refresh();

	edit_color->Refresh();

	edit_cover->Refresh();
	edit_direct->Refresh();

	edit_z->Refresh();

	edit_brightnessShadow->Refresh();
#endif

#if SHADOW_METHOD == 1

#elif SHADOW_METHOD == 2

#endif	

	//edit_ambientColor->Refresh();
}

void Light::setPos(const Vec2 & pos) {
	Soul::setPos(pos);
	_lamp->setPosition(pos);
	LightSystem::Instance().setLightPos(_id, Vec3(pos.x, pos.y, getZ()));
	_updateShadow = true;
}

void Light::setZ(int val) {
	Soul::setZ(val);
	_lamp->setLocalZOrder(_posZ + 10);
	setPos(getPos());
}

bool simple_sort(TShadowRay i, TShadowRay j) {
	return i._angle < j._angle;
}

void clear_equal(std::vector<TShadowRay>& list, const float ep_lenght = 1, const float ep_rad_angle = 0.0000174532925f) {
	size_t count = 0;
	std::vector<TShadowRay>::iterator it_e = list.end(), it_s = list.begin(), it;
	for (it = it_s; it != it_e; ++it)
	{
		std::vector<TShadowRay>::iterator r1 = it;
		std::vector<TShadowRay>::iterator r2;
		if ((it + 1) != it_e)
			r2 = it + 1;
		else
		{
			r2 = it;
			r1 = it_s;
		}
		if (r2 == r1)
			break;
		float len1 = (*r1)._lenght;
		float ang1 = (*r1)._angle;
		float len2 = (*r2)._lenght;
		float ang2 = (*r2)._angle;

		if (ABS(len1 - len2) <= ep_lenght && ABS(ang1 - ang2) <= ep_rad_angle)
		{
			list.erase(r2);
			it_e = list.end();

			count++;
		}
	}
	CCLOG("clear = %i  size = %i", count, list.size());
}

float fixRadian(const float rad) {
	float ret = ((float)ABS(rad)) > _M_2PI ? (float)(rad - floorf(rad / _M_2PI) * _M_2PI) : rad;
	if (ret < 0.0f)
		ret = _M_2PI + ret;
	return ret;
}

#if SHADOW_METHOD == 1
//0 to _M_2PI
void Light::drawSector(const TShadowRay& from, const TShadowRay& to, float radiuas, DrawNode* draw, const Color4F& color) {
	float len1 = from._lenght;
	float len2 = to._lenght;
	float angle_1 = from._angle;
	float angle_2 = to._angle;

	//CCLOG( "len1 = %f , len2 = %f : d1 = %f , d2 = %f   _cutoffRadius = %f", len1, len2, ABS( len1 - _cutoffRadius ), ABS( len2 - _cutoffRadius ), _cutoffRadius );
	//CCLOG( " %f , %f  ", len1 + len2 - _cutoffRadius * 2, _ep_lenghtLine );
	//CCLOG( "angle %f %f", angle_1, angle_2 );
	//if (true)
	Vec2* verts = new Vec2[_segments];
	verts[0] = Vec2::ZERO;
	verts[1] = from._pos;
	size_t countVerts = 2;


	if (ABS(len2 - radiuas) <= _ep_lenghtLine && ABS(len1 - radiuas) <= _ep_lenghtLine)
		//if (false)
			//if (ABS( len2 - radius ) <= _ep_lenghtLine)
	{
		float inc = ((float)(_M_2PI / _segments));
		inc = _M_PI2;
		float angle_end = angle_2 >= angle_1 ? angle_2 : _M_2PI + angle_2;
		float angle_tmp = angle_1;

		len1 = radiuas * 2;
		verts[1] = Vec2(len1, 0).rotateByAngle(Vec2::ZERO, from._angle);
		for (;;)
		{
			angle_tmp += inc;
			GLfloat x = len1 * cosf(angle_tmp);
			GLfloat y = len1 * sinf(angle_tmp);

			//CCLOG( "->%f   %f:%f", angle_tmp, (float) x, (float) y );
			if (angle_tmp > angle_end || (countVerts + 1 == _segments))
			{
				//verts[countVerts] = to._pos;
				verts[countVerts] = Vec2(len1, 0).rotateByAngle(Vec2::ZERO, angle_end);
				countVerts++;
				break;
			}
			else
			{
				verts[countVerts] = Vec2(x, y);
				countVerts++;
			}
		}
	}
	else
	{
		verts[2] = to._pos;
		countVerts = 3;
	}
	//______
	draw->drawPolygon(verts, countVerts, color, 0, Color4F(0, 0, 0, 1));
	CC_SAFE_DELETE_ARRAY(verts);
}

void Light::drawSectors(Vec2 centr, bool sector, const std::vector<TShadowRay>& list, float radius, DrawNode* draw, const Color4F& color) {

	std::vector<TShadowRay>::const_iterator it_e = list.end(), it_s = list.begin(), it;
	std::vector<TShadowRay>::const_iterator last = it_e;
	bool fast = sector ? false : true;
	float Inc = _M_PI2;
	size_t countVerts = 0;
	for (it = it_s; it != it_e; ++it)
	{
		std::vector<TShadowRay>::const_iterator from = it;
		std::vector<TShadowRay>::const_iterator to;
		if ((it + 1) != it_e)
			to = from + 1;
		else if (sector)
			break;
		else
			to = it_s;

		float len1 = (*from)._lenght;
		float len2 = (*to)._lenght;
		float angle_1 = (*from)._angle;
		float angle_2 = (*to)._angle;

		if (!(last != it_e && last == from))
			countVerts += 2;
		if (ABS(len2 - radius) <= _ep_lenghtLine && ABS(len1 - radius) <= _ep_lenghtLine)
		{
			float inc = (fast) ? Inc : ((float)(_M_2PI / _segments));
			angle_2 = angle_2 >= angle_1 ? angle_2 : _M_2PI + angle_2;
			if (!fast)
			{
				countVerts += (int)((angle_2 - angle_1) / inc) + 1;
			}
			else
			{
				countVerts += 1;
				for (;; )
				{
					angle_1 += inc;
					countVerts += 1;
					if (angle_1 > angle_2)
						break;
				}
				countVerts += 1;
			}
		}
		else
			countVerts += 1;
		last = to;
	}

	Vec2* verts = new Vec2[countVerts];
	size_t vertsPos = 0;
	last = it_e;
	float bigR = _cutoffRadius * 10;
	for (it = it_s; it != it_e; ++it)
	{
		std::vector<TShadowRay>::const_iterator from = it;
		std::vector<TShadowRay>::const_iterator to;
		if ((it + 1) != it_e)
			to = from + 1;
		else if (sector)
			break;
		else
			to = it_s;

		float len1 = (*from)._lenght;
		float len2 = (*to)._lenght;
		float angle_1 = (*from)._angle;
		float angle_2 = (*to)._angle;

		if (!(last != it_e && last == from))
		{
			verts[vertsPos] = centr;// Vec2::ZERO;
			verts[vertsPos + 1] = centr + (*from)._pos;
			vertsPos += 2;
		}

		//if (ABS( len2 - len1 ) <= _ep_lenghtLine)
		if (ABS(len2 - radius) <= _ep_lenghtLine && ABS(len1 - radius) <= _ep_lenghtLine)
		{
			float lenRad = fast ? bigR : len1;
			if (fast)
			{
				verts[vertsPos] = centr + Vec2(bigR, 0).rotateByAngle(Vec2::ZERO, angle_1);
				vertsPos += 1;
			}
			float inc = fast ? Inc : ((float)(_M_2PI / _segments));
			angle_2 = angle_2 >= angle_1 ? angle_2 : _M_2PI + angle_2;
			for (;;)
			{
				angle_1 += inc;
				GLfloat x = lenRad * cosf(angle_1);
				GLfloat y = lenRad * sinf(angle_1);
				if (angle_1 > angle_2)
				{
					if (fast)
						verts[vertsPos] = centr + Vec2(bigR, 0).rotateByAngle(Vec2::ZERO, angle_2);
					else
						verts[vertsPos] = centr + (*to)._pos;
					vertsPos += 1;
					break;
				}
				else
				{
					verts[vertsPos] = centr + Vec2(x, y);
					vertsPos += 1;
				}
			}
			if (fast)
			{
				verts[vertsPos] = centr + Vec2(len2, 0).rotateByAngle(Vec2::ZERO, angle_2);
				vertsPos += 1;
			}
		}
		else
		{
			verts[vertsPos] = centr + (*to)._pos;
			vertsPos += 1;
		}
		last = to;
	}
	if (vertsPos != countVerts)
		countVerts = countVerts;
	draw->drawPolygon(verts, countVerts, color, 0, Color4F(1, 0, 0, 1));
	CC_SAFE_DELETE_ARRAY(verts);
}


//____test
std::vector<Vec2> Light::getListSector(Vec2 centr, bool sector, const std::vector<TShadowRay>& list) {
	std::vector<TShadowRay>::const_iterator it_e = list.end(), it_s = list.begin(), it;
	std::vector<TShadowRay>::const_iterator last = it_e;
	bool fast = sector ? false : true;
	float Inc = _M_PI2;

	std::vector<Vec2> verts;
	float bigR = _cutoffRadius * 10;
	for (it = it_s; it != it_e; ++it)
	{
		std::vector<TShadowRay>::const_iterator from = it;
		std::vector<TShadowRay>::const_iterator to;
		if ((it + 1) != it_e)
			to = from + 1;
		else if (sector)
			break;
		else
			to = it_s;

		float len1 = (*from)._lenght;
		float len2 = (*to)._lenght;
		float angle_1 = (*from)._angle;
		float angle_2 = (*to)._angle;

		if (!(last != it_e && last == from))
		{
			verts.push_back(centr);
			verts.push_back(centr + (*from)._pos);
		}

		if (ABS(len2 - _cutoffRadius) <= _ep_lenghtLine && ABS(len1 - _cutoffRadius) <= _ep_lenghtLine)
		{
			float lenRad = fast ? bigR : len1;
			if (fast)
				verts.push_back(Vec2(bigR, 0).rotateByAngle(Vec2::ZERO, angle_1));
			float inc = fast ? Inc : ((float)(_M_2PI / _segments));
			angle_2 = angle_2 >= angle_1 ? angle_2 : _M_2PI + angle_2;
			for (;;)
			{
				angle_1 += inc;
				GLfloat x = lenRad * cosf(angle_1);
				GLfloat y = lenRad * sinf(angle_1);
				if (angle_1 > angle_2)
				{
					if (fast)
						verts.push_back(centr + Vec2(bigR, 0).rotateByAngle(Vec2::ZERO, angle_2));
					else
						verts.push_back(centr + (*to)._pos);
					break;
				}
				else
					verts.push_back(centr + Vec2(x, y));
			}
			if (fast)
				verts.push_back(centr + Vec2(len2, 0).rotateByAngle(Vec2::ZERO, angle_2));
		}
		else
			verts.push_back(centr + (*to)._pos);
		last = to;
	}
	return verts;
}

void Light::drawSectors2(Vec2 centr, bool sector, const std::vector<TShadowRay>& list, DrawNode2* draw, const Color4F& color) {
	std::vector<Vec2> verts = getListSector(centr, sector, list);
	draw->drawPolygon(verts, color, 0, Color4F(1, 0, 0, 1));
	verts.clear();
}

void Light::drawLight(const std::vector<TSpline*>& polygon, bool isSector, const std::vector<TShadowRay>& lightRay, DrawNode2* drawNode, const Color4F & color_shadow, const Color4F & color_Lshadow) {
	std::vector<Vec2> light;
	std::vector<Vec2> lightShadow;

	std::vector<TShadowRay>::const_iterator it_e = lightRay.end(), it_s = lightRay.begin(), it, prev;
	prev = it_e - 1;
	std::vector<TShadowRay>::const_iterator last = it_e;
	bool fast = isSector ? false : true;
	float Inc = _M_PI2;
	float bigR = _cutoffRadius * 10;
	for (it = it_s; it != it_e; ++it)
	{
		std::vector<TShadowRay>::const_iterator from = it;
		std::vector<TShadowRay>::const_iterator to;
		if ((it + 1) != it_e)
			to = from + 1;
		else if (isSector)
			break;
		else
			to = it_s;
		///### start draw one sector
		{
			float len1 = (*from)._lenght;
			float len2 = (*to)._lenght;
			float angle_1 = (*from)._angle;
			float angle_2 = (*to)._angle;

			if (!(last != it_e && last == from))
			{
				light.push_back(Vec2::ZERO);
				light.push_back((*from)._pos);
			}

			if (ABS(len2 - _cutoffRadius) <= _ep_lenghtLine && ABS(len1 - _cutoffRadius) <= _ep_lenghtLine)
			{
				float lenRad = fast ? bigR : len1;
				if (fast)
					light.push_back(Vec2(bigR, 0).rotateByAngle(Vec2::ZERO, angle_1));
				float inc = fast ? Inc : ((float)(_M_2PI / _segments));
				angle_2 = angle_2 >= angle_1 ? angle_2 : _M_2PI + angle_2;
				for (;;)
				{
					angle_1 += inc;
					GLfloat x = lenRad * cosf(angle_1);
					GLfloat y = lenRad * sinf(angle_1);
					if (angle_1 > angle_2)
					{
						if (fast)
							light.push_back(Vec2(bigR, 0).rotateByAngle(Vec2::ZERO, angle_2));
						else
							light.push_back((*to)._pos);
						break;
					}
					else
						light.push_back(Vec2(x, y));
				}
				if (fast)
					light.push_back(Vec2(len2, 0).rotateByAngle(Vec2::ZERO, angle_2));
			}
			else
				light.push_back((*to)._pos);
		}
		///######### end draw one sector

		if (_softness != 0 && (*from)._unique)
		{
			size_t var = 1;
			TShadowRay r2 = (*to);
			TShadowRay centr = (*from);
			TShadowRay r0 = (*prev);

			float d_r = 1.0 - centr._lenght / _cutoffRadius;
			bool draw = false;
			float direct;
			float cover = _angle_softness * MAX(0.0, d_r - _halfRadius);;

			if (ABS(r0._lenght - centr._lenght) > _ep_lenghtLine)
			{
				draw = true;
				direct = r0._angle + cover * 0.5f;
			}
			else if (ABS(centr._lenght - r2._lenght) > _ep_lenghtLine)
			{
				draw = true;
				direct = r2._angle - cover * 0.5f;
			}

			if (draw)
			{
				if (_softness == 2)
				{
					float radius = _cutoffRadius - centr._lenght;
					std::vector<Vec2> lightRay_Vec2 = createLightRay(radius, direct, cover, _segments);
					if (lightRay_Vec2.size() > 1)
					{
						std::vector<TShadowRay> lightRay_tmp = getLightRay(lightRay_Vec2, polygon,
							centr._pos + _pos,
							radius, direct, cover,
							_segments, true);
						if (lightRay_tmp.size() > 1)
						{
							std::vector<Vec2> tmp = getListSector(centr._pos, true, lightRay_tmp);
							lightShadow.insert(lightShadow.end(), tmp.begin(), tmp.end());
						}
					}
				}
				else if (_softness == 1)
				{
					lightShadow.push_back(centr._pos);
					lightShadow.push_back(Vec2(_cutoffRadius * 10, 0).rotateByAngle(Vec2::ZERO, direct));
					lightShadow.push_back(Vec2(_cutoffRadius * 10, 0).rotateByAngle(Vec2::ZERO, direct + cover * 0.5f));
				}
			}
		}

		prev = from;
		last = to;
	}

	drawNode->drawPolygon(light, color_shadow);
	drawNode->drawPolygon(lightShadow, color_Lshadow);
}
//_____

void Light::checkNewRay(const TShadowRay& ray, const TShadowRay& ray2, std::vector<TShadowRay>& list, float radius) {
	float len1 = ray._lenght;
	float len2 = ray2._lenght;
	float angle_1 = ray._angle;
	float angle_2 = ray2._angle;


	//if (ABS( len2 - len1 ) <= _ep_lenghtLine)
	if (ABS(len2 - radius) <= _ep_lenghtLine && ABS(len1 - radius) <= _ep_lenghtLine)
	{
		//const float min_rad = 0.0174532925f;// 0.0000174532925f; MATH_DEG_TO_RAD( 0.001 )
		//if (ABS( angle_1 - angle_2 ) <= min_rad)
		//	return false;
		angle_1 = fixRadian(angle_1 + (angle_2 - angle_1) * 0.5f);
		Vec2 p = Vec2(len1, 0).rotateByAngle(Vec2::ZERO, angle_1);
		list.push_back(TShadowRay(p, angle_1));

		return;
		angle_2 = fixRadian(angle_1 + _M_PI);
		p = Vec2(len1, 0).rotateByAngle(Vec2::ZERO, angle_2);
		list.push_back(TShadowRay(p, angle_2));
	}
}

#endif

bool Light::isVisible() {
	Size sizeShadow = Size(
		_cutoffRadius * 2 * _layer->getScaleX(),
		_cutoffRadius * 2 * _layer->getScaleY());
	Rect rectShadow = Rect(_layer->convertToWorldSpace(getPos()) - sizeShadow * 0.5, sizeShadow);
	if (!VisibleRect::getVisibleRect().intersectsRect(rectShadow))
		return false;
	return true;
}

size_t max_label = 100;
size_t count_label = 0;
void printText(bool first, Node* parent, Vec2 p1, float angle, size_t i) {
	if (first)
	{
		count_label = 0;
		for (uint i = 0; i < max_label; i++)
			parent->removeChildByTag(12000 + i);
	}
	else
	{
		count_label++;
		if (count_label > max_label)
			return;
	}
	char str[100]{};
	sprintf(str, "%i - %.2f", i, angle);
	auto label = Label::createWithTTF(str, "fonts/Marker Felt.ttf", 15);
	label->setAnchorPoint(Vec2(0, 0));
	label->setPosition(p1);
	parent->addChild(label, 1, 12000 + count_label);
}

bool Light::inDirContains(const Vec2& p, float direct, float cover) {
	float angle = p.getAngle();
	return inDirContains(angle, direct, cover);
}

bool Light::inDirContains(float rad, float direct, float cover) {
	float angle = rad;
	if (angle <= 0)
		angle += _M_2PI;
	angle = ABS(angle - direct);
	if (_M_2PI - angle > 0)
		angle = MIN(_M_2PI - angle, angle);
	if (angle >= cover * 0.5)
		return false;
	return true;
}

bool checkSector(float cover) {
	return ABS(ABS(cover) - ABS(_M_2PI)) > _EP_RAD;
}

#if SHADOW_METHOD == 1

std::vector<TShadowRay> Light::getLightRay(const std::vector<Vec2>& lights, const std::vector<TSpline*>& polygon,
	const Vec2& pos,
	float radius, float direct, float cover,
	int segments, bool ignoreZ) {
	std::vector<TShadowRay> ret;
	bool isSector = checkSector(cover);

	std::vector<Vec2> sectorList = lights;
	for (Vec2& it : sectorList)
	{
		float angle = fixRadian(it.getAngle());
		ret.push_back(TShadowRay(it, angle));
	}

	std::vector<std::pair<TShadowRay, TShadowRay> > polygon_tmp;
	//до угловых точек в нутри радиуса 
	//+ пересечение радиуса с фигурами
	for (TSpline* Tspline : polygon) {
		for (Vertex* var in Tspline->getSpline()) {
			size_t contains = 0;
			Vec2 p1 = var->getPos();
			Vec2 convert_p1 = p1 - pos;
			Vec2 convert_p2;
			if (p1.distance(pos) <= radius)// distanc from all vertex spline
			{
				float angle = convert_p1.getAngle();
				float angle_p1 = fixRadian(angle);
				bool skip = isSector;
				if (isSector && inDirContains(angle, direct, cover))//if sector
					skip = false;

				if (!skip)
				{
					contains += 1;
					Vec2 tmp_n = convert_p1.getNormalized() * radius;
					ret.push_back(TShadowRay(convert_p1, angle_p1, true));

					Vec2 tmp_n1 = tmp_n.rotateByAngle(Vec2::ZERO, +_RAD_0001);
					ret.push_back(TShadowRay(tmp_n1, angle_p1 + _RAD_0001));

					Vec2 tmp_n2 = tmp_n.rotateByAngle(Vec2::ZERO, -_RAD_0001);
					ret.push_back(TShadowRay(tmp_n2, angle_p1 - _RAD_0001));
				}
			}
			//____
			if (NULL != var->getHead())
			{
				Vec2 p2 = var->getHead()->getPos();
				Vec2 cross;
				convert_p2 = p2 - pos;
				std::vector<Vec2>::iterator last = sectorList.begin();
				for (std::vector<Vec2>::iterator it = sectorList.begin(), it_e = sectorList.end(); it != it_e; ++it)
				{
					if (it != last && MathUtils::IsLineIntersect(convert_p1, convert_p2, (*last), (*it), &cross))
					{
						ret.push_back(TShadowRay(cross, fixRadian(cross.getAngle())));
						contains += 1;
					}
					last = it;
				}
				if (!isSector &&  MathUtils::IsLineIntersect(convert_p1, convert_p2, (*(sectorList.end() - 1)), (*sectorList.begin()), &cross))
				{
					ret.push_back(TShadowRay(cross, fixRadian(cross.getAngle())));
					contains += 1;
				}
			}
			if (contains > 0)
				polygon_tmp.push_back(std::pair<TShadowRay, TShadowRay >(TShadowRay(convert_p1, 0), TShadowRay(convert_p2, 0)));

		}
	}

	if (isSector)
	{
		ret.push_back(TShadowRay((*sectorList.begin())));
		ret.push_back(TShadowRay((*(sectorList.end() - 1))));
	}

	//добовление лучей между равными лучами
	if (ret.size() >= 2)
	{
		std::sort(ret.begin(), ret.end(), simple_sort);

		std::vector< TShadowRay > lines_inCircle_2;
		std::vector<TShadowRay>::iterator it_e = ret.end();
		for (std::vector<TShadowRay>::iterator it = ret.begin(); it != it_e; ++it)
		{
			if (it + 1 != it_e)
				checkNewRay((*it), (*(it + 1)), lines_inCircle_2, radius);
		}
		if (!isSector && ret.size() > 2)
			checkNewRay(*(it_e - 1), *(ret.begin()), lines_inCircle_2, radius);
		ret.insert(ret.end(), lines_inCircle_2.begin(), lines_inCircle_2.end());
	}
	//удаление левых лучей
	//фильтр углов
	if (isSector)
	{
		float cover_2 = cover * 0.5f;
		float minDir = direct - cover_2;
		std::vector<TShadowRay>::iterator tmp_it, it_e = ret.end();
		for (std::vector<TShadowRay>::iterator it = ret.begin(); it != it_e; ++it)
		{
			if (it->_angle > _M_PI)
				it->_angle -= _M_2PI;
			//#################
			bool er = false;
			float angle = it->_angle;
			if (angle <= 0)
				angle += _M_2PI;
			angle = ABS(angle - direct);
			if (_M_2PI - angle > 0)
				angle = MIN(_M_2PI - angle, angle);
			//if ((cover_2 - angle) > _EP_RAD)
			if (angle - 0.0001f > cover_2)
				er = true;
			if (it->_lenght <= _ep_lenghtLine)
				er = true;
			//###################
			if (er)
			{
				bool first = it == ret.begin();
				bool skip = it + 1 == it_e;
				if (!skip && !first)
					tmp_it = it - 1;
				ret.erase(it);
				if (skip)
					break;
				else if (first)
					it = ret.begin();
				else
					it = tmp_it;
				it_e = ret.end();
			}

			if ((minDir - it->_angle) > _EP_RAD)
			{
				it->_angle += _M_2PI;
			}
		}
		std::sort(ret.begin(), ret.end(), simple_sort);
	}
	//_____

	//второй проход 
	//на пересечения от пересечённых
	for (auto& Tspline in polygon) {
		for (Vertex* var in Tspline->getSpline()) {
			if (NULL != var && NULL != var->getHead())
			{
				Vec2 tmp_p1 = var->getPos() - pos;
				Vec2 tmp_p2 = var->getHead()->getPos() - pos;
				Vec2 cross;
				for (TShadowRay& ray : ret)
				{
					if (MathUtils::IsLineIntersect(tmp_p1, tmp_p2, Vec2::ZERO, ray._pos, &cross))
					{
						if (ignoreZ)
						{
							if (!Rect(Vec2::ZERO, Size(_ep_lenghtLine, _ep_lenghtLine)).containsPoint(cross))
								ray.setPos(cross);
						}
						else
							ray.setPos(cross);
					}
				}
			}
		}
	}
	return ret;
}

#endif


DrawNode2* test = NULL;
void Light::update(const std::vector<TSpline*>& polygon, bool isChange) {

	if (!isChange && !_updateShadow)
		return;

	if (_prevVisible && isChange && !_updateShadow)
		return;

	_updateShadow = false;
	_prevVisible = isVisible();

	if (!_prevVisible)
		return;

#if SHADOW_METHOD == 0
	return;
#endif

	LightSystem::Instance().setChange(this);
	bool isSector = checkSector(getCover());
	if (getCover() <= _EP_RAD)
		return;

#if SHADOW_METHOD == 1
	std::vector<TShadowRay> lightRay = getLightRay(_lightsRay, polygon, getPos(), _cutoffRadius, getDirect(), getCover(), _segments);

	Color4F color_shadow = Color4F(1, 1, 1, 1);
	DrawNode2* drawNode_light = DrawNode2::create();
	drawNode_light->setLineWidth(1);

	Color4F color_Lshadow = Color4F(1, 0, 0, 1);
	DrawNode2* drawNode_shadowLight = DrawNode2::create();
	drawNode_shadowLight->setLineWidth(1);

	if (!lightRay.empty())
	{
		if (!isSector)
			std::sort(lightRay.begin(), lightRay.end(), simple_sort);
		//todo remove
		//clear_equal( lightRay,1.0f,0.0001f );
		///*
		if (false)
		{
			std::vector<TShadowRay>::const_iterator it_e = lightRay.end(), it_s = lightRay.begin(), it, prev;
			prev = it_e - 1;
			for (it = it_s; it != it_e; ++it)
			{
				std::vector<TShadowRay>::const_iterator from = it;
				std::vector<TShadowRay>::const_iterator to;
				if ((it + 1) != it_e)
					to = from + 1;
				else if (isSector)
					break;
				else
					to = it_s;

				if ((*from)._unique)
				{
					size_t var = 1;
					TShadowRay r2 = (*to);
					TShadowRay centr = (*from);
					TShadowRay r0 = (*prev);

					if (ABS(r0._lenght - centr._lenght) > _ep_lenghtLine)
						CCLOG("check 0");
					else if (ABS(centr._lenght - r2._lenght) > _ep_lenghtLine)
						CCLOG("check 2");

					CCLOG("%2.4f : %2.4f : %i ", (*prev)._angle, (*prev)._lenght, (int)(*prev)._unique);
					CCLOG("%2.4f : %2.4f : %i ", (*from)._angle, (*from)._lenght, (int)(*from)._unique);
					CCLOG("%2.4f : %2.4f : %i ", (*to)._angle, (*to)._lenght, (*to)._unique);
				}
			}
			CCLOG("#########################");
		}

		if (_softness != 0)
		{

			std::vector<TShadowRay>::const_iterator it_e = lightRay.end(), it_s = lightRay.begin(), it, prev;
			prev = it_e - 1;
			std::vector<Vec2> light;
			std::vector<Vec2> lightShadow;
			for (it = it_s; it != it_e; ++it)
			{
				std::vector<TShadowRay>::const_iterator from = it;
				std::vector<TShadowRay>::const_iterator to;
				if ((it + 1) != it_e)
					to = from + 1;
				else if (isSector)
					break;
				else
					to = it_s;
				if (_softness != 0 && (*from)._unique)
				{
					size_t var = 1;
					TShadowRay r2 = (*to);
					TShadowRay centr = (*from);
					TShadowRay r0 = (*prev);

					float d_r = 1.0 - centr._lenght / _cutoffRadius;
					bool draw = false;
					float direct;
					float cover = _angle_softness * MAX(0.0, d_r - _halfRadius);;

					if (ABS(r0._lenght - centr._lenght) > _ep_lenghtLine)
					{
						draw = true;
						if (_softness == 1)
							direct = r0._angle - cover * 0.5f;
						else
							direct = r0._angle + cover * 0.5f;
					}
					else if (ABS(centr._lenght - r2._lenght) > _ep_lenghtLine)
					{
						draw = true;
						direct = r2._angle - cover * 0.5f;
					}

					if (draw)
					{
						if (_softness == 2)
						{
							float radius = _cutoffRadius - centr._lenght;
							std::vector<Vec2> lightRay_Vec2 = createLightRay(radius, direct, cover, _segments);
							if (lightRay_Vec2.size() > 1)
							{
								std::vector<TShadowRay> lightRay_tmp = getLightRay(lightRay_Vec2, polygon,
									centr._pos + _pos,
									radius, direct, cover,
									_segments, true);
								if (lightRay_tmp.size() > 1)
									drawSectors(centr._pos, true, lightRay_tmp, radius, drawNode_shadowLight, color_Lshadow);
							}
						}
						else if (_softness == 1)
						{
							Vec2 *vertices = new (std::nothrow) Vec2[3];
							vertices[0] = centr._pos;
							vertices[1] = Vec2(_cutoffRadius * 10, 0).rotateByAngle(Vec2::ZERO, direct);
							vertices[2] = Vec2(_cutoffRadius * 10, 0).rotateByAngle(Vec2::ZERO, direct + cover * 0.5f);
							drawNode_shadowLight->drawSolidPoly(vertices, 3, color_Lshadow);
							CC_SAFE_DELETE_ARRAY(vertices);
						}
					}
				}
				drawSector((*from), *(to), _cutoffRadius, drawNode_light, color_shadow);
				prev = from;
			}
		}
		else
		{
			drawSectors(Vec2::ZERO, isSector, lightRay, _cutoffRadius, drawNode_light, color_shadow);
			//drawLight( polygon, isSector, lightRay, drawNode_light, color_shadow, color_Lshadow );
		}
	}

	//render in texture
	_light_renderTexture->beginWithClear(0, 0, 0, 0.0);

#if 1
	drawNode_light->setScale(_upscale);
	drawNode_light->setPosition(_shadow_rt_centr);
	drawNode_light->visit();
#endif

#if 1
	if (_softness != 0)
	{
		drawNode_shadowLight->setScale(_upscale);
		drawNode_shadowLight->setPosition(_shadow_rt_centr);
		drawNode_shadowLight->visit();
	}
#endif
	_light_renderTexture->end();
	/*
   if (NULL == _light_renderTexture->getParent())
	   _layer->addChild( _light_renderTexture );
   _light_renderTexture->setLocalZOrder( (int) _posZ );
   _light_renderTexture->setPosition( getPos() );
   return;/**/

   //draw shadow and gradient 
	GLProgramState* shader = NULL;
	{
		Sprite* lightMap = _light_renderTexture->getSprite();
		lightMap->setPosition(_shadow_rt_centr);

		shader = _shader_GradientMap->getGLProgramState();
		shader->setUniformFloat("_halfRadius", _halfRadius);
		shader->setUniformFloat("_brightness", _brightnessShadow);
		shader->setUniformInt("_method", _methodBlur);

		lightMap->setGLProgramState(shader);

		_shadow_renderTexture->beginWithClear(0, 0, 0, 0.0, 0);
		lightMap->visit();
		_shadow_renderTexture->end();

		//_shadow_renderTexture->saveToFile( "01_shader_GradientMap.png" );

		//lightMap->setGLProgramState( shaderDef );
		/*
		if (NULL == _shadow_renderTexture->getParent())
			_layer->addChild( _shadow_renderTexture );
		_shadow_renderTexture->setLocalZOrder( (int)_posZ );
		_shadow_renderTexture->setPosition( getPos() );
		return;/**/
	}
	//radial blur from shadow
	{
		Sprite* gradient_shadow = _shadow_renderTexture->getSprite();
		gradient_shadow->setPosition(_shadow_rt_centr);

		shader = _shader_RadialBlur->getGLProgramState();
		shader->setUniformFloat("_blurAngle", _blurAngle);
		shader->setUniformFloat("_blurStrenght", _blurStrenght);

		gradient_shadow->setGLProgramState(shader);

		_shadow_renderTexture_2->beginWithClear(0, 0, 0, 0.0, 0);
		gradient_shadow->visit();
		_shadow_renderTexture_2->end();

		//_shadow_renderTexture_2->saveToFile( "02_shader_RadialBlur.png" );

		/*
		if (NULL == _shadow_renderTexture_2->getParent())
			_layer->addChild( _shadow_renderTexture_2 );
		_shadow_renderTexture_2->setLocalZOrder( (int) _posZ );
		_shadow_renderTexture_2->setPosition( getPos() );
		return;/**/
	}
	//3 step
	{

		Sprite* blurShadow = _shadow_renderTexture_2->getSprite();
		blurShadow->setPosition(_shadow_rt_centr);

		shader = _shader_Blur->getGLProgramState();
		shader->setUniformFloat("_blurRadius", _blurRadius);
		shader->setUniformInt("_sampleNum", _blurSample);
		shader->setUniformVec2("_resolution", blurShadow->getContentSize());
		shader->setUniformFloat("_direct", _direct);
		shader->setUniformFloat("_cover", _cover);
		blurShadow->setGLProgramState(shader);

		_shadow_renderTexture_3->beginWithClear(0, 0, 0, 0.0, 0);
		blurShadow->visit();
		_shadow_renderTexture_3->end();

		_shadow_renderTexture_3->saveToFile("03_shader_Blur.png");
		///*


		//градиент от центра невырежется из тени при инвертировании тени для вычислении только света
		if (NULL == _shadow_renderTexture_3->getParent())
			_layer->addChild(_shadow_renderTexture_3);
		_shadow_renderTexture_3->setLocalZOrder((int)_posZ);
		_shadow_renderTexture_3->setPosition(getPos());
		return;/**/
	}
	_lightMap = _shadow_renderTexture_3;
	_lightMap->setScale(_shadow_renderTexture->getScale());
	_lightMap->setPosition(getPos());

#elif SHADOW_METHOD == 2

	Color4F color_shadow = Color4F(0, 0, 0, 1);
	DrawNode2* drawNode_barier = DrawNode2::create();
	drawNode_barier->setLineWidth(2);

	Vec2 d1 = getPos() + Vec2(-_cutoffRadius, -_cutoffRadius);
	Vec2 d2 = getPos() + Vec2(-_cutoffRadius, _cutoffRadius);
	Vec2 d3 = getPos() + Vec2(_cutoffRadius, _cutoffRadius);
	Vec2 d4 = getPos() + Vec2(_cutoffRadius, -_cutoffRadius);

	if (test == NULL)
	{
		test = DrawNode2::create();
		_layer->addChild(test, getZ() + 1);
	}
	test->clear();
	test->drawLine(d1, d2, Color4F::BLUE);
	test->drawLine(d2, d3, Color4F::BLUE);
	test->drawLine(d3, d4, Color4F::BLUE);
	test->drawLine(d4, d1, Color4F::BLUE);

	for (TSpline* Tspline : polygon) {
		for (Vertex* var : Tspline->getSpline()) {
			size_t contains = 0;
			Vec2 p1 = var->getPos();
			Vec2 cp1 = p1 - getPos();
			const Vertex* head = var->getHead();
			if (NULL == head)
				continue;
			Vec2 p2 = head->getPos();
			Vec2 cp2 = p2 - getPos();
			bool isFind = false;
			if (p1.distance(getPos()) <= _cutoffRadius)// distanc from all vertex spline
			{
				isFind = true;
			}
			if (!isFind)
			{
				Vec2 cross;

				if (var->getId() == 1000)
					cross = cross;
				if (MathUtils::IsLineIntersect(p1, p2, d1, d2, &cross) ||
					MathUtils::IsLineIntersect(p1, p2, d2, d3, &cross) ||
					MathUtils::IsLineIntersect(p1, p2, d3, d4, &cross) ||
					MathUtils::IsLineIntersect(p1, p2, d4, d1, &cross))
				{
					isFind = true;
				}
			}
			if (isFind)
			{
				drawNode_barier->drawLine(cp1, cp2, color_shadow);
				if (NULL != var->getTail())
					drawNode_barier->drawLine(var->getTail()->getPos() - getPos(), cp1, color_shadow);
			}
		}
	}

	_occlusionMap_rendrTexture->beginWithClear(1, 1, 1, 0, 0);
	//_occlusionMap_rendrTexture->setScale( _upscale )
	Vec2 rt_centr = Vec2(_occlusionMap_rendrTexture->getSprite()->getContentSize().width, _occlusionMap_rendrTexture->getSprite()->getContentSize().height) * 0.5f;
	drawNode_barier->setScale(_upscale);
	drawNode_barier->setPosition(rt_centr);
	drawNode_barier->visit();
	_occlusionMap_rendrTexture->end();

	/*
	if (NULL == _occlusionMap_rendrTexture->getParent())
		_layer->addChild(_occlusionMap_rendrTexture);
	_occlusionMap_rendrTexture->setLocalZOrder((int) _posZ);
	_occlusionMap_rendrTexture->setPosition(getPos());
	return;/**/
	//#####################################################

	Sprite* occlusionSprite = _occlusionMap_rendrTexture->getSprite();
	occlusionSprite->setFlippedY(true);
	occlusionSprite->setAnchorPoint(Vec2::ZERO);

	GLProgramState* programShadowMap = _shader_shadowMap->getGLProgramState();
	programShadowMap->setUniformVec2("resolution", occlusionSprite->getContentSize());
	programShadowMap->setUniformTexture("u_texture", occlusionSprite->getTexture());
	programShadowMap->setUniformInt("_countHeightSahdow", _countHeightSahdow);


	Sprite* sprite = _shadowMap_1D_rederTexture->getSprite();
	sprite->setGLProgramState(programShadowMap);
	_shadowMap_1D_rederTexture->beginWithClear(0, 0, 0, 0, 0);
	sprite->setPosition(sprite->getContentSize()*0.5);
	sprite->visit();

	_shadowMap_1D_rederTexture->end();

	//_shadowMap_1D_rederTexture->saveToFile( "test.png" );

	/*
	if (NULL == _shadowMap_1D_rederTexture->getParent())
		_layer->addChild(_shadowMap_1D_rederTexture);
	_shadowMap_1D_rederTexture->setLocalZOrder((int) _posZ);
	_shadowMap_1D_rederTexture->setPosition(getPos());
	_shadowMap_1D_rederTexture->setScale(_upscale);
	_shadowMap_1D_rederTexture->setScaleY(_upscale * 10);
	return;
	/**/
	//#######################################################

	GLProgramState* programShadowRender = _shader_shadowRender->getGLProgramState();
	programShadowRender->setUniformVec2("resolution", occlusionSprite->getContentSize());
	programShadowRender->setUniformFloat("softShadows", _brightnessShadow);
	programShadowRender->setUniformTexture("u_texture2", _shadowMap_1D_rederTexture->getSprite()->getTexture());
	programShadowRender->setUniformInt("_countHeightSahdow", _countHeightSahdow);

	_shadow_renderTexture->clear(0, 0, 0, 0);
	Sprite* finalShadowMapSprite = _shadow_renderTexture->getSprite();
	finalShadowMapSprite->setGLProgramState(programShadowRender);
	finalShadowMapSprite->setFlippedY(false);
	finalShadowMapSprite->visit();

	finalShadowMapSprite->removeFromParent();
	if (NULL == finalShadowMapSprite->getParent())
		_layer->addChild(finalShadowMapSprite);
	finalShadowMapSprite->setLocalZOrder((int)_posZ);
	finalShadowMapSprite->setPosition(getPos());
	finalShadowMapSprite->setScale(1.0 / _upscale);

#endif


}

Light::~Light() {
	CCLOG("~Light");
	_shadow_renderTexture->removeFromParent();
	CC_SAFE_RELEASE(_shadow_renderTexture);
#if SHADOW_METHOD == 1
	CC_SAFE_RELEASE(_shadow_renderTexture_2);
	CC_SAFE_RELEASE(_shadow_renderTexture_3);
	CC_SAFE_RELEASE(_light_renderTexture);
	CC_SAFE_RELEASE(_shader_GradientMap);
	CC_SAFE_RELEASE(_shader_RadialBlur);
	CC_SAFE_RELEASE(_shader_Blur);
#elif SHADOW_METHOD == 2
	CC_SAFE_RELEASE(_shader_shadowMap);
	CC_SAFE_RELEASE(_shader_shadowRender);

	_occlusionMap_rendrTexture->removeFromParent();
	CC_SAFE_RELEASE(_occlusionMap_rendrTexture);

	_shadowMap_1D_rederTexture->removeFromParent();
	CC_SAFE_RELEASE(_shadowMap_1D_rederTexture);
#endif
	LightSystem::Instance().remLight(this);
	deselect();
#if HAS_DEVELOP
	DevelopMenu::Instance().clearChangeDebugMode(this);
#endif
	_layer->removeChild(_lamp);

	CCLOG("~Light_end");
}

void Light::Load(SettingUtil* setting) {
	Soul::Load(setting);
	setCutoffRadius(setting->getFloatForKey("cutoffRadius"));
	setHalfRadius(setting->getFloatForKey("halfRadius"));
	setBrightness(setting->getFloatForKey("brightness"));
	setDirect(setting->getFloatForKey("direct"));
	setCover(setting->getFloatForKey("cover"));
	Vec3 tmp = setting->getVec3ForKey("color");
	setColor(Color3B(tmp.x, tmp.y, tmp.z));
}

SettingUtil* Light::Save() {
	SettingUtil* setting = Soul::Save();
	setting->setFloatForKey("cutoffRadius", getCutoffRadius());
	setting->setFloatForKey("halfRadius", getHalfRadius());
	setting->setFloatForKey("brightness", getBrightness());

	setting->setFloatForKey("direct", getDirect());
	setting->setFloatForKey("cover", getCover());

	Vec3 tmp = Vec3(getColor().r, getColor().g, getColor().b);
	setting->setVec3ForKey("color", tmp);

	return setting;
}

void Light::setCover(float value) {
	if (_cover == value)
		return;
	_cover = value;
	LightSystem::Instance().setLightCover(_id, _cover);
	_updateShadow = true;
#if SHADOW_METHOD == 1
	_lightsRay.clear();
	_lightsRay = createLightRay(_cutoffRadius, _direct, _cover, _segments);
#endif
}

void Light::setDirect(float value) {
	if (_direct == value)
		return;
	_direct = value;
	LightSystem::Instance().setLightDirect(_id, _direct);
	_updateShadow = true;
#if SHADOW_METHOD == 1
	_lightsRay.clear();
	_lightsRay = createLightRay(_cutoffRadius, _direct, _cover, _segments);
#endif
}

void Light::setColor(const Color3B & color) {
	_color = color;
	LightSystem::Instance().setLightColor(_id, color);
}

void Light::setBrightness(float value) {
	_brightness = value;
	LightSystem::Instance().setBrightness(_id, value);
	_updateShadow = true;
}

void Light::setCutoffRadius(float value, bool force) {
	if (!force && _cutoffRadius == value)
		return;
	_cutoffRadius = value;

	float width = _cutoffRadius * 2 * _upscale;
	float height = _cutoffRadius * 2 * _upscale;

	_shadow_rt_centr = Vec2(width, height) * 0.5f;
	if (_shadow_renderTexture)
		_shadow_renderTexture->getSprite()->removeFromParent();
	_layer->removeChild(_shadow_renderTexture);
	if (_shadow_renderTexture)
		_shadow_renderTexture->release();

	_shadow_renderTexture = EffectRenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
	_shadow_renderTexture->retain();
	_shadow_renderTexture->setScale(1.0 / _upscale);
	_shadow_renderTexture->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	_shadow_renderTexture->setClearFlags(GL_COLOR_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT );
	_shadow_renderTexture->getSprite()->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });

#if SHADOW_METHOD == 1
	_lightsRay.clear();
	_lightsRay = createLightRay(_cutoffRadius, _direct, _cover, _segments);
	//________________________________
	{
		{
			if (_shadow_renderTexture_2)
				_shadow_renderTexture_2->getSprite()->removeFromParent();

			_layer->removeChild(_shadow_renderTexture_2);
			if (_shadow_renderTexture_2)
				_shadow_renderTexture_2->release();

			_shadow_renderTexture_2 = RenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
			_shadow_renderTexture_2->retain();
			_shadow_renderTexture_2->setScale(1.0 / _upscale);
			_shadow_renderTexture_2->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

			_shadow_renderTexture_2->setClearFlags(GL_COLOR_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT );
			_shadow_renderTexture_2->getSprite()->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });
		}
		//______________
		{
			if (_shadow_renderTexture_3)
				_shadow_renderTexture_3->getSprite()->removeFromParent();

			_layer->removeChild(_shadow_renderTexture_3);
			if (_shadow_renderTexture_3)
				_shadow_renderTexture_3->release();

			_shadow_renderTexture_3 = RenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
			_shadow_renderTexture_3->retain();
			_shadow_renderTexture_3->setScale(1.0 / _upscale);
			_shadow_renderTexture_3->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

			_shadow_renderTexture_3->setClearFlags(GL_COLOR_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT );
			_shadow_renderTexture_3->getSprite()->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });
		}

		{
			_layer->removeChild(_light_renderTexture);
			if (_light_renderTexture)
				_light_renderTexture->release();

			_light_renderTexture = EffectRenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
			_light_renderTexture->retain();
			_light_renderTexture->setScale(1.0 / _upscale);
			_light_renderTexture->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

			_light_renderTexture->setClearFlags(GL_COLOR_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT );
			_light_renderTexture->getSprite()->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });
		}
		//__
		if (NULL == _shader_GradientMap)
		{
			_shader_GradientMap = Effect::create("shaders/GradientMap.frag");
			_shader_GradientMap->retain();
		}

		if (NULL == _shader_RadialBlur)
		{
			_shader_RadialBlur = Effect::create("shaders/RadialBlur.frag");
			_shader_RadialBlur->retain();
		}

		if (NULL == _shader_Blur)
		{
			_shader_Blur = Effect::create("shaders/Blur.frag");
			_shader_Blur->retain();
		}
	}
	_ep_lenghtLine = (float)(_cutoffRadius / 150);
#elif SHADOW_METHOD == 2

	if (_occlusionMap_rendrTexture)
		_occlusionMap_rendrTexture->getSprite()->removeFromParent();
	_layer->removeChild(_occlusionMap_rendrTexture);

	_occlusionMap_rendrTexture = EffectRenderTexture::create(width, height, Texture2D::PixelFormat::RGBA8888, GL_DEPTH24_STENCIL8);
	_occlusionMap_rendrTexture->retain();
	_occlusionMap_rendrTexture->setScale(1.0 / _upscale);
	_occlusionMap_rendrTexture->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	_occlusionMap_rendrTexture->setClearFlags(GL_COLOR_BUFFER_BIT);// | GL_STENCIL_BUFFER_BIT );
	_occlusionMap_rendrTexture->getSprite()->setBlendFunc({ GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA });
	//______________
	if (_shadowMap_1D_rederTexture)
		_shadowMap_1D_rederTexture->release();
	_layer->removeChild(_shadowMap_1D_rederTexture);

	_shadowMap_1D_rederTexture = RenderTexture::create(width, _countHeightSahdow, Texture2D::PixelFormat::RGBA8888);
	_shadowMap_1D_rederTexture->retain();
	_shadowMap_1D_rederTexture->setScale(1.0 / _upscale);

	//__
	if (NULL == _shader_shadowMap)
	{
		_shader_shadowMap = Effect::create("shaders/ShadowMap.frag");
		_shader_shadowMap->retain();

		_shader_shadowRender = Effect::create("shaders/ShadowRender.frag");
		_shader_shadowRender->retain();
	}
#endif
	LightSystem::Instance().setLightCutoffRadius(_id, value);
	_updateShadow = true;
}

std::vector<Vec2> Light::createLightRay(float radius, float direct, float cover, int segments) {
	std::vector<Vec2> ret;
	float a1 = direct - cover * 0.5;
	float a2 = a1 + cover;
	const float coef = _M_2PI / segments;
	for (size_t i = 0; ; i++)
	{
		bool end = false;
		float rads = a1 + i * coef;
		if (rads >= a2)
		{
			end = true;
			rads = a2;
		}
		GLfloat x = radius * cosf(rads);
		GLfloat y = radius * sinf(rads);
		ret.push_back(Vec2(x, y));
		if (end)
			break;
	}
	return ret;
}

void Light::setHalfRadius(float value) {
	if (_halfRadius == value)
		return;
	_halfRadius = value;
	LightSystem::Instance().setLightHalfRadius(_id, value);
	_updateShadow = true;
}
