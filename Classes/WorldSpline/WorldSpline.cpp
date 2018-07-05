#include "stdafx.h"
#include "WorldSpline.h"
#include "DevelopMenu.h"
//on-off create spline( добавить новую точку и стоп)

//on-off edit point from type 
// перемещение вершины и направляющих


//todo    
//	добавить точку к выделяемой линии
//

//______
//on-off направлящий вектор к выделенной линии

WorldSpline::WorldSpline(int max_count_spline, DevelopTabs tabs, bool createEdits):
	_scale_from_draw(.0),
	_enable_save(true),
	_enable_text(true),
	_pos(Vec2::ZERO),
	_tabs(tabs),
	_isVisible(true),
	_fileName(),
	_parentWOBJ(NULL),
	_color_text(Color4B::BLACK),
	_z(0)
	//_loadChild(nullptr),
	//_saveChild(nullptr)
{
	_maxCountSplines = max_count_spline;
	isAutoCreate = false;
	_splineChanged = false;

	_isEdit = false;
#if HAS_DEVELOP
	changeEdits(createEdits);
#endif
};

bool WorldSpline::changeKeys(EventKeyboard::KeyCode keyCode, KeyState keyState) {
#if HAS_DEVELOP
	if (!DevelopMenu::Instance().isEnable())
		return false;
	bool ret = false;
	switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_DELETE:
		case EventKeyboard::KeyCode::KEY_BACKSPACE:
			if (keyState == KeyState_RELEASE) {
				stopCreate();
				ret = true;
			}

			break;
	}
	return ret;
#endif
	return false;
}


void WorldSpline::setScaleFromDraw(float scale) {
	if (_scale_from_draw == scale)return;
	_scale_from_draw = scale;
	for (auto* _tspline : _TSplines)
		for (Vertex* var : _tspline->getControl()) {
			var->setScale(scale);
		}
}

bool WorldSpline::isChangedSpline() {
	bool ret = getChangedSpline();
	setChangedSpline(false);
	return ret;
}

WorldSpline::~WorldSpline() {

	free();
#if HAS_DEVELOP
	DevelopMenu::Instance().eraceSettings(this);
	DevelopMenu::Instance().clearChangeKeyEvents(this);
#endif
}

#if HAS_DEVELOP
void WorldSpline::changeEdits(bool create) {
	if (create && NULL == m_editCreateSpline) {
		m_editCreateSpline = DevelopMenu::Instance().addBoolSetting(this, 0, _tabs, "Create Spline",
			([this](const bool isEdit) { _isEdit = isEdit; stopCreate(true); }),
																	([this](void) { return _isEdit; }), SeparatorType_Bottom);

		DevelopMenu::Instance().changeKeyEvents(
			CC_CALLBACK_2(WorldSpline::changeKeys, this)
			, this, "Del,Backspace - stopCreateSpline");
	}

	if (!create && NULL != m_editCreateSpline) {
		DevelopMenu::Instance().eraceSettings(this);
		DevelopMenu::Instance().clearChangeKeyEvents(this);
		m_editCreateSpline = NULL;
	}
}
#endif

void WorldSpline::deselect() {
#if HAS_DEVELOP
	DevelopMenu::Instance().eraceSettings(this, 1);
	edit_isVertexType = NULL;
	_isAllocated_prev = NULL;
	
	//_isEditableSpline = NULL;
	//_isAllocated = NULL;
#endif
}

