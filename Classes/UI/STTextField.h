#pragma once
#include "stdafx.h"


enum STTextField_Type
{
	Type_String = -1,
	Type_Float,
	Type_Point,
	Type_UInt
};

class STTextField : public TextField
{
private:
	STTextField_Type type;
	bool autoScale;
	std::string lastText;
	size_t pos;
	size_t getTextCount();
	EventListenerKeyboard* keyboardListener;
	ccTextFieldCallback callback;
	ccTextFieldCallback callbackAfterCursor;
	Vec2 startPos;
	std::string startVal;
	std::string accomulateVal;
	bool inPos;
	bool onTouch;

	std::string minVal;
	std::string maxVal;
	std::string fixMinMax( const std::string& newText );


	float m_change_dx;
	float m_change_dy;
public:

	STTextField();
	virtual ~STTextField();

	static STTextField* create( const std::string& placeholder,
								const std::string& fontName,
								int fontSize, bool _autoScale = true,
								float dx = 0,float dy = 0);

	void deleteFromPos();
	void setString( const std::string& text );
	void setAutoScale( bool _autoScale ) { autoScale = _autoScale; }
	void setType( STTextField_Type _type ) { type = _type; }


	virtual bool init();
	virtual bool onTouchBegan( Touch *touch, Event *unusedEvent );
	virtual void onTouchMoved( Touch *touch, Event *unusedEvent );
	virtual void onTouchEnded( Touch *touch, Event *unusedEvent );

	virtual void addEventListener( const ccTextFieldCallback& callback );
	virtual void setFontSize( int size );
	virtual void onEnter();
	virtual void onExit();

	void updateCursor();

	size_t getStringToSizeT() { return convertToSizeT( getString() ); }
	float getStringToFloat() { return convertToFloat( getString() ); }
	Vec2 getStringToVec2() { return convertToVec2( getString() ); }

	size_t convertToSizeT( const std::string& val );
	float convertToFloat( const std::string& val );
	Vec2 convertToVec2( const std::string& val );


	void setValue( const size_t& val ) { setString( SetValue( val ) ); }
	void setValue( const Vec2& val ) { setString( SetValue( val ) ); }
	void setValue( const float& val ) { setString( SetValue( val ) ); }
	void setValue( const std::string& val ) { setString( val ); }

	void setMinMax( const std::string& min = "", const std::string& max = "" );

	void setDChanged(float dx, float dy);

	static std::string SetValue(const size_t& val) { return StringUtils::format("%u", val); }
	static std::string SetValue(const Vec2& val) { return StringUtils::format("%.2f;%.2f", val.x, val.y); }
	static std::string SetValue(const float& val) { return StringUtils::toString(val); }
protected:
	cocos2d::Label* _cursorLabel;
};
