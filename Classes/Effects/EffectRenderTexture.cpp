#include "stdafx.h"
#include "EffectRenderTexture.h"

static bool _isAntiAlias = true;

EffectRenderTexture::EffectRenderTexture():
	_effectCommand(),
	_defaultState(NULL),
	_defaultEffect(NULL),
	_textureBB(NULL),
	RenderTexture() {}

EffectRenderTexture::~EffectRenderTexture() {
	CC_SAFE_RELEASE(_defaultEffect);
	CC_SAFE_RELEASE(_textureBB);
	CC_SAFE_DELETE_ARRAY(_bb_tempData);
	CC_SAFE_DELETE_ARRAY(_bb_buffer);
}

EffectRenderTexture* EffectRenderTexture::create(int w, int h, Texture2D::PixelFormat format, GLuint depthStencilFormat) {
	auto ret = new ( std::nothrow ) EffectRenderTexture;
	if (ret && ret->initWithWidthAndHeight(w, h, format, depthStencilFormat)) {
		if (_isAntiAlias)
			ret->getTexture()->setAntiAliasTexParameters();
		else
			ret->getTexture()->setAliasTexParameters();
		ret->autorelease();
		return ret;
	}
	CC_SAFE_RELEASE(ret);
	return nullptr;
}

void EffectRenderTexture::setEffect(Effect *effect) {
	if (_defaultEffect != effect) {
		_defaultState = getGLProgramState();

		CC_SAFE_RELEASE(_defaultEffect);
		_defaultEffect = effect;
		CC_SAFE_RETAIN(_defaultEffect);

		setGLProgramState(_defaultEffect->getGLProgramState());
	}
}

void EffectRenderTexture::disableEffect() {
	if (_defaultEffect == NULL)
		return;
	CC_SAFE_RELEASE(_defaultEffect);
	_defaultEffect = NULL;
	setGLProgramState(_defaultState);
}

void EffectRenderTexture::prepareDraw() {
	if (NULL != _defaultEffect) {
		if (_defaultEffect->prepareForRender(this))
			setGLProgramState(_defaultState);
	}
}

void EffectRenderTexture::onEnd() {

	if (_textureBB != NULL) {
		//_bb_tempData[0] = 0;
		memset(_bb_tempData, 0, _bb_dataLen);
		//_bb_tempData = new ( std::nothrow ) GLubyte[_bb_dataLen];

		int savedBufferHeight = _textureBB->getPixelsHigh();
		int savedBufferWidth = _textureBB->getPixelsWide();

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, savedBufferWidth, savedBufferHeight, GL_RGBA, GL_UNSIGNED_BYTE, _bb_tempData);

		if (false) // -- flipY
		{
			for (int i = 0; i < savedBufferHeight; ++i) {
				memcpy(&_bb_buffer[i * savedBufferWidth * 4],
					   &_bb_tempData[( savedBufferHeight - i - 1 ) * savedBufferWidth * 4],
					   savedBufferWidth * 4);
			}
			_textureBB->initWithData(_bb_buffer, _bb_dataLen, _pixelFormat, savedBufferWidth, savedBufferHeight, Size(savedBufferWidth, savedBufferHeight));
			//image->initWithRawData(buffer, savedBufferWidth * savedBufferHeight * 4, savedBufferWidth, savedBufferHeight, 8);
		}
		else {
			_textureBB->initWithData(_bb_tempData, _bb_dataLen, _pixelFormat, savedBufferWidth, savedBufferHeight, Size(savedBufferWidth, savedBufferHeight));
			//image->initWithRawData(tempData, savedBufferWidth * savedBufferHeight * 4, savedBufferWidth, savedBufferHeight, 8);
		}

		//CC_SAFE_DELETE_ARRAY(_bb_tempData);

	}

	//RenderTexture::onEnd();

	Director *director = Director::getInstance();

	glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);

	// restore viewport
	director->setViewport();
	const auto& vp = Camera::getDefaultViewport();
	glViewport(vp._left, vp._bottom, vp._width, vp._height);
	//
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _oldProjMatrix);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _oldTransMatrix);

	if (nullptr != _callback)
		_callback(this);
};

void EffectRenderTexture::clear() {
	//RenderTexture::clear(0., 0., 0., 0.);
	beginWithClear(0., 0., 0., 0.);
	end();
	//if (NULL != _textureBB)memset(_bb_tempData, 0, _bb_dataLen);
}

void EffectRenderTexture::onClear() {
	RenderTexture::onClear();
};

