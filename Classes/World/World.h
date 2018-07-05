#pragma once
#include "stdafx.h"
#include "WorldSpline/WorldSpline.h"

class World
{
private:
	Node* m_include_node;
protected:

	Layer* _layer;
	WorldSpline* _worldSpline;

public:

	World( Layer* layer );
	virtual ~World( void );

	virtual void update( float delta );
	virtual void draw( cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags );

	virtual bool touchBegan( Touch*, Event* ) { return false; }
	virtual void touchMoved( Touch*, Event* ) { return; }
	virtual void touchEnded( Touch*, Event* ) { return; }
};