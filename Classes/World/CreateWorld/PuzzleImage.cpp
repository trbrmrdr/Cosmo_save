#include "stdafx.h"
#include "PuzzleImage.h"
#include "DevelopMenu.h"
#include "Effects/InvertDrawEffect.h"
#include "../UI_canvas.h"

#define SCALE_IMAGE (.1f)

#define PYZZ_D (.285f)
#define COUNT_L (5.f)

//#define PYZZLE 1//simple
#define PYZZLE 2//is cuts

DrawNode* drawNode = NULL;

struct Puzzle {
	Sprite* _image;
	Node* _parent;
	float _delay_action;

	Vec2 _pos_start;
	Vec2 _pos_end;

public:
	Puzzle(Node* parent, Sprite* parent_img, Vec2 ids):
		_parent(parent) {
		_image = parent_img;

		Vec2 t_size = _image->getContentSize()* _image->getScale();
		_pos_end = _image->getPosition();

		_pos_start = t_size * 10.;
		_pos_start = _pos_start.rotateByAngle(_pos_end, RandomHelper::random_real(.0, 360.));

		_parent->addChild(_image);
		_parent->setLocalZOrder(_image->getLocalZOrder());

		_image->setOpacity(.0);

		_delay_action = runAction();
	}

	float runAction() {
		_image->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

		_image->setPosition(_pos_start);

		_image->setRotation(RandomHelper::random_real(.0, 360.));

		float duration = .5 + RandomHelper::random_real(.0, 1.);
		float delay = RandomHelper::random_real(.0, 1.);

		_image->runAction(Sequence::create(
			//DelayTime::create(delay),
			FadeIn::create(.0),
			Spawn::create(
				MoveTo::create(duration, _pos_end),
				RotateTo::create(duration, .0), nullptr),
			nullptr));

		//return delay + duration;
		return duration;
	}

	void runOpacity() {
		_image->runAction(FadeOut::create(.1f));
	}

	~Puzzle() {
		_image->removeFromParent();
	}
};

PuzzleImage::PuzzleImage(Node* parent, Rect rect_pos):
	_delay_to_start(-1.),
	_not_play(false),
	_parent(parent) {


	//DEVELOP_ADD_S_POINT(0, DevelopTabs_Settings, "_pos", _pos);
	//DEVELOP_ADD_S_POINT(0, DevelopTabs_Settings, "_size", _size);

	_parent->addChild(_image = Sprite::create());
	_parent->addChild(_image_next = Sprite::create());

	_size = rect_pos.size;
	_pos = rect_pos.origin;// +rect_pos.size*.5;
	//_scale = scale;
	_z_order = UI_canvas::_canvasZOrder - 1;
	_image->setLocalZOrder(_z_order);
	_image_next->setLocalZOrder(_z_order);
}

PuzzleImage::~PuzzleImage() {
	_image->removeFromParent();
	_image_next->removeFromParent();
	for (auto& it : _puzzles)delete it;
}

void PuzzleImage::setImage(const std::string& name) {
	_not_play = 0 == name.compare(_new_name);
	if (_not_play) return;
	_new_name = name;

	CC_SWAP(_image, _image_next, Sprite*);

	_image_next->initWithFile(_new_name);
	_scale = _size.width / _image_next->getContentSize().width;
	_image_next->setScale(_scale);
	_image_next->setPosition(_size * .5);

	_image_next->setOpacity(0.);

	for (auto& it : _puzzles)delete it;
	_puzzles.clear();
}


void PuzzleImage::play_anim(float delay) {
	if (_not_play)return;
	_delay_to_start = delay;
	_delay_to_start = .1;

	_image->setOpacity(.0);
	_image_next->setOpacity(.0);
}

void PuzzleImage::hide() {
	//_image->runAction(FadeOut::create(.3f));
	//_image_next->runAction(FadeOut::create(.3f));
	_delay_to_start = -1.f;
	_image_duration_action = -1.f;
	_new_name.clear();
}

