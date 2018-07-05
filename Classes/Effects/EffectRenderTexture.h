#pragma once
#include "stdafx.h"
#include "Effect.h"


class EffectRenderTexture: public cocos2d::RenderTexture {
public:

	static EffectRenderTexture* create(int w, int h, Texture2D::PixelFormat format, GLuint depthStencilFormat = 0);

	void setEffect(Effect *effect);

	void disableEffect();

	void setEnableBackBuffer(bool enable);
	virtual void onEnd();
	virtual void onBegin();
	void saveToFile2(const std::string& filename);

	void onClear();

	Texture2D* getTexture() { return _texture; }
	Texture2D* getBackTexture() { return _textureBB; }
	void setTexture(Texture2D* texture) { _texture = texture; }


	void clear();

	virtual void beginWithClear(float r = .0f, float g = .0f, float b = .0f, float a = .0f, float d = .0f);
	virtual void begin();
	virtual void end();


	void setEndCallback(const callback_render_texture& callback) { _callback = callback; }

protected:

	void onSaveToFile(const std::string& filename);

	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
	void prepareDraw();

	EffectRenderTexture();
	virtual ~EffectRenderTexture();

	Effect* _defaultEffect;
	GLProgramState* _defaultState;

	CustomCommand _effectCommand;

	callback_render_texture _callback = nullptr;

	GLubyte *_bb_buffer = NULL;
	GLubyte *_bb_tempData = NULL;
	int _bb_dataLen = 0;

	Texture2D* _textureBB;
};


