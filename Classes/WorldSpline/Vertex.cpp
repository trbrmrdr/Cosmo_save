#include "stdafx.h"
#include "Vertex.h"

#define WCROSS 20
#define WCROSS2 40
#define RV 5.2f

bool Vertex::_IsAngle = false;

Vertex::~Vertex(){
    deleteControl();

	if (!m_data) {
		delete m_data;
	}
    //ToDo remove
    //#######
    if(node != NULL){
        node->removeFromParent();
        node = NULL;
    }
}

void Vertex::setScale(float scale){
	_scale = scale;

	wCros = WCROSS*_scale;
	wCros2 = WCROSS2*_scale;
	//R = RV*_scale;
	R = ( 20.5f )*(_scale);
	if(c1) c1->setScale(_scale);
	if(c2) c2->setScale(_scale);
}

bool Vertex::MouseDown(const Vec2& mousePos){
	if(isContains(mousePos)){
		_isAllocated = true;
		_lastPos = _pos - mousePos;
		return true;
	}
	else{
		if(isC1() && c1->MouseDown(mousePos))
			return true;
		else if(isC2() && c2->MouseDown(mousePos))
			return true;
	}

	return	false;
}

bool Vertex::MouseUp(const Vec2& mousePos){
	if(_isAllocated){
		_isAllocated = false;
		_isSelected = false;
		return true;
	}
	else{
		if(isC1() && c1->MouseUp(mousePos))
			return true;
		else if(isC2() && c2->MouseUp(mousePos))
			return true;
	}

	return false;
}

void Vertex::ChangePos(const Vec2& dPos){
	Vec2 dPos_1;
	Vec2 dPos_2;
	if(isC1())
		dPos_1 = c1->getPos() - _pos;
	if(isC2())
		dPos_2 = c2->getPos() - _pos;

	_pos += dPos;
	updateAngle(true);

	if(isC1())
		c1->setPos(_pos + dPos_1);
	if(isC2())
		c2->setPos(_pos + dPos_2);
}

bool Vertex::MouseMoovie(const Vec2& mousePos, Vec2* dPos){
	if(isContains(mousePos)){
		_isSelected = true;
	}
	else{
		_isSelected = false;
	}

	if(_isAllocated){
		Vec2 dPos_1;
		Vec2 dPos_2;
		if(isC1())
			dPos_1 = c1->getPos() - _pos;
		if(isC2())
			dPos_2 = c2->getPos() - _pos;


		Vec2 newPos = _lastPos + mousePos;
		if(NULL != dPos)
			( *dPos ) = Vec2(newPos - _pos);
		_pos = newPos;
		updateAngle(true);

		if(isC1())
			c1->setPos(_pos + dPos_1);
		if(isC2())
			c2->setPos(_pos + dPos_2);
		return true;
	}

	{
		if(isC1() && c1->MouseMoovie(mousePos)){
			if(_type == TypeVertex_Reflected)
				c2->setPos(_pos + _pos - c1->getPos());
			return true;
		}
		else if(isC2() && c2->MouseMoovie(mousePos)){
			if(_type == TypeVertex_Reflected)
				c1->setPos(_pos + _pos - c2->getPos());
			return true;
		}
	}
	return false;
}

void Vertex::setConnections(Vertex* head, Vertex* tail){
	bool isEdit = false;
	if(head != _head){
		isEdit = true;
		_head = head;
		if(NULL != _head){
			_head->setTail(this);
		}
	}
	if(tail != _tail){
		isEdit = true;
		_tail = tail;
		if(NULL != _tail){
			_tail->setHead(this);
		}
	}
	if(isEdit)
		updateAngle(false);
}


void Vertex::updateAngle(bool isConnections){
	if (!_IsAngle)return;
	bool isUpdateAngleToHead = false;
	if(_head != NULL){
		isUpdateAngleToHead = _angleToHead_2 == _angleToHead;

		if(isConnections)
			_head->updateAngle(false);
		_angleToHead = MATH_RAD_TO_DEG(Vec2(_pos, _head->getPos()).getAngle());


	}

	//ToDo remove angle to tail
	if(_tail != NULL){
		if(isConnections)
			_tail->updateAngle(false);
		_angleToTail = MATH_RAD_TO_DEG(Vec2(_pos, _tail->getPos()).getAngle());
	}

	if(_head != NULL && _tail != NULL){
		Vec2 a1 = ( _head->getPos() - _pos );//.getNormalized();
		Vec2 a2 = ( _tail->getPos() - _pos );//.getNormalized();
		_angle = MATH_RAD_TO_DEG(acosf(a1.dot(a2) / ( a1.getLength() * a2.getLength() )));
		if(_orientation == VertexOrientation_External)
			_angle = _angle;
		else if(_orientation == VertexOrientation_Internal)
			_angle = 360 - _angle;
		//else
		//    _angle = 0;

	}

	if(isUpdateAngleToHead)
		setAngleToHead_2(_angleToHead);
}

