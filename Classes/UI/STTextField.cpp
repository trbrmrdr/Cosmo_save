#include "stdafx.h"
#include "STTextField.h"

USING_NS_CC;

STTextField::STTextField() :
	TextField(),

	_cursorLabel(NULL),
	pos(0),
	callbackAfterCursor(nullptr),
	keyboardListener(NULL),
	type(Type_String),
	onTouch(false),
	startVal(),
	inPos(false),
	minVal(),
	maxVal(),
	m_change_dx(.0),
	m_change_dy(.0),
	accomulateVal()
{}

STTextField::~STTextField() {}

bool STTextField::init()
{
	if (!TextField::init())
		return false;

	_cursorLabel = Label::createWithTTF("|", FontCirceBold, 1);
	if (!_cursorLabel)
		return false;
	auto seq = Sequence::create(FadeOut::create(0.5), FadeIn::create(0.25), nullptr);
	_cursorLabel->runAction(RepeatForever::create(seq));
	_cursorLabel->setTag(kTagMenuAcceptDecline);
	this->addChild(_cursorLabel, 10);

	_cursorLabel->setVisible(false);

	//addEventListener( 
	callback = ([this](Ref* ref, EventType etype)
	{
		if (etype == EventType::ATTACH_WITH_IME)
		{
			_cursorLabel->setVisible(true);

			keyboardListener = EventListenerKeyboard::create();
			keyboardListener->onKeyReleased = ([this](EventKeyboard::KeyCode keyCode, Event* event)
			{

				if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
				{
					if ((int)(pos - 1) >= 0)
						pos = pos - 1;
				}
				else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
				{
					pos = MIN(getTextCount(), pos + 1);
				}
				updateCursor();
			});

			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
		}
		else if (etype == EventType::DETACH_WITH_IME)
		{
			_cursorLabel->setVisible(false);

			Director::getInstance()->getEventDispatcher()->removeEventListener(keyboardListener);
			keyboardListener = NULL;
		}
		else if (etype == EventType::INSERT_TEXT)
		{
			inPos = true;
			setString(getString());
		}
		else if (etype == EventType::DELETE_BACKWARD)
		{
			deleteFromPos();
		}
		if (autoScale)
		{
			Size size = getContentSize();
			setScale(1, 1);

			Rect tmp = getBoundingBox();
			float sx = size.width / tmp.size.width;
			float sy = size.height / tmp.size.height;
			setScale(MAX(MIN(sx, sy), 1));
		}
		if (nullptr != callbackAfterCursor)
			callbackAfterCursor(ref, etype);
	});
	return true;
}

void STTextField::setFontSize(int size)
{
	TextField::setFontSize(size);
	_cursorLabel->setSystemFontSize(size);
}

void STTextField::addEventListener(const ccTextFieldCallback& callback)
{
	callbackAfterCursor = callback;
}

STTextField* STTextField::create(const std::string& placeholder,
	const std::string& fontName,
	int fontSize, bool _autoScale,
	float dx, float dy)
{
	STTextField* widget = new (std::nothrow) STTextField();
	if (widget && widget->init())
	{
		widget->setFontName(fontName);
		widget->setFontSize(fontSize);
		widget->setPlaceHolder(placeholder);
		widget->setAutoScale(_autoScale);
		widget->setDChanged(dx, dy);
		widget->autorelease();

		return widget;
	}
	CC_SAFE_DELETE(widget);
	return nullptr;
}

void STTextField::onEnter()
{
	_eventCallback = callback;
	TextField::onEnter();
}

void STTextField::onExit()
{
	_eventCallback = nullptr;
	_cursorLabel->setVisible(false);

	Director::getInstance()->getEventDispatcher()->removeEventListener(keyboardListener);
	keyboardListener = NULL;

	didNotSelectSelf();

	TextField::onExit();
}

bool STTextField::onTouchBegan(Touch *touch, Event *unusedEvent)
{
	onTouch = TextField::onTouchBegan(touch, unusedEvent);
	if (!onTouch)
	{
		return false;
	}

	//CCLOG( "onTouchBegan" );
	accomulateVal.clear();
	if (type == Type_Float || type == Type_Point || type == Type_UInt)
	{
		startPos = touch->getLocation();
		startVal = getString();
	}
	else
	{
		onTouch = false;
	}
	auto camera = Camera::getVisitingCamera();
	if (hitTest(_touchBeganPosition, camera, nullptr))
	{
		auto lbl = dynamic_cast<Label*>(getVirtualRenderer());
		if (lbl)
		{
			int count = MAX(1, strlen(_textFieldRenderer->getString().c_str()));
			float width = lbl->getBoundingBox().size.width;
			float dx = width / count;
			float tx = this->convertToNodeSpace(_touchBeganPosition).x;
			pos = (int)(tx / dx);
			float w1 = pos * dx;
			if (tx - w1 >= dx * 0.5)
				pos = pos + 1;
			updateCursor();
		}
	}

	return true;
}

