#pragma once

#include "cocos2d.h"
USING_NS_CC;

#define EVER_INTERSECTION_D 10000

namespace MathUtils
{
	double GetCurrTime();

	bool IsOneDimensionSegmentOverlap(float A, float B, float C, float D, float *S, float *E);

	float CrossProduct2Vector(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D);

	Vec2 GetSizeScale(Node* node, bool scale);

	Vec2 GetPositionC(Node* node);

	Vec2 GetPositionLB(Node* node, Vec2* posCB = NULL, Vec2* posRB = NULL);

	Vec2 GetPositionLB(Vec2 pos, Vec2 sizeS, Vec2 anchor, float angle, Vec2* posCB = NULL, Vec2* posRB = NULL);

	Vec2 GetPositionLT(Vec2 pos, Vec2 sizeS, Vec2 anchor, float angle, Vec2* posCT = NULL, Vec2* posRT = NULL);

	Vec2 SetAnchorFromSavePosition(Node* node, Vec2 newAnchor);

	Vec2 SetAnchorFromSavePosition(Vec2 prevPos, Vec2 prevAnchor, Vec2 sizeS, float angle, Vec2 newAnchor);

	Mat4 GetNodeToParentTransform_ignoreRS(Node* node);

	Vec2 ConvertToFirstSpace_ignoreRS(Node* node, const Vec2& nodePoint);

	Vec2 ConvertToParent(Node* node, const Vec2& worldPoint);

	Vec2 ConvertToNodeSpace_ignoreRS(Node* node, const Vec2& worldPoint);

	bool IsLineIntersect(Vec2 A, Vec2 B, Vec2 C, Vec2 D, Vec2* cross = NULL);

	void Swap(Vec2* p1, Vec2* p2);

	bool AnywayIntersection(Vec2 top, Vec2 bottom, Vec2 left, Vec2 right, Vec2* dBottom = NULL, Vec2* intersect = NULL, DrawNode* node = NULL);
	//#################################################################################

	float CubicInterpolate(float elapsed, float change, float duration);

	Vec2 CubicInterpolate(float elapsed, Vec2 change, float duration);




	Vec2 SetAnchorFromSavePosition_2(Node* node, Vec2 newAnchor);
}
