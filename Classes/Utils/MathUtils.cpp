#include "stdafx.h"
#include "MathUtils.h"

#if IS_PC || IS_IOS
#define  LOGD(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "Brush"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif


double MathUtils::GetCurrTime() {//in sec
	struct timeval t;
	gettimeofday(&t, NULL);
	//float retMs = (double) t.tv_sec * 1000 + (double) t.tv_usec / 1000.0;
	double retS = (double)t.tv_sec + (double)t.tv_usec / 1000000.0;
	//return (float) utils::getTimeInMilliseconds()
	//LOGD("rets = %.2f", (float) retS);
	return retS;

	/*
	struct timeval now;
	if (gettimeofday(&now, nullptr)!=0)
	{
		CCLOG("error in gettimeofday");
		return 0;
	}
	return (now.tv_sec)+(now.tv_usec)/1000000.0f;
	*/
}

bool MathUtils::IsOneDimensionSegmentOverlap(float A, float B, float C, float D, float *S, float *E) {
	float ABmin = std::min(A, B);
	float ABmax = std::max(A, B);
	float CDmin = std::min(C, D);
	float CDmax = std::max(C, D);

	if (ABmax < CDmin || CDmax < ABmin) {
		// ABmin->ABmax->CDmin->CDmax or CDmin->CDmax->ABmin->ABmax
		return false;
	}
	else {
		if (ABmin >= CDmin && ABmin <= CDmax) {
			// CDmin->ABmin->CDmax->ABmax or CDmin->ABmin->ABmax->CDmax
			if (S != nullptr) *S = ABmin;
			if (E != nullptr) *E = CDmax < ABmax ? CDmax : ABmax;
		}
		else if (ABmax >= CDmin && ABmax <= CDmax) {
			// ABmin->CDmin->ABmax->CDmax
			if (S != nullptr) *S = CDmin;
			if (E != nullptr) *E = ABmax;
		}
		else {
			// ABmin->CDmin->CDmax->ABmax
			if (S != nullptr) *S = CDmin;
			if (E != nullptr) *E = CDmax;
		}
		return true;
	}
}

float MathUtils::CrossProduct2Vector(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D) {
	return  (B.x - A.x) * (D.y - C.y) - (D.x - C.x) * (B.y - A.y);
}

Vec2 MathUtils::GetSizeScale(Node* node, bool scale) {
	Vec2 ret = Vec2(ABS(node->getContentSize().width *
		(scale ? node->getScaleX() : 1)),
					ABS(node->getContentSize().height *
					(scale ? node->getScaleY() : 1)));
	return ret;
}


Vec2 MathUtils::GetPositionC(Node* node) {
	Vec2 sizeS = GetSizeScale(node, true);
	Vec2 pos = node->getPosition();
	Vec2 anchor = node->getAnchorPoint();
	float angle = node->getRotation();

	Vec2 toC = sizeS; toC.scale(Vec2(0.5f, 0.5f) - anchor);
	toC = (pos + toC).rotateByAngle(pos, -MATH_DEG_TO_RAD(angle));
	return toC;
}

Vec2 MathUtils::GetPositionLB(Node* node, Vec2* posCB, Vec2* posRB) {
	Vec2 sizeS = GetSizeScale(node, true);
	float angle = node->getRotation();
	Vec2 anchor = node->getAnchorPoint();
	Vec2 pos = node->getPosition();

	return GetPositionLB(pos, sizeS, anchor, angle, posCB, posRB);
}