void WorldSpline::select(bool force) {
#if HAS_DEVELOP
	if (!force) {
		if (_isAllocated_prev == _isAllocated)
			return;
		deselect();
		if (_isAllocated == NULL)
			return;
		_isAllocated_prev = _isAllocated;
	}

	DevelopMenu::Instance().addBoolSetting(
		this, 1,
		_tabs, "Invert Orientation",
		([this](const bool& clear) {
		if (NULL != _isEditableSpline)
			_isEditableSpline->recreteSpline(true);
	}),
		 ([this](void) { return false; }),
		SeparatorType_Bottom);

	const std::vector<std::string> namesType{ "Point", "Reflected", "Free", "Only C1", "Only C2" };

	edit_isVertexType = DevelopMenu::Instance().addSelectItem(
		this, 1,
		_tabs, "Vertex type", namesType,
		([this](const size_t& item) {
		_isAllocated->setType((TypeVertex)item);
		setChangedSpline(true);
	}),
		 ([this](void) {
		return (size_t)_isAllocated->getType();
	})
		, SeparatorType_Bottom
		);

	/*
	const std::vector<std::string> namesOrientation{ "Forward", "Backward" };
	edit_isEditOrientation = DevelopMenu::Instance().addSelectItem(
	_tabs, "Vertex Orientation", namesOrientation,
	([this]( const size_t& item )
	{
	_isAllocated->setOrientation( (VertexOrientation)item );
	setChangedSpline( true );
	}),
	([this]( void )
	{
	return (size_t)_isAllocated->getOrientation();
	})
	, SeparatorType_Bottom
	);
	*/


	DevelopMenu::Instance().addBoolSetting(
		this, 1,
		_tabs, "Add in Curr Spline",
		([this](const bool& add) {
		Vertex* vertex = new Vertex(_isAllocated);
		const Vertex* to = _isAllocated->getHead();
		if (to == NULL)
			to = _isAllocated->getTail();
		Vec2 d_new_pos = Vec2(100, 100);
		if (to) {
			float t_lenght = _isAllocated->getPos().distance(to->getPos());
			float angle = _isAllocated->getPos().getAngle(to->getPos());
			d_new_pos = Vec2(t_lenght, .0).rotateByAngle(Vec2::ZERO, angle);
		}

		vertex->setType(TypeVertex_Point);
		vertex->setPos(vertex->getPos() + d_new_pos);
		_isEditableSpline->addControl(vertex, _isAllocated);
		_isAllocated = vertex;
		setChangedSpline(true);

		//edit_isEditOrientation->Refresh();
		edit_isVertexType->Refresh();
	}),
		 ([this](void) { return false; }),
		SeparatorType_Bottom);


	DevelopMenu::Instance().addUIntSettings(
		this, 1,
		_tabs, "count Bezier",
		([this](const size_t& size) {
		_isAllocated->setCountfromBezier(size);
		setChangedSpline(true);
	}),
		 ([this](void) {
		return _isAllocated->getCountFromBezier();
	}),
		SeparatorType_Bottom);

	//##################### NEW
	DevelopMenu::Instance().addBoolSetting(
		this, 1,
		_tabs, "setClosed",
		([this](const bool& enable) {
		_isEditableSpline->setClosed(enable);
	}),
		 ([this](void) { return _isEditableSpline->IsClosed(); }),
		SeparatorType_Bottom);

	if (_enable_text) {
		DevelopMenu::Instance().addPointSetting(
			this, 1,
			_tabs, "dTextPos",
			([this](const Vec2& pos) { _isAllocated->setPosText(pos); }),
			([this](void) { return _isAllocated->getPosText(); })
			, SeparatorType_Bottom);
	}
	/*

float min = -360;
float max = 360;
DevelopMenu::Instance().addFloatSettingFromMinMax(
	this, 1,
	_tabs,
	"angle",
	([this](const float& angle) {
	_isAllocated->setAngleToHead_2(angle);
	setChangedSpline(true);
}),
	 ([this]() { return _isAllocated->getAngleToHead_2(); }),
	&(min), &max,
	SeparatorType_Bottom);

DevelopMenu::Instance().addBoolSetting(
	this, 1,
	_tabs, "ClearRotate",
	([this](const bool& clear) {
	if (_isAllocated->clearAngleToHead_2())
		setChangedSpline(true);
}),
	 ([this](void) { return false; }),
	SeparatorType_Bottom);

//___

const std::vector<std::string> namesType_2{"Free", "OnlyLeftRight"};

DevelopMenu::Instance().addSelectItem(
	this, 1,
	_tabs, "Type From Crossing", namesType_2,
	([this](const size_t& item) {
	_isAllocated->setTypeFromCrossing(item==1?TypeFromCrossing_OnlyLeftRight:TypeFromCrossing_Free);
	setChangedSpline(true);
}),
	 ([this](void) {
	return _isAllocated->getTypeFromCrossing()==TypeFromCrossing_Free?0:1;
})
	);

DevelopMenu::Instance().addBoolSetting(
	this, 1,
	_tabs, "ClearTypeFromCrossing",
	([this](const bool& clear) {
	_isAllocated->freeCalcType();
	setChangedSpline(true);
}),
	 ([this](void) { return false; }),
	SeparatorType_Bottom);
DevelopMenu::Instance().addFloatSliderSetting(
	this, 1,
	_tabs, "Edit Friction",
	([this](const float& val) {
	_isAllocated->setFriction(val);
	setChangedSpline(true);
}),
	 ([this](void) { return _isAllocated->getFriction(); }),
	SeparatorType_Bottom);
	*/
#endif
}

