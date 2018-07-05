#include "stdafx.h"
#include "World.h"
#include "Scenes/DevelopScene.h"
#include "WorldSpline/WorldSpline.h"


World::World( Layer* layer )
{
	_layer = layer;
	m_include_node = Node::create();
	_layer->addChild( m_include_node );

	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = ([this]( Touch* touch, Event* event )
	{
		Vec2 pos = m_include_node->convertToNodeSpace( touch->getLocation() );
		if (NULL != _worldSpline && _worldSpline->mouseDown( pos )) return true;
		return false;
	});

	listener->onTouchMoved = ([this]( Touch* touch, Event* event )
	{
		Vec2 pos = m_include_node->convertToNodeSpace( touch->getLocation() );
		if (NULL != _worldSpline && _worldSpline->mouseMovie( pos )) return;
		return;
	});

	listener->onTouchEnded = ([this]( Touch* touch, Event* event )
	{
		Vec2 pos = m_include_node->convertToNodeSpace( touch->getLocation() );
		if (NULL != _worldSpline && _worldSpline->mouseUp( pos )) return;
		return;
	});

	m_include_node->getEventDispatcher()->addEventListenerWithSceneGraphPriority( listener, m_include_node );

}

World::~World( void )
{
	_layer = NULL;
	CC_SAFE_DELETE( _worldSpline );
	CCLOG( "~end world" );
}

void World::update( float delta )
{
	if (NULL != _worldSpline)
	{
		_worldSpline->update( delta );
		_worldSpline->setScaleFromDraw( 1. / _layer->getScale() );
	}
}

#pragma warning (push)
#pragma warning (disable:4996)
void World::draw( cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags )
{
	Director* director = Director::getInstance();
	director->pushMatrix( MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW );
	director->loadMatrix( MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform );
#if HAS_DEVELOP
	bool is_debug = DevelopMenu::Instance().isEnable();

	if (is_debug)
	{
		CHECK_GL_ERROR_DEBUG();
		CALL_VOID_METHOD( _worldSpline, draw() );
	}
#endif

	director->popMatrix( MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW );
}

#pragma warning(pop)
