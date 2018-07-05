#pragma once
#include "stdafx.h"



enum TypeVertex
{
	TypeVertex_None = -1,
	TypeVertex_Point,
	TypeVertex_Reflected,
	TypeVertex_Free,
	TypeVertex_OnlyC1,
	TypeVertex_OnlyC2,
	TypeVertex_Control
};

//ToDo deprecated this
enum VertexOrientation
{
	VertexOrientation_None = -1,
	VertexOrientation_External,
	VertexOrientation_Internal
};

enum TypeFromCrossing
{
	TypeFromCrossing_None = -1,
	TypeFromCrossing_Free,
	TypeFromCrossing_OnlyLeftRight,
	TypeFromCrossing_OnlyBottom
};




class Vertex
{
public:
	Node* node = NULL;
	void* getParent() const{ return m_parent; }
	void setParent(void* parent){ m_parent = parent; }

	void* m_data = NULL;
	void setData(void* data) { m_data = data; }
	void* getData() { return m_data; }
private:

	void* m_parent = NULL;
	Vertex* c1 = NULL;
	Vertex* c2 = NULL;
	size_t countFromBezier = 2;// 4;

	bool _isCalcType = true;
	TypeFromCrossing _typeFromCrossing = TypeFromCrossing_None;

	TypeVertex _type = TypeVertex_None;
	VertexOrientation _orientation = VertexOrientation_External;
	Vec2 _pos;
	Vec2 _lastPos;
	Vec2 _dtpos;

	float wCros = 20;
	float wCros2 = 50;
	float R = 20.;
	float _scale = 1.;
	bool  _isAllocated = false;
	bool _isSelected = false;

	bool _isAssistant = false;

	Vertex* _head = NULL;
	Vertex* _tail = NULL;
	float _angleToHead = 0;
	float _angleToHead_2 = 0;
	float _angleToTail = 0;

	float _angle = 0;
	float _friction = 0;

	int _temporaryId = -1;

public:
	void setPosText(const Vec2& pos){ _dtpos = pos; }
	Vec2 getPosText() const{ return _dtpos; }

	void setAssistant(bool enable){ _isAssistant = enable; }
	bool hasAssistant()const{ return _isAssistant; }

	void setScale(float scale);
	float getScale()const { return _scale; }
	//todo remove
	void setId(size_t temporaryId){ _temporaryId = temporaryId; }
	size_t getId()const{ return _temporaryId; }
	//float getAngleToHead( bool force = false )const { return  false == force && isAngleToHead_2() ? _angleToHead_2 : _angleToHead; }
	float getAngleToHead()const{ return _angleToHead; }
	float getAngleToTail()const{ return _angleToTail; }
	float getAngle()const{ return _angle; }

	float getFriction()const{ return _friction; }
	void setFriction(float val){ _friction = val; }

	TypeFromCrossing getTypeFromCrossing() const;
	bool isCalcTypeFromCrossing() const{ return _isCalcType; }

	void freeCalcType(){
		_typeFromCrossing = TypeFromCrossing_None;
		_isCalcType = true;
	}

	void setTypeFromCrossing(TypeFromCrossing  type);

	bool isAngleToHead_2() const{ return _angleToHead != _angleToHead_2; }
	float getAngleToHead_2()const{ return _angleToHead_2; }
	void setAngleToHead_2(float angle){ _angleToHead_2 = angle; }
	bool clearAngleToHead_2(){ bool ret = isAngleToHead_2(); _angleToHead_2 = _angleToHead; return ret; }

	Vertex(const Vertex* vertex){
		setAssistant(vertex->hasAssistant());
		setFriction(vertex->getFriction());
		if(!vertex->isCalcTypeFromCrossing())
			setTypeFromCrossing(vertex->getTypeFromCrossing());
		_orientation = vertex->getOrientation();
		_pos = vertex->getPos();
		setPosText(vertex->getPosText());
		setType(vertex->getType());
		setControll(vertex->getC1(), vertex->getC2());
		setCountfromBezier(vertex->getCountFromBezier());
		if(vertex->isAngleToHead_2())
			setAngleToHead_2(vertex->getAngleToHead_2());

		setParent(vertex->getParent());

		setScale(vertex->getScale());
	}

	Vertex(){
		setType(TypeVertex_None);
	}