std::vector<TSpline*> WorldSpline::getPolygons() {
	return _TSplines;
}

void WorldSpline::free() {
	deselect();
	for (TSpline* spline : _TSplines) {
		delete spline;
	}
	_TSplines.clear();
}

#define _SPLINE_SIZE "_splines_size"
#define SPLINE_SIZE "splines_size"

bool hasOldFOrmat(const std::string& filepath) {
	return 0 == FileUtils::getInstance()->getFileExtension(filepath).compare(".txt");
}

void WorldSpline::save(bool force, const Vec2& d_pos /*= Vec2::ZERO*/, const float& scale /*= 1.f*/) {
#if HAS_DEVELOP
	if (!_enable_save)return;
	if (!force && !_isEdit)return;

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(_fileName);
#if 0//сохранение в Debug папку неправильное - надо путь поменять для внутренней под директории
	if (fullPath.empty() || !FileUtils::getInstance()->isFileExist(fullPath)) {
		//_filePath += FileUtils::getInstance()->getWritablePath() + fileName;
		fullPath = HelperUtils::getLocalPath(false) + _fileName;
		//DebugBreak();
	}
#endif
	if (
		//hasOldFOrmat(fullPath) && 
		//!FileUtils::getInstance()->isFileExist(fullPath + ".xml")
		true
		) {
		size_t splines_size = _TSplines.size();
		auto root = __Dictionary::create();
		root->setObject(__Integer::create((int)splines_size), _SPLINE_SIZE);

		if (splines_size > 0) {
			//for (int i = splines_size - 1; i >= 0; i--) {
			//    TSpline* spline = _TSplines.at(i);
			size_t i = 0;
			for (TSpline* spline : _TSplines) {
				const std::vector<Vertex*> controls = spline->getControl();
				size_t controls_size = controls.size();

				auto dictInSplines = __Dictionary::create();
				dictInSplines->setObject(__Integer::create(controls_size), SPLINE_SIZE);
				dictInSplines->setObject(__Bool::create(spline->IsClosed()), "spline_isClosed");

				//___
				//if(nullptr != _saveChild)
				//	_saveChild(spline, dictInSplines);
				saveChild(spline, dictInSplines, d_pos, scale);
				//___

				char str[100] = { 0 };
				//for (int j = controls_size - 1; j >= 0; j--) {
				   // Vertex* vertex = controls.at(j);
				size_t j = 0;
				for (Vertex* vertex : controls) {

					auto dictInSpline = __Dictionary::create();

					//if (vertex->isAngleToHead_2())
					//	dictInSpline->setObject(__Float::create(vertex->getAngleToHead_2()), "angleToHead");

					//if (!vertex->isCalcTypeFromCrossing())
					//	dictInSpline->setObject(__Integer::create(vertex->getTypeFromCrossing()), "typeFromCrossing");

					//if (vertex->getFriction()!=0)
					//	dictInSpline->setObject(__Float::create(vertex->getFriction()), "friction");

					dictInSpline->setObject(__Float::create((vertex->getPos().x - d_pos.x) / scale), "x");
					dictInSpline->setObject(__Float::create((vertex->getPos().y - d_pos.y) / scale), "y");
					dictInSpline->setObject(__Integer::create(vertex->getCountFromBezier()), "countFromBezier");
					dictInSpline->setObject(__Integer::create((int)vertex->getType()), "type");
					TypeVertex type = vertex->getType();
					if (type == TypeVertex_Free || type == TypeVertex_Reflected || type == TypeVertex_OnlyC1 || type == TypeVertex_OnlyC2) {
						dictInSpline->setObject(__Float::create((vertex->getC1().x - d_pos.x) / scale), "c1_x");
						dictInSpline->setObject(__Float::create((vertex->getC1().y - d_pos.y) / scale), "c1_y");

						dictInSpline->setObject(__Float::create((vertex->getC2().x - d_pos.x) / scale), "c2_x");
						dictInSpline->setObject(__Float::create((vertex->getC2().y - d_pos.y) / scale), "c2_y");
					}

					if (_enable_text) {
						dictInSpline->setObject(__Float::create((vertex->getPosText().x - d_pos.x) / scale), "dtx");
						dictInSpline->setObject(__Float::create((vertex->getPosText().y - d_pos.y) / scale), "dty");
					}

					if (vertex->hasAssistant()) {
						dictInSpline->setObject(__Bool::create(true), "isAssistant");
					}

					sprintf(str, "vertex_%i", j);
					j++;
					dictInSplines->setObject(dictInSpline, str);
				}
				sprintf(str, "spline_%i", i);
				i++;
				root->setObject(dictInSplines, str);
			}
		}
		root->writeToFile(fullPath.c_str());
	}
	else {

		//save NEW
		tinyxml2::XMLDocument doc;

		size_t splines_size = _TSplines.size();
		if (splines_size > 0) {

			size_t i = 0;
			for (TSpline* spline : _TSplines) {
				const std::vector<Vertex*> controls = spline->getControl();

				tinyxml2::XMLElement* element = doc.NewElement("spline");
				element->SetAttribute("spline_isClosed", spline->IsClosed());

				//todo maybe
				//saveChild(spline, dictInSplines, d_pos, scale);

				size_t j = 0;
				for (Vertex* vertex : controls) {
					std::string point_param = StringUtils::format(
						"%f %f %i %i %s %s %i",//x y countFromBezier type "c1_x c1_y c2_x c2_y" "dtx dty" isAssistant
						(vertex->getPos().x - d_pos.x) / scale,//x
						(vertex->getPos().y - d_pos.y) / scale,//y
						vertex->getCountFromBezier(),//countFromBezier
						(int)vertex->getType(), //type
						((vertex->getType() >= TypeVertex_Reflected && vertex->getType() <= TypeVertex_OnlyC2) ?

						 StringUtils::format("%f %f %f %f",//"c1_x c1_y c2_x c2_y "
						 ((vertex->getC1().x - d_pos.x) / scale),// c1_x
											 ((vertex->getC1().y - d_pos.y) / scale), //c1_y
											 ((vertex->getC2().x - d_pos.x) / scale),//c2_x
											 ((vertex->getC2().y - d_pos.y) / scale) //c2_y
						 ).c_str():
						 "0 0 0 0"),

						 (_enable_text ?
						  StringUtils::format("%f %f",//"dtx dty"
						  ((vertex->getPosText().x - d_pos.x) / scale),//dtx
											  ((vertex->getPosText().y - d_pos.y) / scale)//dty
						  ).c_str() : "0 0"),
						  (int)vertex->hasAssistant()
					);
					tinyxml2::XMLElement* XMLpoint = doc.NewElement("point");
					XMLpoint->SetAttribute("param", point_param.c_str());
					element->InsertEndChild(XMLpoint);
				}
				doc.InsertEndChild(element);
			}
		}
		doc.SaveFile(fullPath.c_str(), false);
		//________
	}
#endif
}