Vec2 MathUtils::GetPositionLT(Vec2 pos, Vec2 sizeS, Vec2 anchor, float angle, Vec2* posCT, Vec2* posRT) {
	Vec2 posLT = sizeS; posLT.scale(anchor); posLT = pos - posLT;
	posLT.y += sizeS.y;
	if (NULL != posRT) {
		*posRT = sizeS; (*posRT).scale(Vec2(1, 0)); *posRT = posLT + *posRT;
		*posRT = (*posRT).rotateByAngle(pos, angle);
	}
	if (NULL != posCT) {
		*posCT = sizeS; (*posCT).scale(Vec2(0.5, 0)); *posCT = posLT + *posCT;
		*posCT = (*posCT).rotateByAngle(pos, angle);
	}
	posLT = posLT.rotateByAngle(pos, angle);
	return posLT;
}

Vec2 MathUtils::GetPositionLB(Vec2 pos, Vec2 sizeS, Vec2 anchor, float angle, Vec2* posCB, Vec2* posRB) {
	Vec2 posLB = sizeS; posLB.scale(anchor); posLB = pos - posLB;
	if (NULL != posRB) {
		*posRB = sizeS; (*posRB).scale(Vec2(1, 0)); *posRB = posLB + *posRB;
		*posRB = (*posRB).rotateByAngle(pos, angle);
	}
	if (NULL != posCB) {
		*posCB = sizeS; (*posCB).scale(Vec2(0.5, 0)); *posCB = posLB + *posCB;
		*posCB = (*posCB).rotateByAngle(pos, angle);
	}
	posLB = posLB.rotateByAngle(pos, angle);
	return posLB;
}

Vec2 MathUtils::SetAnchorFromSavePosition(Vec2 prevPos, Vec2 prevAnchor, Vec2 sizeS, float angle, Vec2 newAnchor) {
	Vec2 dA = prevAnchor - newAnchor;
	Vec2 newPosition = prevPos - sizeS;
	newPosition = newPosition.rotateByAngle(prevPos, angle);
	return newPosition;
}

Vec2 MathUtils::SetAnchorFromSavePosition(Node* node, Vec2 newAnchor) {
	Vec2 prevAnchor = node->getAnchorPoint();
	Vec2 prevPosition = node->getPosition();

	if (prevAnchor == newAnchor)
		return prevPosition;

	Vec2 dA = prevAnchor - newAnchor;

	Vec2 dSize = MathUtils::GetSizeScale(node, true);
	dSize.scale(dA);

	Vec2 newPosition = prevPosition - dSize;
	newPosition = newPosition.rotateByAngle(prevPosition, CC_DEGREES_TO_RADIANS(-node->getRotation()));

	node->setAnchorPoint(newAnchor);
	node->setPosition(newPosition);
	return newPosition;
}

Vec2 MathUtils::ConvertToFirstSpace_ignoreRS(Node* node, const Vec2& nodePoint) {
	//ToDo
	Vec3 ret;
	if (Director::getInstance()->getRunningScene() == NULL) {
		DebugBreak();
	}
	else {
		Node* ancestor = Director::getInstance()->getRunningScene()->getChildByTag(TAG_LAYER_1);
		Mat4 tmp = node->getNodeToParentTransform(ancestor);
		Vec3 vec3(nodePoint.x, nodePoint.y, 0);
		tmp.transformPoint(vec3, &ret);
	}
	return Vec2(ret.x, ret.y);
}

Mat4 MathUtils::GetNodeToParentTransform_ignoreRS(Node* node) {
	Node* ancestor = Director::getInstance()->getRunningScene()->getChildByTag(TAG_LAYER_1);
	Mat4 ret = node->getNodeToParentTransform(ancestor);
	return ret;
}


Vec2 MathUtils::ConvertToParent(Node* node, const Vec2& worldPoint) {
	Vec3 ret;
	Mat4 tmp = node->getNodeToParentTransform(node->getParent()).getInversed();
	Vec3 vec3(worldPoint.x, worldPoint.y, 0);
	tmp.transformPoint(vec3, &ret);
	return Vec2(ret.x, ret.y);
}