void STTextField::onTouchMoved(Touch *touch, Event *unusedEvent)
{
	TextField::onTouchMoved(touch, unusedEvent);
	if (onTouch)
	{
		Vec2 dl = startPos - touch->getLocation();
		bool t_call_callback = true;
		if (type == Type_Float)
		{
			float t_accomulate = dl.x / 2;
			if (m_change_dx > .0) {
				t_accomulate += convertToFloat(accomulateVal);
				if (ABS(t_accomulate) <= ABS(m_change_dx)) {
					accomulateVal = SetValue(t_accomulate);
					t_call_callback = false;
					//return;
				}
			}
			float start_v = convertToFloat(startVal);
			float tmp_val = start_v + t_accomulate;
			inPos = false;
			setValue(tmp_val);
		}
		else if (type == Type_UInt)
		{
			float t_accomulate = dl.x / 4;
			if (m_change_dx > .0) {
				t_accomulate += convertToSizeT(accomulateVal);
				if (ABS(t_accomulate) <= ABS(m_change_dx)) {
					accomulateVal = SetValue(t_accomulate);
					t_call_callback = false;
					//return;
				}
			}
			size_t value = convertToSizeT(startVal);
			float tmp_val = ABS(value + t_accomulate);
			inPos = false;
			setValue((size_t)tmp_val);
		}
		else if (type == Type_Point)
		{
			Vec2 t_accomulate = Vec2(dl.x / 2, dl.y / 2);
			if (m_change_dx > .0 || m_change_dy > .0) {
				t_accomulate += convertToVec2(accomulateVal);
				if (ABS(t_accomulate.x) <= ABS(m_change_dx)) {
					accomulateVal = SetValue(t_accomulate);
					t_call_callback = false;
					//return;
				}
				if (ABS(t_accomulate.y) <= ABS(m_change_dy)) {
					accomulateVal = SetValue(t_accomulate);
					t_call_callback = false;
					//return;
				}
			}
			Vec2 value = convertToVec2(startVal);
			Vec2 tmp_val = value + t_accomulate;
			inPos = false;
			setValue(tmp_val);

		}
		if (t_call_callback && nullptr != callbackAfterCursor)
			callbackAfterCursor(this, EventType::INSERT_TEXT);
	}
}

void STTextField::onTouchEnded(Touch *touch, Event *unusedEvent)
{
	TextField::onTouchEnded(touch, unusedEvent);
	if (onTouch)
	{
		startVal.clear();
		onTouch = false;
	}
}

size_t STTextField::getTextCount()
{
	return strlen(_textFieldRenderer->getString().c_str());
}

void STTextField::deleteFromPos()
{
	if ((int)(pos - 1) >= 0)
	{
		pos = pos - 1;
		lastText = lastText.erase(pos, 1);
	}
	TextField::setString(lastText);

	updateCursor();
}

void STTextField::setString(const std::string& text)
{
	bool force_end = false;
	std::string filter_text = std::string(text);
	if (type == Type_Float || type == Type_Point || type == Type_UInt)
	{
		for (size_t i = 0; i < filter_text.length();)
		{
			char ch = filter_text[i];
			bool onlyNumber = ch >= '0' && ch <= '9';
			if (type == Type_UInt && onlyNumber)
				i++;
			else
				if (onlyNumber
					|| ch == '.' || ch == ',' || ch == '-'
					|| (type == Type_Point && ch == ';')
					)
					i++;
				else
				{
					filter_text = filter_text.erase(i, 1);
				}
		}
		if (filter_text == lastText)
		{
			force_end = true;
		}
	}

	if (!force_end)
	{
		size_t len = filter_text.length();
		if (inPos && pos != getTextCount() && len > 0)
		{
			std::string newText = std::string(lastText);
			newText.insert(pos, 1, filter_text[len - 1]);
			TextField::setString(newText);
			pos += 1;
		}
		else
		{
			force_end = true;
		}
	}

	if (force_end)
	{
		filter_text = fixMinMax(filter_text);
		TextField::setString(filter_text);
		pos = getTextCount();
	}
	inPos = false;
	lastText = getString();
	updateCursor();
}

size_t STTextField::convertToSizeT(const std::string& val) { return (size_t)atoi(val.c_str()); }
float STTextField::convertToFloat(const std::string& val) { return atof(val.c_str()); }

Vec2 STTextField::convertToVec2(const std::string& val)
{
	size_t pos = val.find(';', 0);
	if (pos == 0)
		return Vec2::ZERO;
	char tmp[20] = { 0 };
	val.copy(tmp, pos - 1, 0);
	float x = std::atof(tmp);
	tmp[0] = 0;
	val.copy(tmp, val.length() - pos + 1, pos + 1);
	float y = std::atof(tmp);
	return Vec2(x, y);
}

void STTextField::updateCursor()
{
	auto lbl = dynamic_cast<Label*>(getVirtualRenderer());
	if (!lbl) return;

	int count = MAX(1, strlen(_textFieldRenderer->getString().c_str()));

	float dx = lbl->getBoundingBox().size.width / count;

	auto px = dx * pos + 2.f;
	auto py = this->getBoundingBox().size.height * .5f;
	_cursorLabel->setPosition(px, py);
}


std::string STTextField::fixMinMax(const std::string& newText)
{
	std::string ret = newText;

	if (type == Type_Float)
	{
		float value = convertToFloat(newText);
		bool isMin = !minVal.empty();
		bool isMax = !maxVal.empty();
		float min = isMin ? convertToFloat(minVal) : 0;
		float max = isMax ? convertToFloat(maxVal) : 0;
		if (isMin && value < min)
		{
			value = isMax ? max : min;
			startVal = isMax ? maxVal : minVal;
		}
		else if (isMax && value > max)
		{
			value = isMin ? min : max;
			startVal = isMin ? minVal : maxVal;
		}
		ret = StringUtils::format("%f", value);
	}

	return ret;
}


void STTextField::setMinMax(const std::string& min, const std::string& max)
{
	if (!min.empty())
	{
		minVal = std::string(min);
	}
	if (!max.empty())
	{
		maxVal = std::string(max);
	}
}

void STTextField::setDChanged(float dx, float dy) {
	m_change_dx = dx;
	m_change_dy = dy;

}