#if IS_PC || IS_IOS
#define  LOGD(...) CCLOG(__VA_ARGS__)
#elif IS_ANDROID
#include "platform/CCPlatformConfig.h"
#define  LOG_TAG    "CCFileUtils-stage"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#endif


void WorldSpline::load(const std::string& fileName, const Vec2& d_pos /*= Vec2::ZERO*/, const float& scale /*= 1.*/) {
	if (fileName.empty())return;
	_fileName = fileName;
	_isAllocated = NULL;
	free();

	addSpline(_fileName, d_pos, scale);

	//std::string full_path = FileUtils::getInstance()->fullPathForFilename(_fileName) + ".xml";
	//if (!FileUtils::getInstance()->isFileExist(full_path)) {
	//	_fileName += ".xml";
	//	save(true, d_pos, scale);
	//}
}

void WorldSpline::freeClear() {
	_isAllocated = NULL;
	deselect();
}

//static bool step = false;
void WorldSpline::addSpline(const std::string& fileName, const Vec2& d_pos /*= Vec2::ZERO*/, const float& scale /*= 1.*/) {
	if (fileName.empty())return;

	std::string fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
	auto root = __Dictionary::createWithContentsOfFile(fullPath.c_str());

	//double s_t2 = HelperUtils::GetCurrTime();
	if (
		//hasOldFOrmat(fullPath) && 
		//!FileUtils::getInstance()->isFileExist(fullPath + ".xml")
		//step = !step
		true
		) {
		//CCLOG("old variant");
		//s_t2 = HelperUtils::GetCurrTime();
		auto size = ((__String*)root->valueForKey(_SPLINE_SIZE));
		if (NULL == size) return;
		int splines_size = size->intValue();
		//LOGD("splines_size = %i", splines_size);
		if (splines_size <= 0)
			return;

		char str[100] = { 0 };
		for (int i = 0; i < splines_size; ++i) {
			sprintf(str, "spline_%i", i);

			auto dictInSplines = (__Dictionary*)root->objectForKey(str);
			int spline_size = ((__String*)dictInSplines->valueForKey(SPLINE_SIZE))->intValue();

			bool isClose = ((__String*)dictInSplines->valueForKey("spline_isClosed"))->boolValue();
			TSpline* spline = new TSpline();

			for (int j = 0; j < spline_size; ++j) {
				sprintf(str, "vertex_%i", j);
				auto dictInSpline = (__Dictionary*)dictInSplines->objectForKey(str);


				/*
				bool isAngleToHead = false;
				float angle;

				__String* tmp = ((__String*)dictInSpline->valueForKey("angleToHead"));
				if (tmp->length()!=0)
				{
				isAngleToHead = true;
				angle = tmp->floatValue();
				}

				int typeFromCross = -1;
				tmp = ((__String*)dictInSpline->valueForKey("typeFromCrossing"));
				if (tmp->length()!=0)
				typeFromCross = tmp->intValue();

				float friction = 0;
				tmp = ((__String*)dictInSpline->valueForKey("friction"));
				if (tmp->length()!=0)
				friction = tmp->floatValue();
				*/

				float x = ((__String*)dictInSpline->valueForKey("x"))->floatValue();
				float y = ((__String*)dictInSpline->valueForKey("y"))->floatValue();
				size_t count = ((__String*)dictInSpline->valueForKey("countFromBezier"))->uintValue();

				TypeVertex type = (TypeVertex)((__String*)dictInSpline->valueForKey("type"))->intValue();
				//LOGD("x=%.2f y=%.2f", x, y);
				Vertex* vertex = new Vertex(Vec2(x, y)*scale + d_pos, type);
				//if (friction!=0)					  - d_pos.y
				//	vertex->setFriction(friction);
				//if (isAngleToHead)
				//	vertex->setAngleToHead_2(angle);
				//if (typeFromCross!=-1)
				//	vertex->setTypeFromCrossing((TypeFromCrossing)typeFromCross);
				vertex->setCountfromBezier(count);
				if (type == TypeVertex_Free || type == TypeVertex_Reflected || type == TypeVertex_OnlyC1 || type == TypeVertex_OnlyC2) {
					float c1_x = ((__String*)dictInSpline->valueForKey("c1_x"))->floatValue();
					float c1_y = ((__String*)dictInSpline->valueForKey("c1_y"))->floatValue();

					float c2_x = ((__String*)dictInSpline->valueForKey("c2_x"))->floatValue();
					float c2_y = ((__String*)dictInSpline->valueForKey("c2_y"))->floatValue();

					vertex->setControll(Vec2(c1_x, c1_y)*scale + d_pos, Vec2(c2_x, c2_y)*scale + d_pos);
				}
				if (_enable_text) {
					float dtx = ((__String*)dictInSpline->valueForKey("dtx"))->floatValue();
					float dty = ((__String*)dictInSpline->valueForKey("dty"))->floatValue();
					vertex->setPosText(Vec2(dtx, dty)*scale + d_pos);
				}

				if (((__String*)dictInSpline->valueForKey("isAssistant"))->boolValue()) {
					vertex->setAssistant(true);
				}
				spline->pushBackControl(vertex, false);
			}

			spline->setClosed(isClose, false);
			//___
			loadChild(spline, dictInSplines, d_pos, scale);
			//___
			_TSplines.push_back(spline);
		}
	}
	else {
		//##################
		//CCLOG("nnew  variant");
		//s_t2 = HelperUtils::GetCurrTime();
		std::string buf;
		FileUtils::getInstance()->getContents(fullPath + ".xml", &buf);
		//save NEW
		tinyxml2::XMLDocument doc;
		doc.Parse(buf.c_str());

		//tinyxml2::XMLElement* root = doc.RootElement();
		FOR_EACH_XML_ELEMENT((&doc), node_spline, "spline") {
			bool isClose = xmlAttrToBool(node_spline, "spline_isClosed", false);
			TSpline* spline = new TSpline();

			FOR_EACH_XML_ELEMENT(node_spline, node_point, "point") {
				std::vector<std::string> param = HelperUtils::split(node_point->Attribute("param"), " ");
				std::vector<std::string>::iterator it = param.begin();
#define GET()	((*(it++)).c_str())
				float x = atof(GET());
				float y = atof(GET());
				size_t count = atoi(GET());

				TypeVertex type = (TypeVertex)(atoi(GET()));
				Vertex* vertex = new Vertex(Vec2(x, y)*scale + d_pos, type);
				vertex->setCountfromBezier(count);
				if (type == TypeVertex_Free || type == TypeVertex_Reflected || type == TypeVertex_OnlyC1 || type == TypeVertex_OnlyC2) {
					float c1_x = atof(GET());
					float c1_y = atof(GET());

					float c2_x = atof(GET());
					float c2_y = atof(GET());

					vertex->setControll(Vec2(c1_x, c1_y)*scale + d_pos, Vec2(c2_x, c2_y)*scale + d_pos);
				}
				else { GET(); GET(); GET(); GET(); }
				if (_enable_text) {
					float dtx = atof(GET());
					float dty = atof(GET());
					vertex->setPosText(Vec2(dtx, dty)*scale + d_pos);
				}
				else { GET(); GET(); }

				if (1 == atoi(GET())) {
					vertex->setAssistant(true);
				}
				spline->pushBackControl(vertex, false);
			}
			spline->setClosed(isClose, false);
			//___todo maybe
			//loadChild(spline, dictInSplines, d_pos, scale);
			//___
			_TSplines.push_back(spline);
		}
	}
	/*
	CCLOG("delay = %f", HelperUtils::GetCurrTime() - s_t2);
	s_t2 = HelperUtils::GetCurrTime();
	*/
	setChangedSpline(true);
	//CCLOG("delay 2= %f", HelperUtils::GetCurrTime() - s_t2);
	//##################
}

