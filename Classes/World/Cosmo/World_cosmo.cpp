#include "stdafx.h"
#include "World_cosmo.h"

//#define WORLD_PTP_FILE_NAME "world_paint_to_point.xml" 
#define LEVELS_PTP_FILE_NAME "ptp_levels.txt"

#define BUTTON_MENU "BUTTON_MENU"
#define BUTTON_CLOSE "BUTTON_CLOSE"
#define BUTTON_LEFT "BUTTON_LEFT"
#define BUTTON_RIGHT "BUTTON_RIGHT"

//#########################################

World_cosmo::World_cosmo(Layer* layer):
	World(layer) {

	m_cosmoSpline = new PlanetSpline(layer);
	_worldSpline = m_cosmoSpline;
}

World_cosmo::~World_cosmo(void) {
	m_cosmoSpline->save();
	delete m_cosmoSpline;
}

void World_cosmo::update(float delta) {
	World::update(delta);
}

void World_cosmo::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) {
	//World::draw(renderer, transform, flags);

	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);

	_worldSpline->draw();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	

}