#include "stdafx.h"
#include "TSpline.h"
#include "Vertex.h"

TSpline::TSpline(const Vertex* v1, const Vertex* v2)://from only bezier
closed(false){
	size_t size = v2->getCountFromBezier();// 10;
	Vec2* segment = CreateSegments(v1->getPos(), v1->getC2(), v2->getPos(), v2->getC1(), size);
	//v1->setType( TypeVertex_None );
	//v2->setType( TypeVertex_None );
	for (size_t i = 0; i <= size; ++i) {
		if (i == 0)
			continue;
		//pushBackSimpleSpline( v1, true );
		else if (i == size)
			continue;
		//pushBackSimpleSpline( v2, true );
		else {
			Vertex* tmp = new Vertex(segment[i]);
			if (v1->isAngleToHead_2())
				tmp->setAngleToHead_2(v1->getAngleToHead_2());
			if (!v1->isCalcTypeFromCrossing())
				tmp->setTypeFromCrossing(v1->getTypeFromCrossing());
			if (v1->getFriction() != 0)
				tmp->setFriction(v1->getFriction());
			pushBackSpline(tmp, false);
		}
	}

	CC_SAFE_DELETE_ARRAY(segment);
}

TSpline::~TSpline() {
	if (NULL != m_data) {
		m_data->release();
		//delete m_data;
	}
	for (Vertex* vertex : control)
		delete vertex;
	control.clear();

	for (auto& it : spline)
		delete it;
	spline.clear();
}

bool TSpline::CalculateSpline(const std::vector<Vec2> &values, std::vector<Vec2> &calcSpline) {
	std::vector<Segment> newSpline;
	calcSpline.clear();

	if (!CalculateSpline(values, newSpline))
		return false;

	Vec2 p;
	for (auto s : newSpline) {
		for (int i = 0; i < RESOLUTION; ++i) {
			s.calc((double)i / (double)RESOLUTION, p);
			calcSpline.push_back(Vec2(p));
		}
	}
	calcSpline.push_back(Vec2(values.back()));
	return  true;
}

bool TSpline::CalculateSpline(const std::vector<Vec2> &values, std::vector<Segment> &bezier) {
	int n = values.size() - 1;

	if (n < 2)
		return false;

	bezier.resize(n);

	Vec2 tgL;
	Vec2 tgR;
	Vec2 cur;
	Vec2 next = values[1] - values[0];
	next.normalize();

	double l1, l2, tmp, x;

	--n;

	for (int i = 0; i < n; ++i) {
		bezier[i].points[0] = bezier[i].points[1] = values[i];
		bezier[i].points[2] = bezier[i].points[3] = values[i + 1];

		cur = next;
		next = values[i + 2] - values[i + 1];
		next.normalize();

		tgL = tgR;

		tgR = cur + next;
		tgR.normalize();

		if (abs(values[i + 1].y - values[i].y) < EPSILON) {
			l1 = l2 = 0.0;
		}
		else {
			tmp = values[i + 1].x - values[i].x;
			l1 = abs(tgL.x) > EPSILON ? tmp / (2.0 * tgL.x) : 1.0;
			l2 = abs(tgR.x) > EPSILON ? tmp / (2.0 * tgR.x) : 1.0;
		}

		if (abs(tgL.x) > EPSILON && abs(tgR.x) > EPSILON) {
			tmp = tgL.y / tgL.x - tgR.y / tgR.x;
			if (abs(tmp) > EPSILON) {
				x = (values[i + 1].y - tgR.y / tgR.x * values[i + 1].x - values[i].y + tgL.y / tgL.x * values[i].x) / tmp;
				if (x > values[i].x && x < values[i + 1].x) {
					if (tgL.y > 0.0) {
						if (l1 > l2)
							l1 = 0.0;
						else
							l2 = 0.0;
					}
					else {
						if (l1 < l2)
							l1 = 0.0;
						else
							l2 = 0.0;
					}
				}
			}
		}

		bezier[i].points[1] += tgL * l1;
		bezier[i].points[2] -= tgR * l2;
	}

	l1 = abs(tgL.x) > EPSILON ? (values[n + 1].x - values[n].x) / (2.0 * tgL.x) : 1.0;

	bezier[n].points[0] = bezier[n].points[1] = values[n];
	bezier[n].points[2] = bezier[n].points[3] = values[n + 1];
	bezier[n].points[1] += tgR * l1;

	return true;
}

