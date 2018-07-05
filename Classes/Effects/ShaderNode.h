#pragma once
#include "stdafx.h"

class ShaderNode : public Node
{
public:
	CREATE_FUNC( ShaderNode );
	static ShaderNode* shaderNodeWithVertex( const std::string &vert, const std::string &frag );

	virtual void update( float dt ) override;
	virtual void draw( cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags ) override;

	void setResolution( float width, float height ) { _resolution = Vec2( width, height ); };

protected:
	ShaderNode();
	~ShaderNode();

	bool initWithVertex( const std::string &vert, const std::string &frag );
	void loadShaderVertex( const std::string &vert, const std::string &frag );

	void onDraw( const cocos2d::Mat4& transform, uint32_t flags );

	cocos2d::Vec2 _center;
	cocos2d::Vec2 _resolution;
	float      _time;
	std::string _vertFileName;
	std::string _fragFileName;
	cocos2d::CustomCommand _customCommand;
};