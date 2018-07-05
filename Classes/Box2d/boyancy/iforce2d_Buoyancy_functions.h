#pragma once
#include "stdafx.h"

#include "Box2D/Box2D.h"
#include <set>
#include <vector>

namespace b2dHelper {

	typedef std::pair<b2Fixture*, b2Fixture*> fixturePair;

	b2Vec2 ComputeCentroid(std::vector<b2Vec2> vs, float& area);

	bool inside(b2Vec2 cp1, b2Vec2 cp2, b2Vec2 p);

	b2Vec2 intersection(b2Vec2 cp1, b2Vec2 cp2, b2Vec2 s, b2Vec2 e);

	//http://rosettacode.org/wiki/Sutherland-Hodgman_polygon_clipping#JavaScript
	bool findIntersectionOfFixtures(b2Fixture* fA, b2Fixture* fB, std::vector<b2Vec2>& outputVertices);
}