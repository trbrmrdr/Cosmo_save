#pragma once
#include "stdafx.h"

class TabButton : public Button
{
private:
	bool isChecked;
	std::string titleName;
public:
	TabButton();

	static TabButton* create( const std::string& normalImage, const std::string& selectedImage, bool simpleButton );

	static TabButton* create( const std::string& normalImage, const std::string& selectedImage = "", const std::string& disableImage = "", TextureResType texType = TextureResType::LOCAL, bool check = false );

	bool isCheck();

	void setText( const std::string& name );


	void onCheck( bool pressed,bool force = false );
	//virtual void onPressStateChangedToNormal() { Button::onPressStateChangedToNormal(); };
	//virtual void onPressStateChangedToPressed() { Button::onPressStateChangedToPressed(); };
	//virtual void onPressStateChangedToDisabled() { Button::onPressStateChangedToDisabled(); };
};