void EffectRenderTexture::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
	RenderTexture::draw(renderer, transform, flags);

	if (NULL == _defaultEffect) return;

	_effectCommand.init(_globalZOrder);
	_effectCommand.func = CC_CALLBACK_0(EffectRenderTexture::prepareDraw, this);

	Director::getInstance()->getRenderer()->addCommand(&_effectCommand);

	//renderer->render();
}

void EffectRenderTexture::onBegin() {
	//RenderTexture::onBegin();-->
	Director *director = Director::getInstance();

	_oldProjMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	//director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _projectionMatrix);

	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	_oldTransMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	//director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _transformMatrix);

	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	if (!_keepMatrix)
		//if(false)
	{
		director->setProjection(director->getProjection());
		const Size& texSize = _texture->getContentSizeInPixels();

		// Calculate the adjustment ratios based on the old and new projections
		Size size = director->getWinSizeInPixels();
		float widthRatio = size.width / texSize.width;
		float heightRatio = size.height / texSize.height;

		Mat4 orthoMatrix;
		//widthRatio *= .9f;heightRatio *= .9f;
		//Mat4::createOrthographicOffCenter((float) -1.0 / widthRatio, ( float )1.0 / widthRatio, (float) -1.0 / heightRatio, ( float )1.0 / heightRatio, -1, 1, &orthoMatrix);
		Mat4::createOrthographicOffCenter(0, texSize.width, 0, texSize.height, -1, 1, &orthoMatrix);
		//director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);
		director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);
	}

	//calculate viewport
	{
		Rect viewport;
		viewport.size.width = _fullviewPort.size.width;
		viewport.size.height = _fullviewPort.size.height;
		float viewPortRectWidthRatio = float(viewport.size.width) / _fullRect.size.width;
		float viewPortRectHeightRatio = float(viewport.size.height) / _fullRect.size.height;
		viewport.origin.x = ( _fullRect.origin.x - _rtTextureRect.origin.x ) * viewPortRectWidthRatio;
		viewport.origin.y = ( _fullRect.origin.y - _rtTextureRect.origin.y ) * viewPortRectHeightRatio;
		//glViewport(_fullviewPort.origin.x, _fullviewPort.origin.y, (GLsizei) _fullviewPort.size.width, (GLsizei) _fullviewPort.size.height);
		glViewport(viewport.origin.x, viewport.origin.y, (GLsizei) viewport.size.width, (GLsizei) viewport.size.height);
	}

	// Adjust the orthographic projection and viewport

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	// TODO: move this to configuration, so we don't check it every time
	/*  Certain Qualcomm Adreno GPU's will retain data in memory after a frame buffer switch which corrupts the render to the texture. The solution is to clear the frame buffer before rendering to the texture. However, calling glClear has the unintended result of clearing the current texture. Create a temporary texture to overcome this. At the end of RenderTexture::begin(), switch the attached texture to the second one, call glClear, and then switch back to the original texture. This solution is unnecessary for other devices as they don't have the same issue with switching frame buffers.
	*/
	if (Configuration::getInstance()->checkForGLExtension("GL_QCOM")) {
		// -- bind a temporary texture so we can clear the render buffer without losing our texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureCopy->getName(), 0);
		CHECK_GL_ERROR_DEBUG();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture->getName(), 0);
	}
}

void EffectRenderTexture::setEnableBackBuffer(bool enable) {
	if (( NULL == _textureBB ) != enable)return;
	if (!enable) {
		CC_SAFE_RELEASE(_textureBB);
		CC_SAFE_DELETE_ARRAY(_bb_tempData);
		CC_SAFE_DELETE_ARRAY(_bb_buffer);
		_textureBB = NULL;
		return;
	}

	do {
		int w = _fullviewPort.size.width;
		int h = _fullviewPort.size.height;

		// textures must be power of two squared
		int powW = 0;
		int powH = 0;

		if (Configuration::getInstance()->supportsNPOT()) {
			powW = w;
			powH = h;
		}
		else {
			CC_BREAK_IF(true);
			//	powW = cocos2d::ccNextPOT(w);
			//	powH = cocos2d::ccNextPOT(h);
		}

		_bb_dataLen = powW * powH * 4;
		//void* data = malloc(_bb_dataLen);
		_bb_tempData = new ( std::nothrow ) GLubyte[_bb_dataLen];
		CC_BREAK_IF(!_bb_tempData);

		memset(_bb_tempData, 0, _bb_dataLen);
		//memset(data, 0, _bb_dataLen);

		_textureBB = new ( std::nothrow ) Texture2D();
		if (_textureBB) {
			//_textureBB->initWithData(data, _bb_dataLen, ( Texture2D::PixelFormat )_pixelFormat, powW, powH, Size((float) w, (float) h));
			_textureBB->initWithData(_bb_tempData, _bb_dataLen, ( Texture2D::PixelFormat )_pixelFormat, powW, powH, Size((float) w, (float) h));
		}
		else {
			break;
		}
		//CC_SAFE_DELETE_ARRAY(_bb_tempData);
		//CC_SAFE_DELETE_ARRAY(data);

		if (_isAntiAlias)
			_textureBB->setAntiAliasTexParameters();
		else
			_textureBB->setAliasTexParameters();

	}
	while (0);



}