bool WorldSpline::mouseDown(const Vec2& mousePos, bool isCreated) {
	if (!_isEdit)return false;
	if (isAutoCreate)
		return true;

	//edited point
	for (TSpline* _tspline : _TSplines)
		for (Vertex* var : _tspline->getControl()) {
			if (var->MouseDown(mousePos)) {
				_lastPos = mousePos;
				_isAllocated = var;
				_isEditableSpline = _tspline;

				//_isAllocated->setParent(_isEditableSpline);
				return true;
			}
		}
	if (!isCreated)return false;
	if (_maxCountSplines != -1 && _maxCountSplines <= _TSplines.size())
		return true;
	_isAllocated = new Vertex(mousePos, TypeVertex_Point, true);
	_isEditableSpline = new TSpline(_isAllocated);
	_TSplines.push_back(_isEditableSpline);
	isAutoCreate = true;

	//_isAllocated->setParent(_isEditableSpline);
	return true;

}

bool WorldSpline::mouseMovie(const Vec2& mousePos, Vec2* dPos) {
	if (!_isEdit)return false;


	if (NULL != _isAllocated && _isAllocated->MouseMoovie(mousePos, dPos)) {
		return true;
	}
	//_____________ 

	for (TSpline* _tspline : _TSplines)
		for (Vertex* var : _tspline->getControl()) {
			if (var->MouseMoovie(mousePos)) {
				return true;
			}
		}
	return false;
}

