#pragma once
#include "stdafx.h"


#define COLOR_START color_circle_start


#define COLOR_YES color_yes
#define COLOR_NO Color3B::RED
#define COLOR_HELPER Color3B::BLACK


#if IS_PC
#define  LOGD(...) CCLOG(__VA_ARGS__)
#else
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "Brush"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif

struct Brush {

	enum BrushType {
		NONE = -1,
		NO,
		YES,
		FINISH,
		FINISH_IN,
		FINISH_OUT,
		HELPER,
		HIDE
	};


	BrushType m_type = BrushType::NONE;

	Vec2 m_pos;

	Sprite* m_sprite;
	const Vec2& getCurrPos() { return m_sprite->getPosition(); }

	float m_def_opacity = 1;
	float m_duration = 1.f;
	double m_elapsed = -1.;

	float m_amplitude = 20.f;
	float m_da;

	Vec2 m_to_pos;
	float m_length_way = -1;


	DrawToDots* m_parent_spline;

	//todo 
	Brush(Brush* brush) {
		m_parent_spline = brush->m_parent_spline;
		m_pos = brush->m_pos;

		m_sprite = Sprite::createWithTexture(brush->m_sprite->getTexture());
		m_sprite->retain();

		m_sprite->setPosition(brush->m_sprite->getPosition());
		m_sprite->setScale(brush->m_sprite->getScale());
		setOpacity(brush->m_def_opacity);

		setType(brush->m_type);

		sNew = brush->sNew;
	}

	Brush(DrawToDots* parent_spline, Sprite* brush, const Vec2& pos) {
		m_parent_spline = parent_spline;
		m_pos = pos;
		m_sprite = Sprite::createWithTexture(brush->getTexture());
		m_sprite->retain();
		setOpacity(brush->getOpacity());
		m_sprite->setPosition(m_pos);
		m_sprite->setScale(brush->getScale());

		setType(NO);
	}

	void setOpacity(float opacity) {
		m_def_opacity = opacity;
		m_sprite->setOpacity(m_def_opacity);
	}

	~Brush() {
		if (NULL == m_sprite) return;
		m_sprite->removeFromParent();
		m_sprite->release();
	}

	bool iStarted = false;

	bool hasStartToEnd() { return iStarted; }

	void reset() {
		iStarted = false;
	}

	void toEnd(float delay, float duration) {
		if (iStarted) return;
		iStarted = true;

		m_duration = duration;
		m_elapsed = -delay;
	}

	bool isEnd = false;

	void toEnd(const Vec2& pos, float duration) {
		iStarted = true;
		isEnd = false;
		m_elapsed = 0;
		m_to_pos = pos;
		m_duration = duration;

		m_length_way = m_to_pos.distance(m_pos);
	}

	void setType(BrushType type) {
		if (m_type == type)return;
		m_type = type;
		switch (m_type) {
			case Brush::NO:
				m_sprite->setColor(COLOR_NO);
				break;
			case Brush::YES:
			case Brush::FINISH:
			case Brush::FINISH_IN:
			case Brush::FINISH_OUT:
				m_sprite->setColor(m_parent_spline->getColorYes());
				break;
			case Brush::HELPER:
				m_sprite->setColor(COLOR_HELPER);
				m_sprite->setOpacity(0);
				m_sprite->setScale(m_sprite->getScale()*.6);
				break;
			case Brush::HIDE:
				m_def_opacity = m_sprite->getOpacity();
				break;
		}
	}

	void setAmplitude(float amplitude, float da) {
		m_amplitude = amplitude;
		m_da = da;
	}

	bool sNew = true;
	bool isNew() { return sNew; }


	void update(float delay) {
		sNew = false;
		switch (m_type) {
			case Brush::NO:
			case Brush::YES:
			{
				if (!iStarted) return;
				if (isEnd) return;

				m_elapsed += delay;
				if (m_elapsed < 0.f)
					return;
				float percent = MIN(m_elapsed / m_duration, 1.f);
				if (percent >= 1.f)
					isEnd = true;

				percent = 1 - percent;

				float t_percent = Easing::LinearInterpolation(percent);
				float newO = m_def_opacity * t_percent;

				if (m_sprite->getOpacity() != newO)
					m_sprite->setOpacity(newO);
			}
			break;
			case Brush::FINISH:
			{
				m_sprite->setOpacity(m_def_opacity);
				if (isEnd) return;

				m_elapsed += delay;
				float percent = MIN(m_elapsed / m_duration, 1.f);

				if (percent >= 1.f)
					isEnd = true;

				//float percentR = Easing::QuadraticEaseIn(percent);
				float percentR = Easing::LinearInterpolation(percent);
				//float percentR = Easing::ElasticEaseOut(percent);
				//float percentR = Easing::BounceEaseInOut(percent);
				//float percentR = Easing::CircularEaseInOut(percent);
				//float percentR = Easing::ExponentialEaseInOut(percent);
				Vec2 nPos = m_pos + (m_to_pos - m_pos).getNormalized() * (m_length_way * percentR);
				m_sprite->setPosition(nPos);
				return;
			}break;
			case Brush::FINISH_IN:
			{
				m_sprite->setOpacity(255.);
				if (isEnd) return;
				m_elapsed += delay;
				float percent = MIN(m_elapsed / m_duration, 1.f);
				if (percent >= 1.f)isEnd = true;

				float percentR = Easing::LinearInterpolation(percent);
				Vec2 nPos = m_pos + (m_to_pos - m_pos).getNormalized() * (m_length_way * percentR);
				m_sprite->setPosition(nPos);
				m_sprite->setOpacity(percentR * 255.);
			}break;
			case Brush::FINISH_OUT:
			{
				m_sprite->setOpacity(.0);
				if (isEnd) return;
				m_elapsed += delay;
				float percent = MIN(m_elapsed / m_duration, 1.f);
				if (percent >= 1.f)isEnd = true;

				float percentR = Easing::LinearInterpolation(percent);
				Vec2 nPos = m_pos + (m_to_pos - m_pos).getNormalized() * (m_length_way * percentR);
				m_sprite->setPosition(nPos);
				m_sprite->setOpacity((1. - percentR) * 255.);
			}break;
			case Brush::HELPER:
			{
				//m_sprite->setOpacity(255);
				m_elapsed += delay;
				if (m_elapsed < .0)return;
				float percent = MIN(m_elapsed / m_duration, 1.f);
				if (percent >= 1.f) isEnd = true;
				//float t_sin = (1.f + sin(m_elapsed * m_amplitude))*.5f;
				float t_sin = MAX(.0, sin(m_elapsed  * m_amplitude + m_da));
				m_sprite->setOpacity(255 * t_sin);

				return;
			}break;
			case Brush::HIDE:
			{
				m_elapsed += delay;
				if (m_elapsed < .0)return;
				float percent = MIN(m_elapsed / m_duration, 1.f);
				if (percent >= 1.f) isEnd = true;

				float old_opacity = m_sprite->getOpacity();
				if (old_opacity <= .0) {
					isEnd = true;
				}
				else {
					float new_opacity = MAX(0, m_def_opacity - (255 * percent));
					m_sprite->setOpacity(new_opacity);
				}
			}
		}
	}

	//если isEnd то в начале обработки все кисти удалятся !!!!!
	void visit() {
		m_sprite->visit();
	}
};