void TSpline::recalc() {

	for (Vec2* vec2 : calcSpline)
		delete vec2;
	calcSpline.clear();

	std::vector<Vec2> testValues;
	std::vector<Segment> newSpline;
	Vec2 p;
	for (Vertex* vertex : spline)
		testValues.push_back(vertex->getPos());
	//testValues.push_back(Vec2(0, 0));

	CalculateSpline(testValues, newSpline);

	for (auto s : newSpline) {
		for (int i = 0; i < RESOLUTION; ++i) {
			s.calc((double)i / (double)RESOLUTION, p);
			calcSpline.push_back(new Vec2(p));
		}
	}
	calcSpline.push_back(new Vec2(testValues.back()));
}

void TSpline::pushBackControl(Vertex* vertex, bool update_that_closed /*= true*/) {
	if (NULL == vertex)
		return;

	vertex->setParent(this);

	vertex->setTail(last_tmp);

	control.push_back(vertex);

	last_tmp = vertex;
	if (update_that_closed)
		updateThatClosed();
}

void TSpline::addControl(Vertex* vertex, Vertex* after) {
	if (control.size() == 0)
		return;
	std::vector<Vertex*>::iterator begin = control.begin();
	std::vector<Vertex*>::iterator end = control.end();
	for (std::vector<Vertex*>::iterator it = begin; it != end; ++it) {
		if ((*it) == after) {

			vertex->setTail(*it);

			if ((it + 1) == end) {
				vertex->setHead(*begin);
			}
			else {
				vertex->setHead(*(it + 1));
			}

			control.insert((it + 1), vertex);
			break;
		}
	}
	last_tmp = *(control.end() - 1);

	//ToDo
	//int size = control.size();
	//setClosed( size > 2 );

	//updateThatClosed();
}

void TSpline::eraceControl(Vertex* vertex) {
	std::vector<Vertex*>::iterator end = control.end();
	std::vector<Vertex*>::iterator begin = control.begin();
	std::vector<Vertex*>::iterator it = std::find(control.begin(), end, vertex);

	vertex->setParent(NULL);
	size_t size = control.size();
	if (size > 1 && it != begin) {
		(*(it - 1))->setHead(NULL);
	}
	if (size > 1 && it != end && (it + 1) != end) {
		(*(it + 1))->setTail(NULL);
	}

	if (it != end) {
		control.erase(it);
	}

	if (control.size() == 0) {
		last_tmp = NULL;
		return;
	}

	last_tmp = *(control.end() - 1);
	updateThatClosed();
}

void  TSpline::updateThatClosed(std::vector<Vertex*>& _list, bool closed) {
	size_t size = _list.size();
	if (closed) {
		if (size > 2) {
			std::vector<Vertex* >::iterator begin = _list.begin();
			std::vector<Vertex* >::iterator end = (_list.end() - 1);
			if ((*begin)->getTail() != (*end)) {
				(*begin)->setTail((*end));
				(*end)->setHead((*begin));
			}
		}
	}
	else {
		if (size > 0) {
			std::vector<Vertex* >::iterator begin = _list.begin();
			(*begin)->setTail(NULL);

			if (size > 1) {
				std::vector<Vertex* >::iterator end = (_list.end() - 1);
				(*end)->setHead(NULL);
			}
		}
	}
}

