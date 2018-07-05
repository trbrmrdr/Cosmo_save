#pragma once
#include "stdafx.h"


class PointArray2: public PointArray {
public:
	virtual ~PointArray2() {};

	PointArray2():PointArray() {};

	const Vec2& getControlPointAtIndex2(ssize_t index) const {
		//index = MIN(static_cast<ssize_t>(_controlPoints.size()) - 1, MAX(index, 0));
		//return _controlPoints.at(index);
		ssize_t tIndex = index;
		ssize_t pSize = static_cast<ssize_t>(getControlPoints().size());
		if (tIndex < 0) tIndex += pSize;
		if (tIndex >= pSize) tIndex -= pSize;
		return getControlPoints().at(tIndex);
	};

};

class DrawNode2: public DrawNode {
public:


	DrawNode2();
	virtual ~DrawNode2() {};

	static DrawNode2* create();

	void drawCircle_ex(const Vec2& center, float radius, float expand);

	void drawFaceCircle(const Vec2& center, float radius, float expand);

	void drawPolygon(const std::vector<Vec2>& verts, const Color4F &fillColor, float borderWidth = 0., const Color4F &borderColor = Color4F::WHITE);

	void drawCardinalSpline(PointArray2* config, float tension, unsigned int segments, const Color4F &color);
};