bool WorldSpline::checkIntersectVertex() {
	bool ret = false;

	for (std::vector<TSpline*>::iterator _tspline = _TSplines.begin(); _tspline != _TSplines.end(); ++_tspline) {
		TSpline* spline = *_tspline;
		std::vector<Vertex*> spline_control = spline->getControl();
		bool erace_spline = false;
		if (spline_control.size() < 2)
			erace_spline = true;
		else {
			/*
			if (spline.size() <= 2 &&
			(NULL != _isEditableSpline && (*_tspline) == _isEditableSpline)
			)
			continue;

			for (std::vector<Vertex*>::iterator pi = spline.begin(); pi != spline.end() - 1 && spline.size() > 0; ++pi)
			{
			for (std::vector<Vertex*>::iterator pj = pi + 1; pj < spline.end(); ++pj)
			{
			if ((*pi)->isContains( (*pj)->getPos() ))
			{
			if (NULL != _isAllocated && (_isAllocated == (*pi) || _isAllocated == (*pj)))
			continue;
			pj--;
			spline.erase( pj + 1 );
			ret = true;
			if (spline.size() <= 0 && (*_tspline) != _isEditableSpline)
			erace_spline = true;
			}
			}
			if (erace_spline)
			{
			break;
			}
			}
			/**/
		}
		if (erace_spline) {
			if ((NULL != _isEditableSpline && spline == _isEditableSpline))
				_isEditableSpline = NULL;
			bool has_stop = _TSplines.size() == 2;
			delete spline;
			std::vector<TSpline*>::iterator t_it = _TSplines.erase(_tspline);
			if (has_stop) break;

			if (_TSplines.size() == 0)
				continue;
			if (t_it != _TSplines.begin())
				_tspline = t_it - 1;
			else
				//_tspline = _TSplines.end();
				break;

		}
	}

	return ret;
}

