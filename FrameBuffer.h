#pragma once

#include <d3d11.h>
#include "GraphicsSystem.h"

class FrameBuffer
{
public:
	explicit FrameBuffer(int width = GraphicsSystem::GetInstance().GetDisplayWidth(), int height = GraphicsSystem::GetInstance().GetDisplayHeight());
	~FrameBuffer();

	void Set(bool depthOnly = false);
	void Unset();

	ID3D11ShaderResourceView *GetColorBuffer() { return mColorBufferShaderResourceView; }
	ID3D11ShaderResourceView *GetDepthStencilBuffer() { return mDepthStencilBufferShaderResourceView; }
	
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
private:
	int mWidth;
	int mHeight;

	ID3D11Texture2D * mColorBuffer;
	ID3D11RenderTargetView *mColorBufferRenderTargetView;
	ID3D11ShaderResourceView *mColorBufferShaderResourceView;
	
	ID3D11Texture2D * mDepthStencilBuffer;
	ID3D11DepthStencilView *mDepthStencilView;
	ID3D11ShaderResourceView *mDepthStencilBufferShaderResourceView;
	
	ID3D11RenderTargetView *mOldRenderTargetView;
	ID3D11DepthStencilView *mOldDepthStencilView;
	
	D3D11_VIEWPORT mViewport;
	D3D11_VIEWPORT mOldViewport;
};