void PuzzleImage::update(float delta) {
	if (_image_duration_action > .0 && (_image_duration_action -= delta) <= .0) {
		_image_duration_action = -1.;
		_image->runAction(FadeOut::create(_image_fadein));
		_image_next->runAction(FadeIn::create(_image_fadein));

	}
	///__________
	if (_image_next->getOpacity() == 255. && _image_duration_action == -1.) {
		_image_duration_action = -2.;
		for (auto& it : _puzzles)it->runOpacity();
	}
	//____________
	if (_delay_to_start > .0) {
		_delay_to_start -= delta;
		if (_delay_to_start <= .0) {
			_delay_to_start = -1.;

#if 1
			Vec2 t_size = _image_next->getContentSize()*_scale;_image->getScale();
			Vec2 t_size2 = t_size * .5;
			Vec2 t_ds = t_size / COUNT_L;
			Vec2 t_pos = _image_next->getPosition();

			Vec2 t_size_s = _image_next->getContentSize() *_scale;// *SCALE_IMAGE;
			Vec2 t_size2_s = t_size_s * .5;
			Vec2 t_ds_s = t_size_s / COUNT_L;

			_image_duration_action = .0;
			for (int i_x = 0; i_x < COUNT_L; i_x++) {
				Vec2 t_i_pos = Vec2(i_x*t_ds.x, .0);
				for (int i_y = 0; i_y < COUNT_L; i_y++) {
					t_i_pos.y = i_y * t_ds.y;

					std::string name = _new_name;
#if PYZZLE == 1
					Vec2 t_dr_fpz = t_ds * PYZZ_D;

					t_dr_fpz = Vec2(.0, .0);

					Sprite* sprite = Sprite::create(name, Rect(t_i_pos.x - t_dr_fpz.x,
															   t_i_pos.y - t_dr_fpz.y,
															   t_ds.x + t_dr_fpz.x*2.,
															   t_ds.y + t_dr_fpz.y * 2.
					));

#else
					std::string dir = name.substr(0, name.find("/") + 1);
					Sprite* sprite = Sprite::create(dir + StringUtils::format("%i_%i.png", i_x, i_y));
#endif
					if (!sprite)continue;
					sprite->setScale(_scale);// SCALE_IMAGE);

					Vec2 inc_pos = t_i_pos;// *SCALE_IMAGE;
					inc_pos = Vec2(inc_pos.x, -inc_pos.y) + Vec2(t_ds_s.x*.5, -t_ds_s.y*.5);
					Vec2 sprite_pos = t_pos - Vec2(t_size2_s.x, -t_size2_s.y) + inc_pos;

					sprite->setPosition(sprite_pos);
					sprite->setLocalZOrder(_z_order);
					Puzzle* puzzle = new Puzzle(_parent, sprite, Vec2(i_x, i_y));

					_image_duration_action = MAX(_image_duration_action, puzzle->_delay_action);

					_puzzles.push_back(puzzle);
				}
			}
#else
			float max_duration_action = .0;
			for (auto& it : _puzzles) {
				max_duration_action = MAX(max_duration_action, it->runAction());
			}
#endif

			_image_duration_action -= _image_fadein;
		}
	}
}

void PuzzleImage::draw() {
	return;
	if (drawNode)
		drawNode->clear();
	else
		_parent->addChild(drawNode = DrawNode::create(), 100000);

	//drawNode->drawRect(_pos, _pos + _size, Color4F::RED);


	Vec2 t_size = _image->getContentSize()*_image->getScale();
	Vec2 t_size2 = t_size * .5;
	Vec2 t_ds = t_size / COUNT_L;
	Vec2 t_pos = _image->getPosition();

	drawNode->drawRect(t_pos - t_size2, t_pos + t_size2, Color4F::GREEN);


	for (int i = 1; i < COUNT_L; i++) {
		drawNode->drawLine(t_pos - t_size2 + i * Vec2(t_ds.x, .0),
						   t_pos - t_size2 + i * Vec2(t_ds.x, .0) + Vec2(.0, t_size.y), Color4F::BLUE);
		drawNode->drawLine(t_pos - t_size2 + i * Vec2(.0, t_ds.y),
						   t_pos - t_size2 + i * Vec2(.0, t_ds.y) + Vec2(t_size.x, .0), Color4F::BLUE);
	}


}