bool WorldSpline::mouseUp(const Vec2& mousePos) {
	if (!_isEdit)return false;
	//Edit Point
	if (NULL != _isAllocated) {
		if (_isAllocated->MouseUp(mousePos)) {
			if (isAutoCreate) {
				_isAllocated = new Vertex(mousePos, TypeVertex_Point, true);
				_isEditableSpline->pushBackControl(_isAllocated);
			}
			else {
				//if (NULL != _isEditableSpline)
				//_isAllocated->setParent(_isEditableSpline);
				select();
			}
			//return true;
		}
		checkIntersectVertex();
		setChangedSpline(true);

		save();
		return true;
	}
	//__________________ todo remove

	for (TSpline* _tspline : _TSplines)
		for (Vertex* var : _tspline->getControl()) {
			if (var->MouseUp(mousePos)) {
				//setChangedSpline( true );
				return true;
			}
		}

	return false;
}

bool WorldSpline::stopCreate(bool force) {

	if (NULL != _isAllocated && NULL != _isEditableSpline) {
		if (false == force) {
			_isEditableSpline->eraceControl(_isAllocated);
			delete _isAllocated;
			_isAllocated = NULL;

			bool isChanging = _isEditableSpline != NULL;
			checkIntersectVertex();
			//ToDo
			if (isChanging && _isEditableSpline != NULL)
				setChangedSpline(true);
			else
				_splineChanged = true;
		}
		_isEditableSpline = NULL;
		//m_editCreateS->Refresh();
	}
	isAutoCreate = false;

	save(true);
	deselect();
	return false;
}

bool WorldSpline::setChangedSpline(bool splineChanged) {
	_splineChanged = splineChanged;

	if (false == _splineChanged)
		return false;
	_recreateLabel = true;
	if (NULL != _isEditableSpline) {
		_isEditableSpline->recreteSpline();
	}
	else {
		for (TSpline* _tspline : _TSplines) {
			_tspline->recreteSpline();
		}
	}
	return true;
};