Vec2 MathUtils::ConvertToNodeSpace_ignoreRS(Node* node, const Vec2& worldPoint) {
	//ToDo
	Vec3 ret;
	if (Director::getInstance()->getRunningScene() == NULL) {
		//DebugBreak();
	}
	else {
		Node* ancestor = Director::getInstance()->getRunningScene()->getChildByTag(TAG_LAYER_1);
		Mat4 tmp = node->getNodeToParentTransform(ancestor).getInversed();
		Vec3 vec3(worldPoint.x, worldPoint.y, 0);
		tmp.transformPoint(vec3, &ret);
	}
	return Vec2(ret.x, ret.y);
}

bool MathUtils::IsLineIntersect(Vec2 A, Vec2 B, Vec2 C, Vec2 D, Vec2* cross) {
	float S, T;
	bool isIntersect(false);

	if (Vec2::isLineIntersect(A, B, C, D, &S, &T) &&
		(S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f)) {
		isIntersect = true;
	}
	if (isIntersect && NULL != cross) {
		(*cross).setZero();
		(*cross).x = A.x + S * (B.x - A.x);
		(*cross).y = A.y + S * (B.y - A.y);
	}
	return isIntersect;
}

void MathUtils::Swap(Vec2* p1, Vec2* p2) {
	Vec2 tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}

bool MathUtils::AnywayIntersection(Vec2 top, Vec2 bottom, Vec2 left, Vec2 right, Vec2* dBottom, Vec2* intersect, DrawNode* node) {
	Vec2 normal = (top - bottom).getNormalized();
	Vec2 n1 = bottom + normal * EVER_INTERSECTION_D;
	Vec2 n2 = bottom + normal * -EVER_INTERSECTION_D;
	Vec2 virtualShape = (left - right).getNormalized() * EVER_INTERSECTION_D;
	Vec2 intersect_tmp;
	bool isIntersect = IsLineIntersect(right + virtualShape, left - virtualShape, n1, n2, &intersect_tmp);
	if (!isIntersect) {
		DebugBreak();
	}
	if (NULL != dBottom)
		*dBottom = bottom - intersect_tmp;
	if (NULL != intersect)
		*intersect = intersect_tmp;
	if (NULL != node)
		//if (false)
	{
		node->drawLine(right + virtualShape, left - virtualShape, Color4F::WHITE);
		node->drawLine(n1, n2, Color4F::WHITE);
	}
	return isIntersect;
}

//##############################################################################

float MathUtils::CubicInterpolate(float elapsed, float change, float duration) {
	float tmp = elapsed / duration;
	auto ts = tmp * tmp;
	auto tc = ts * tmp;
	return change * (tc + -3 * ts + 3 * tmp);
}

Vec2 MathUtils::CubicInterpolate(float elapsed, Vec2 change, float duration) {
	Vec2 ret;
	ret.x = CubicInterpolate(elapsed, change.x, duration);
	ret.y = CubicInterpolate(elapsed, change.y, duration);
	return ret;
}

//##############################################################################

//   from tested
Vec2 positionOfAnchorPoint(Node* node, Vec2 anchor) {
	float x = anchor.x * node->getContentSize().width;
	float y = anchor.y * node->getContentSize().height;

	Vec2 pos = Vec2(x, y);

	pos = PointApplyAffineTransform(pos, node->getNodeToParentAffineTransform());

	return  pos * 1 / CC_CONTENT_SCALE_FACTOR();
}

Vec2 positionOfAnchorPointInPoints(Node* node, Vec2 anchor) {
	Vec2 anchorPointInPercent = Vec2(anchor.x / node->getContentSize().width, anchor.y / node->getContentSize().height);
	return positionOfAnchorPoint(node, anchorPointInPercent);
}

Vec2 MathUtils::SetAnchorFromSavePosition_2(Node* node, Vec2 newAnchorPoint) {
	Vec2 tempPos = positionOfAnchorPoint(node, newAnchorPoint);
	node->setAnchorPoint(newAnchorPoint);
	node->setPosition(tempPos);
	return tempPos;
}