	Vertex(const Vec2& pos, TypeVertex type = TypeVertex_None, bool isAllocated = false):_pos(pos){
		setType(type);
		_isAllocated = isAllocated;
	}

    ~Vertex();

	TypeVertex getType() const{ return _type; }

	void setControll(Vec2 _c1, Vec2 _c2){
		if(_type == TypeVertex_Free || _type == TypeVertex_Reflected){
			c1->setPos(_c1);
			c2->setPos(_c2);
		}
		else if(_type == TypeVertex_OnlyC1){
			c1->setPos(_c1);
		}
		else if(_type == TypeVertex_OnlyC2){
			c2->setPos(_c2);
		}
	}

	void setType(TypeVertex type){
		if(type == _type)
			return;
		if(type == TypeVertex_Point){
			if(_type == TypeVertex_Free || _type == TypeVertex_Reflected || _type == TypeVertex_OnlyC1 || _type == TypeVertex_OnlyC2){
				deleteControl();
			}
		}
		else if(type == TypeVertex_Free){
			createControl();
		}
		else if(type == TypeVertex_Reflected){
			createControl();
			c2->setPos(_pos + _pos - c1->getPos());
		}
		else if(type == TypeVertex_OnlyC1){
			createControl();
			if(NULL != c2)
				delete c2;
			c2 = NULL;
		}
		else if(type == TypeVertex_OnlyC2){
			createControl();
			if(NULL != c1)
				delete c1;
			c1 = NULL;
		}
		_type = type;
	}

	VertexOrientation getOrientation()const{ return _orientation; }

	void setOrientation(VertexOrientation orientation){
		if(_orientation == orientation)
			return;
		_orientation = orientation;
		updateAngle(false);


		//ToDo remove
		//#######
		if(node != NULL){
			node->removeFromParent();
			node = NULL;
		}
	}

	void deleteControl(){
		if(NULL != c1)
			delete c1;
		if(NULL != c2)
			delete c2;
		c1 = NULL;
		c2 = NULL;
	}

	void createControl(){
		if(NULL == c1){
			c1 = new Vertex(_pos + Vec2(20, 20), TypeVertex_Control);
		}
		if(NULL == c2){
			c2 = new Vertex(_pos - Vec2(20, 20), TypeVertex_Control);
		}
	}



	const size_t getCountFromBezier()const{ return countFromBezier; }
	void setCountfromBezier(const size_t& count){ countFromBezier = MAX(2, count); }

	bool isC()const{ return isC1() || isC2(); }
	bool isC1()const{ return NULL != c1; }
	bool isC2() const{ return NULL != c2; }

	const Vec2& getC1()const{ if(NULL == c1) return _pos; return c1->getPos(); }
	const Vec2& getC2()const{ if(NULL == c2) return _pos; return c2->getPos(); }

	const Vec2& getPos() const{ return _pos; }

	void setPos(const Vec2& pos, bool hasC = false){
		if(hasC){
			Vec2 dPos_1;
			Vec2 dPos_2;
			if(isC1())
				dPos_1 = c1->getPos() - _pos;
			if(isC2())
				dPos_2 = c2->getPos() - _pos;

			updateAngle(true);

			if(isC1())
				c1->setPos(pos + dPos_1);
			if(isC2())
				c2->setPos(pos + dPos_2);
		}
		_pos = Vec2(pos);
	}

	bool isContains(const Vec2& pos){
		return _pos.distance(pos) <= R;
	}

	bool MouseDown(const Vec2& mousePos);

	bool MouseUp(const Vec2& mousePos);

	bool MouseMoovie(const Vec2& mousePos, Vec2* dPos = NULL);

	void setConnections(Vertex* head = NULL, Vertex* tail = NULL);

	void setHead(Vertex* head = NULL){ setConnections(head, _tail); };
	void setTail(Vertex* tail = NULL){ setConnections(_head, tail); };

	const Vertex* getHead() const{ return _head; }
	const Vertex* getTail() const{ return _tail; }

	void updateAngle(bool isConnections);

	void Draw(bool recurse = false, bool debugDraw = true);


	static void Draw(const Vertex* v1, const Vertex* v2, bool notStyle = false, bool forceLine = false, bool debugDraw = false);
	static void DrawSpline(const Vertex* v1, const Vertex* v2, uint count_setors);

	void ChangePos(const Vec2& dPos);
};