void EffectRenderTexture::beginWithClear(float r, float g, float b, float a, float d) {
	//RenderTexture::beginWithClear(r, g, b, a, d);
	//return;

	setClearColor(Color4F(r, g, b, a));
	setClearDepth(d);
	setClearStencil(0);
	setClearFlags(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	begin();

	//clear screen
	_beginWithClearCommand.init(_globalZOrder);
	_beginWithClearCommand.func = CC_CALLBACK_0(EffectRenderTexture::onClear, this);
	Director::getInstance()->getRenderer()->addCommand(&_beginWithClearCommand);
}

void EffectRenderTexture::begin() {
	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when setting matrix stack");

	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	_projectionMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	_transformMatrix = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	//if (!_keepMatrix)
	if (false) {
		director->setProjection(director->getProjection());

		const Size& texSize = _texture->getContentSizeInPixels();

		// Calculate the adjustment ratios based on the old and new projections
		Size size = director->getWinSizeInPixels();

		float widthRatio = size.width / texSize.width;
		float heightRatio = size.height / texSize.height;

		Mat4 orthoMatrix;
		Mat4::createOrthographicOffCenter((float) -1.0 / widthRatio, ( float )1.0 / widthRatio, (float) -1.0 / heightRatio, ( float )1.0 / heightRatio, -1024, 1024, &orthoMatrix);
		director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);

	}


	_groupCommand.init(_globalZOrder);

	Renderer *renderer = Director::getInstance()->getRenderer();
	renderer->addCommand(&_groupCommand);
	renderer->pushGroup(_groupCommand.getRenderQueueID());

	_beginCommand.init(_globalZOrder);
	_beginCommand.func = CC_CALLBACK_0(EffectRenderTexture::onBegin, this);

	Director::getInstance()->getRenderer()->addCommand(&_beginCommand);
}

void EffectRenderTexture::end() {
	_endCommand.init(_globalZOrder);
	_endCommand.func = CC_CALLBACK_0(EffectRenderTexture::onEnd, this);

	Director* director = Director::getInstance();
	CCASSERT(nullptr != director, "Director is null when setting matrix stack");

	Renderer *renderer = director->getRenderer();
	renderer->addCommand(&_endCommand);
	renderer->popGroup();

	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void EffectRenderTexture::onSaveToFile(const std::string& filename) {
	std::string fullpath = FileUtils::getInstance()->getWritablePath();
	Image *image = newImage(true);
	if (image)
		image->saveToFile(fullpath + filename, false);
	CC_SAFE_DELETE(image);

	Image *image_back = new ( std::nothrow ) Image();

	int savedBufferHeight = _textureBB->getPixelsHigh();
	int savedBufferWidth = _textureBB->getPixelsWide();
	//image_back->initWithData(_bb_tempData, _bb_dataLen, _pixelFormat, savedBufferWidth, savedBufferHeight, Size(savedBufferWidth, savedBufferHeight));
	
	image_back->initWithRawData(_bb_tempData, _bb_dataLen, savedBufferWidth, savedBufferHeight, 4, true);
	//image_back->initWithImageData(_bb_tempData, _bb_dataLen);

	//image_back->initWithRawData(_bb_tempData, _bb_dataLen, _pixelFormat, savedBufferWidth, savedBufferHeight, Size(savedBufferWidth, savedBufferHeight));
	image_back->saveToFile(fullpath + "_back.png", false);

	CC_SAFE_DELETE(image_back);

}

void EffectRenderTexture::saveToFile2(const std::string& fileName) {

	if (NULL == _textureBB)return;
	//std::string fullpath = FileUtils::getInstance()->getWritablePath() + fileName;
	_saveToFileCommand.init(_globalZOrder);
	_saveToFileCommand.func = CC_CALLBACK_0(EffectRenderTexture::onSaveToFile, this, fileName);

	Director::getInstance()->getRenderer()->addCommand(&_saveToFileCommand);
}
