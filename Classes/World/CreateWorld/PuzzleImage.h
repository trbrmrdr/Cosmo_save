#pragma once
#include "stdafx.h"
#include "UI/LayoutRefresh.h"
#include "Effects/EffectSprite.h"
#include "LightSystem/LightSystem.h"
#include "World/DrawToDots/DrawToDots.h"

class Puzzle;

class PuzzleImage {
public:
	PuzzleImage(Node* parent, Rect rect_pos);
	virtual ~PuzzleImage();

	void setImage(const std::string& name);

	void update(float delta);

	void draw();

	void hide();

	void play_anim(float delay);
private:
	Node * _parent;
	bool _not_play;
	float _delay_to_start;
	std::string _new_name;
	float _image_duration_action;

	float _image_fadein = .2f;
	Vec2 _pos;
	Size _size;
	int _z_order;
	float _scale;
	Sprite* _image;
	Sprite* _image_next;

	std::vector<Puzzle*> _puzzles;
};