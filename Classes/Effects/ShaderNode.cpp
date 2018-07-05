#include "stdafx.h"
#include "ShaderNode.h"


ShaderNode::ShaderNode()
	:_center( Vec2( 0.0f, 0.0f ) )
	, _resolution( Vec2( 256, 256 ) )
	, _time( 0.0f )
{}

ShaderNode::~ShaderNode()
{}

ShaderNode* ShaderNode::shaderNodeWithVertex( const std::string &vert, const std::string& frag )
{
	auto node = new (std::nothrow) ShaderNode();
	node->initWithVertex( vert, frag );
	node->autorelease();

	return node;
}

bool ShaderNode::initWithVertex( const std::string &vert, const std::string &frag )
{
	_vertFileName = vert;
	_fragFileName = frag;
#if CC_ENABLE_CACHE_TEXTURE_DATA
	auto listener = EventListenerCustom::create( EVENT_RENDERER_RECREATED, [this]( EventCustom* event )
	{
		this->setGLProgramState( nullptr );
		loadShaderVertex( _vertFileName, _fragFileName );
	} );

	_eventDispatcher->addEventListenerWithSceneGraphPriority( listener, this );
#endif

	loadShaderVertex( vert, frag );

	_time = 0;

	scheduleUpdate();

	setContentSize( Size( _resolution.x, _resolution.y ) );
	setAnchorPoint( Vec2( 0.5f, 0.5f ) );


	return true;
}

void ShaderNode::loadShaderVertex( const std::string &vert, const std::string &frag )
{
	auto fileUtiles = FileUtils::getInstance();

	// frag
	auto fragmentFilePath = fileUtiles->fullPathForFilename( frag );
	auto fragSource = fileUtiles->getStringFromFile( fragmentFilePath );

	// vert
	std::string vertSource;
	if (vert.empty())
	{
		vertSource = ccPositionTextureColor_vert;

		//vertSource = ccPositionTextureColor_noMVP_vert;
	}
	else
	{
		std::string vertexFilePath = fileUtiles->fullPathForFilename( vert );
		vertSource = fileUtiles->getStringFromFile( vertexFilePath );
	}

	auto glprogram = GLProgram::createWithByteArrays( vertSource.c_str(), fragSource.c_str() );
	auto glprogramstate = GLProgramState::getOrCreateWithGLProgram( glprogram );
	setGLProgramState( glprogramstate );
}

void ShaderNode::update( float dt )
{
	_time += dt;
}

void ShaderNode::draw( Renderer *renderer, const Mat4 &transform, uint32_t flags )
{
	_customCommand.init( _globalZOrder, transform, flags );
	_customCommand.func = CC_CALLBACK_0( ShaderNode::onDraw, this, transform, flags );
	renderer->addCommand( &_customCommand );
}

void ShaderNode::onDraw( const Mat4 &transform, uint32_t flags )
{
	auto position = getPosition();
	position = convertToWindowSpace( position );
	auto frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto retinaFactor = Director::getInstance()->getOpenGLView()->getRetinaFactor();
	_center = Vec2( position.x * frameSize.width / visibleSize.width * retinaFactor, position.y * frameSize.height / visibleSize.height * retinaFactor );

	//_center = position;

	Vec2 size = MathUtils::ConvertToFirstSpace_ignoreRS( this, _resolution );
	//float w = size.x, h = size.y;
	float w = _resolution.x, h = _resolution.y;
	GLfloat vertices[12] = { 0,0, w,0, w,h, 0,0, 0,h, w,h };

	auto glProgramState = getGLProgramState();
	glProgramState->setUniformVec2( "resolution", size );// _resolution );
	glProgramState->setUniformVec2( "center", _center );
	glProgramState->setVertexAttribPointer( "a_position", 2, GL_FLOAT, GL_FALSE, 0, vertices );

	glProgramState->apply( transform );

	glDrawArrays( GL_TRIANGLES, 0, 6 );

	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES( 1, 6 );
}