#pragma once
#include "stdafx.h"
#include "Vertex.h"

#define EPSILON 1.0e-5
#define RESOLUTION 32

class Segment {
public:
	Vec2 points[4];

	void calc(double t, Vec2 &p) {
		double t2 = t * t;
		double t3 = t2 * t;
		double nt = 1.0 - t;
		double nt2 = nt * nt;
		double nt3 = nt2 * nt;
		p.x = nt3 * points[0].x + 3.0 * t * nt2 * points[1].x + 3.0 * t2 * nt * points[2].x + t3 * points[3].x;
		p.y = nt3 * points[0].y + 3.0 * t * nt2 * points[1].y + 3.0 * t2 * nt * points[2].y + t3 * points[3].y;
	};
};

class TSpline {
private:
	Ref * m_data = NULL;
	bool closed;
	std::vector<Vertex*> spline;
	std::vector<Vertex*> control;


public:
	std::vector<Vec2*> calcSpline;

	TSpline(Vertex* vertex) {
		closed = false;
		pushBackControl(vertex);
	};

	TSpline():closed(false) {};


	TSpline(const Vertex* v1, const Vertex* v2);
	~TSpline();

	void recalc();

	Vertex* last_tmp = NULL;
	void pushBackControl(Vertex* vertex, bool update_that_closed = true);

	void addControl(Vertex* vertex, Vertex* after);

	void eraceControl(Vertex* vertex);

	std::vector<Vertex*> getControl() { return control; }

	std::vector<Vertex*> getSpline() { return spline; }

	bool IsClosed() { return closed; }

	void setClosed(bool _closed, bool update_that_closed = true) {
		if (closed == _closed)
			return;
		closed = _closed;
		if (update_that_closed)
			updateThatClosed();
	}

	void recreteSpline(bool invert = false);


	static Vec2* CreateSegments(const Vec2& p1, const Vec2& c1, const Vec2& p2, const Vec2& c2, int count, float* length_segment, bool align);
	static Vec2* CreateSegments(const Vec2& p1, const Vec2& c1, const Vec2& p2, const Vec2& c2, int count = 40, float* length_segment = NULL);
	static Vec2* AliginSegments(Vec2* segments, int* count, float lenght_segment, float _d_len = -1);

	static bool CalculateSpline(const std::vector<Vec2> &values, std::vector<Vec2> &calcSpline);
	static bool CalculateSpline(const std::vector<Vec2> &values, std::vector<Segment> &bezier);


	void* getData() { return m_data; }
	Ref* setData(Ref* data) {
		m_data = data;
		//m_data->retain(); 
		return m_data;
	}
private:

	void pushBackSpline(const Vertex* v, bool bezier, bool last_v = false);

	void updateThatClosed() {
		//updateThatClosed( spline, closed );
		updateThatClosed(control, closed);
	}

	static void  updateThatClosed(std::vector<Vertex*>& _list, bool closed);
};