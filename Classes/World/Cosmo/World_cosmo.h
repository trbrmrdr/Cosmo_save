#pragma once
#include "stdafx.h"
#include "../World.h"
#include "DrawToDots.h"

#include "Scenes/DevelopScene.h"
#include "WorldSpline/WorldSpline.h"
#include "PlanetSpline.h"

class World_cosmo : public World {
private:	
	PlanetSpline* m_cosmoSpline;
	
public:

	World_cosmo(Layer* layer);

	virtual ~World_cosmo(void);

	virtual void update(float delta);
	virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;

};