void TSpline::recreteSpline(bool invert) {
	//ToDo
	//int size = control.size();
	//setClosed( size > 2 );

	for (auto& it : spline)
		delete it;
	spline.clear();

	if (invert) {
		std::vector<Vertex*> control2;

		std::vector<Vertex*>::iterator begin = control.begin();
		std::vector<Vertex*>::iterator end = control.end();
		std::vector<Vertex*>::iterator it, next = end;
		for (it = end - 1;; --it) {
			control2.push_back(*it);

			if (it == begin) {
				//(*it)->setHead((*(end-1)));
				break;
			}
			else {
				(*it)->setHead((*(it - 1)));
			}
		}
		control = control2;
	}


	Vertex* t_last = *(control.end() - 1);
	for (auto& it : control) {
		pushBackSpline(it, true, it == t_last);
	}
	updateThatClosed();
	return;
}

void TSpline::pushBackSpline(const Vertex* v, bool bezier, bool last_v/*=false*/) {
	Vertex* new_v = new Vertex(v);
	new_v->setType(TypeVertex_None);

	Vertex* first = spline.size() > 0 ? (*spline.begin()) : NULL;  //X-------->
	Vertex* last = spline.size() > 0 ? (*(spline.end() - 1)) : NULL; //>------X


	if (last && bezier) {
		if (last->isC2() || v->isC1()) {
			TSpline* first_bezierSpline = new TSpline(last, v);
			std::vector<Vertex* > bezierSpline = first_bezierSpline->getSpline();

			last->setHead((*bezierSpline.begin()));
			spline.insert(spline.end(), bezierSpline.begin(), bezierSpline.end());

			last = (*(spline.end() - 1));

			//if (closed)
			//	v->setHead( first );
			//delete first_bezierSpline;
			//return;
		}

	}
	spline.push_back(new_v); //>----->-X

	if (IsClosed()) {
		new_v->setConnections(first, last);    // >-last--->v--->first->
		if (last_v && (new_v->isC2() || first->isC1())) {
			TSpline* first_bezierSpline = new TSpline(new_v, first);
			std::vector<Vertex* > bezierSpline = first_bezierSpline->getSpline();

			new_v->setHead((*bezierSpline.begin()));
			spline.insert(spline.end(), bezierSpline.begin(), bezierSpline.end());


			last = (*(spline.end() - 1));
			Vertex* last2 = spline.size() > 2 ? (*(spline.end() - 2)) : NULL;
			last->setConnections(first, last2);    // >-last--->v--->first->


		}
	}
	else {
		new_v->setConnections(NULL, last);	// >-last--->v--->NULL->
	}

	//__________________________________________________


	// ________________________________________
	/*
	if (last == first && NULL != first)
	{
	if (closed)
	last->setConnections( v, v );
	else
	last->setConnections( v, NULL );
	}
	else
	{
	Vertex* tmp = closed ? v : NULL;

	if (NULL != last)
	last->setHead( tmp );
	if (NULL != first)
	first->setTail( tmp );
	}
	*/
}

Vec2* TSpline::CreateSegments(const Vec2& p1, const Vec2& c1, const Vec2& p2, const Vec2& c2, int count, float* _length_out, bool align) {
	float length_out;
	Vec2* segment = CreateSegments(p1, c1, p2, c2, count, &length_out);
	if (NULL != _length_out)
		*_length_out = length_out;
	if (align) {
		Vec2* align_segment = AliginSegments(segment, &count, length_out);
		CC_SAFE_DELETE_ARRAY(segment);
		return align_segment;
	}
	return segment;
}