Node* WorldSpline::createParent(Node* layer, const Vertex& p1) {
#if 0
	Node* parent = Node::create();
	parent->retain();
	parent->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	//parent->setPosition(p1.getPosText());
#else
	if (_parentWOBJ == NULL) {
		_parentWOBJ = Node::create();
		//parentWOBJ->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		layer->addChild(_parentWOBJ, getZ() + 2);
}

	Node* parent = Node::create();
	parent->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	parent->setPosition(p1.getPos());
	_parentWOBJ->addChild(parent);
#endif
	return parent;
}

#pragma warning (push)
#pragma warning (disable:4996)

void WorldSpline::setColorText(Color3B color, bool reset /*= false*/) {
	_color_text = color;
	if (!reset)return;
	//удалёем текст который пересаздатся при отрисовки
	for (TSpline* _tspline : _TSplines) {
		for (Vertex* p1 : _tspline->getControl()) {
			Node* parent = p1->node;
			if (NULL == parent)continue;
			parent->removeFromParent();
			p1->node = NULL;
		}
	}
}

Node* WorldSpline::printText(Node* layer, Vertex* p1, int id, bool forceClear) {
	Node* parent = p1->node;
	if (p1->hasAssistant()) {
		if (NULL == parent)return NULL;
		Node* t_nodeText = parent->getChildByName(NAME_CHILD_TEXT);
		if (NULL != t_nodeText)
			parent->removeChild(t_nodeText);
		p1->node->removeFromParent();
		p1->node = NULL;
		return NULL;
	}

	if (NULL == parent)
		parent = createParent(layer, *p1);
	parent->setPosition(p1->getPos());

	Node* nodeText = parent->getChildByName(NAME_CHILD_TEXT);
	if (forceClear && NULL != nodeText) {
		parent->removeChild(nodeText);
		nodeText = NULL;
	}

	if (NULL != nodeText) {
		nodeText->setPosition(p1->getPosText());
		return nodeText;
	}

	nodeText = Node::create();
	nodeText->setPosition(p1->getPosText());
	nodeText->setCascadeOpacityEnabled(true);
	char str[100]{};
	//sprintf( str, "%.1f\n%.1f", p1->getAngleToTail(), p1->getAngleToHead() );
	//sprintf( str, "%.1f", p1->getAngleToHead() );
	//sprintf(str, "%.1f\nto %.1f", p1->getAngle(), p1->getAngleToHead());
	sprintf(str, "%i", id);
	auto label = Label::createWithTTF(str, FontCirceBold, 30);
	label->setTextColor(Color4B(getColorText()));

	label->setAnchorPoint(Vec2(0, 0));
	nodeText->addChild(label, 1);

	parent->addChild(nodeText, 1, NAME_CHILD_TEXT);
	p1->node = parent;
	return nodeText;
}



void WorldSpline::draw() {
	Vertex* v2 = NULL;
	for (TSpline* _tspline : _TSplines) {
		v2 = NULL;
		Vertex* first = NULL;
#if 1
		for (Vertex* v1 : _tspline->getSpline()) {
			if (NULL == first)
				first = v1;
			v1->Draw(false, _isEdit);
			//v1->Draw(false, true);
			if (NULL != v2) {
				Vertex::Draw(v2, v1, false, true, _isEdit);
			}
			v2 = v1;
			//printText(parent, v1, i, _recreateLabel);
		}

		if (_tspline->IsClosed()) {
			Vertex::Draw(v2, first, false, true, _isEdit);
		}
#endif
		//###################################
		v2 = NULL;
		first = NULL;
		int i = 0;
		for (Vertex* v1 : _tspline->getControl()) {
			i++;
			if (NULL == first)
				first = v1;
			v1->Draw(true, _isEdit);
			if (NULL != v2) {
				Vertex::Draw(v2, v1, false, false, _isEdit);
			}
			v2 = v1;
			//todo
			//printText(_layer, v1, i, _recreateLabel);
		}

		if (_tspline->IsClosed()) {
			Vertex::Draw(v2, first, false, false, _isEdit);
		}
	}

	if (!_TSplines.empty())
		_recreateLabel = false;
}


#pragma warning (pop)