#pragma warning (push)
#pragma warning (disable:4996)


void Vertex::Draw(bool recurse, bool debugDraw){
	float lineWidth = 1;
	Color4B color = Color4B::RED;
	if(_type == TypeVertex_None)
		lineWidth = 2;
	else if(_type == TypeVertex_Control)
		lineWidth = 2;
	else if(_type == TypeVertex_Control)
		lineWidth = 1;
	else
		lineWidth = 2;

	if(_type == TypeVertex_Control)
		color = Color4B::BLUE;
	else
		color = Color4B::RED;

	if(_isAllocated){
		color = Color4B::ORANGE;
		lineWidth *= 2;
	}
	else if(_isSelected){
		color = Color4B::GREEN;
		lineWidth *= 1.5;
	}


	DrawColor(color);
	glLineWidth(lineWidth);
	if(debugDraw)
		ccDrawPoint(_pos);

	if(debugDraw)
		if(_type != TypeVertex_None)
			ccDrawCircle(_pos, R, 260, 14, false);

	if(recurse){
		if(NULL != this->getHead())
			Draw(this, this->getHead(), true, debugDraw);
		//if (NULL != this->getTail())
		//    Draw( this, this->getTail(), true);
	}
	
	//__________
	if(debugDraw){
		if(isC1()){
			c1->Draw();
			ccDrawLine(c1->getPos(), _pos);
		}

		if(isC2()){
			c2->Draw();
			ccDrawLine(c2->getPos(), _pos);
		}
	}
	
	//___________
	if(debugDraw)
		if(_type != TypeVertex_None && NULL != this->getHead()){
			Vec2 p1 = _pos;
			Vec2 p2 = this->getHead()->getPos();

			Vec2 p3 = p1 + Vec2(( ( p1 - p2 ).getLength() * 0.5 ), 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead));

			glLineWidth(1);
			DrawColor(Color4B::YELLOW);

			//стрелка
			p1 = p3 + Vec2(wCros, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead_2 - 160));
			p2 = p3 + Vec2(wCros, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead_2 + 165));
			ccDrawLine(p3, p1);
			ccDrawLine(p3, p2);

			if(isAngleToHead_2()){
				//glLineWidth( 5 );
				//ccDrawPoint( p3 );

				p1 = p3 + Vec2(wCros2, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead_2));
				p2 = p3 + Vec2(wCros2, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead_2 + 180));
				ccDrawLine(p3, p1);
				ccDrawLine(p3, p2);
			}

			//###
			if(_friction != 0){
				DrawColor(Color4B(114, 84, 173, 255));
				p1 = _pos;
				p2 = this->getHead()->getPos();
				//стрелка
				p3 = p1 + Vec2(( ( p1 - p2 ).getLength() * _friction ), 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead));
				p1 = p3 + Vec2(wCros, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead_2 - 160));
				p2 = p3 + Vec2(wCros, 0).rotateByAngle(Vec2::ZERO, MATH_DEG_TO_RAD(_angleToHead_2 + 165));

				ccDrawLine(p3, p1);
				ccDrawLine(p3, p2);
				//линия
				//ccDrawLine(p1, p1);

			}
		}

}


void Vertex::Draw(const Vertex* v1, const Vertex* v2, bool notStyle, bool forceLine, bool debugDraw){
	if(!notStyle){
		DrawColor(Color4B::RED);
		glLineWidth(2);
		/*
		if (v1->getTypeFromCrossing()==TypeFromCrossing_OnlyLeftRight)
		{
			DrawColor(Color4B(210, 3, 232, 255));
			glLineWidth(4);
		}
		*/
	}

#if 0
	if(debugDraw)
		if(!forceLine && ( v1->isC1() || v2->isC2() )){
			ccDrawCubicBezier(v1->getPos(),
							  v1->getC2(),
							  v2->getC1(),
							  v2->getPos(), 40);
			return;
	}
	ccDrawLine(v1->getPos(), v2->getPos());
#endif
	if(!forceLine && ( v1->isC() || v2->isC() )){
		glLineWidth(4);
		DrawSpline(v1, v2, 40);
	}
	else{
		ccDrawLine(v1->getPos(), v2->getPos());
	}

}

#pragma warning (pop)

void Vertex::setTypeFromCrossing(TypeFromCrossing  type){
	_isCalcType = false;
	_typeFromCrossing = type;
}

TypeFromCrossing Vertex::getTypeFromCrossing() const{
	if(!_isCalcType)
		return _typeFromCrossing;

	float angle = getAngleToHead();
	if(angle >= 60 || angle <= -60){
		return TypeFromCrossing_OnlyLeftRight;
	}
	return TypeFromCrossing_Free;
}


void Vertex::DrawSpline(const Vertex* v1,const Vertex* v2,uint count_setors) {
	ccDrawCubicBezier(v1->getPos(),
					  v1->getC2(),
					  v2->getC1(),
					  v2->getPos(), count_setors);
}