Vec2* TSpline::CreateSegments(const Vec2& p1, const Vec2& c1, const Vec2& p2, const Vec2& c2, int count /*= 40*/, float* length_out /*= NULL*/) {
	bool isCalcLength = length_out != NULL;
	Vec2 prev = IONE;
	float retLength = 0;

	Vec2* ret = new (std::nothrow) Vec2[count + 1];

	float t = 0;
	for (unsigned int i = 0; i < count; i++) {
		ret[i].x = powf(1 - t, 3) * p1.x + 3.0f * powf(1 - t, 2) * t * c1.x + 3.0f * (1 - t) * t * t * c2.x + t * t * t * p2.x;
		ret[i].y = powf(1 - t, 3) * p1.y + 3.0f * powf(1 - t, 2) * t * c1.y + 3.0f * (1 - t) * t * t * c2.y + t * t * t * p2.y;
		t += 1.0f / count;
		if (isCalcLength && !ISONE(prev)) {
			retLength += prev.distance(ret[i]);
		}
		prev = ret[i];

	}
	ret[count].x = p2.x;
	ret[count].y = p2.y;
	if (isCalcLength) {
		if (ISONE(prev))
			retLength += p1.distance(ret[count]);
		else
			retLength += prev.distance(ret[count]);
		if (length_out)
			*length_out = retLength;
	}
#if 0
	if (align) {
		Vec2* ret2 = new (std::nothrow) Vec2[count + 1];
		const float _ep = .01;
		float d_len = retLength / (count);
		float len2 = .0;
		Vec2* end = ret + count;
		Vec2* it = ret;
		Vec2* prev = ret;
		int ri = 1;
		ret2[0] = ret[0];
		ret2[count] = ret[count];

		if (isLog) CCLOG("######################");
#define LOG(str,i) if(isLog) CCLOG("%s  %.2f to %.2f  dif(%.2f)",str, ret[i-1].distance(ret[i ]), ret2[i-1].distance(ret2[i ]), abs(d_len - ret2[i-1].distance(ret2[i ])));

		while (it++ != end && ri < count) {
			float distance = (*it).distance(*prev);
			if (len2 + distance >= d_len) {
				Vec2 norm = ((*it) - (*prev)).getNormalized();
				float dtmp = d_len - len2;
				if (dtmp < 0)
					DebugBreak();
				ret2[ri] = (*prev) + norm * dtmp;
				LOG("first", ri);
				ri++;
				len2 = distance - dtmp;
				//##############
				while (len2 >= d_len && ri < count) {
					ret2[ri] = ret2[ri - 1] + norm * d_len;
					if (abs(d_len - ret2[ri - 1].distance(ret2[ri])) > 100.)
						DebugBreak();
					LOG("proc", ri);
					len2 -= d_len;
					ri++;
				}
				//##############
			}
			else if (it == end) {
				ret2[ri] = (*it);
				LOG("end", ri);
				ri++;
			}
			else {
				len2 += distance;
			}
			prev = it;
		}


		CC_SAFE_DELETE_ARRAY(ret);
		return ret2;
	}
#endif

	//after
	//CC_SAFE_DELETE_ARRAY(ret);
	return ret;
}

Vec2* TSpline::AliginSegments(Vec2* segments, int* _count, float lenght_segment, float _d_len /* = -1*/) {

	float d_len = _d_len;
	int count = *_count;
	int new_count = count;
	if (-1 == d_len) {
		d_len = lenght_segment / count;
	}
	else {
		new_count = lenght_segment / d_len + 1;
	}

	Vec2* ret = new (std::nothrow) Vec2[new_count + 1];
	Vec2* end = segments + count;
	Vec2* it = segments;
	Vec2* prev = segments;
	float len_tmp = .0f;
	int ri = 1;
	ret[0] = segments[0];
	ret[new_count] = segments[count - 1];

	while (it++ != end && ri < new_count) {
		float distance = (*it).distance(*prev);

		if (len_tmp + distance >= d_len) {
			Vec2 norm = ((*it) - (*prev)).getNormalized();
			float dtmp = d_len - len_tmp;
#if defined(IS_PC)
			if (dtmp < 0)
				DebugBreak();
#endif
			ret[ri] = (*prev) + norm * dtmp;
			ri++;
			len_tmp = distance - dtmp;
			//##############
			while (len_tmp >= d_len && ri < new_count) {
				ret[ri] = ret[ri - 1] + norm * d_len;
#if defined(IS_PC)
				if (abs(d_len - ret[ri - 1].distance(ret[ri])) > 100.)
					DebugBreak();
#endif
				len_tmp -= d_len;
				ri++;
			}
			//##############
		}
		else if (it == end) {
			//ret2[ri] = (*it);
			ri++;
		}
		else {
			len_tmp += distance;
		}
		prev = it;
	}


	//after 
	//CC_SAFE_DELETE_ARRAY(segments);
	*_count = new_count;
	return ret;
}