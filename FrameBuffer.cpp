#include "FrameBuffer.h"
#include "GraphicsSystem.h"
#include "error.h"

FrameBuffer::FrameBuffer(int width, int height) : mWidth(width), mHeight(height)
{
	HRESULT hr;

	/**** create a Texture2D to use as a color buffer ****/

	// texture 2D resource description
	D3D11_TEXTURE2D_DESC colorBufferTexture2DDesc;
	colorBufferTexture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	colorBufferTexture2DDesc.Width = mWidth;
	colorBufferTexture2DDesc.Height = mHeight;
	colorBufferTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	colorBufferTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	colorBufferTexture2DDesc.CPUAccessFlags = 0;
	colorBufferTexture2DDesc.ArraySize = 1;
	colorBufferTexture2DDesc.MipLevels = 1;
	colorBufferTexture2DDesc.SampleDesc.Count = 1;
	colorBufferTexture2DDesc.SampleDesc.Quality = 0;
	colorBufferTexture2DDesc.MiscFlags = 0;
	
	// create Texture2D resource
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateTexture2D(&colorBufferTexture2DDesc, nullptr, &mColorBuffer);
	if (FAILED(hr))
		ErrorBox("unable to create texture for framebuffer's color buffer");

	// render target view description
	D3D11_RENDER_TARGET_VIEW_DESC colorBufferRenderTargetViewDesc;
	//colorBufferRenderTargetViewDesc.Buffer.ElementOffset;
	colorBufferRenderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	colorBufferRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	colorBufferRenderTargetViewDesc.Texture2D.MipSlice = 0;

	// create render target view
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateRenderTargetView(mColorBuffer, &colorBufferRenderTargetViewDesc, &mColorBufferRenderTargetView);
	if (FAILED(hr))
		ErrorBox("unable to create render target view for color buffer");

	// shader resource view description 
	D3D11_SHADER_RESOURCE_VIEW_DESC colorBufferShaderResourceViewDesc;
	//colorBufferShaderResourceViewDesc.Buffer;
	//colorBufferShaderResourceViewDesc.BufferEx;
	colorBufferShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	colorBufferShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	colorBufferShaderResourceViewDesc.Texture2D.MipLevels = 1;
	colorBufferShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	// create shader resource view
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateShaderResourceView(mColorBuffer, &colorBufferShaderResourceViewDesc, &mColorBufferShaderResourceView);
	if (FAILED(hr))
		ErrorBox("unable to create shader resource view for color buffer");

	/**** create a Texture2D to use ad a depth stencil buffer ****/
	
	// Texture2D resource description
	D3D11_TEXTURE2D_DESC depthStencilBufferTexture2DDesc;
	depthStencilBufferTexture2DDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilBufferTexture2DDesc.Width = mWidth;
	depthStencilBufferTexture2DDesc.Height = mHeight;
	depthStencilBufferTexture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilBufferTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferTexture2DDesc.CPUAccessFlags = 0;
	depthStencilBufferTexture2DDesc.ArraySize = 1;
	depthStencilBufferTexture2DDesc.MipLevels = 1;
	depthStencilBufferTexture2DDesc.SampleDesc.Count = 1;
	depthStencilBufferTexture2DDesc.SampleDesc.Quality = 0;
	depthStencilBufferTexture2DDesc.MiscFlags = 0;

	// create Texture2D resource
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateTexture2D(&depthStencilBufferTexture2DDesc, nullptr, &mDepthStencilBuffer);
	if (FAILED(hr))
		ErrorBox("unable to create texture for framebuffer's depth stencil buffer");

	// create depth stencil view 
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilBufferDepthStencilViewDesc;
	depthStencilBufferDepthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilBufferDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDepthStencilViewDesc.Flags = 0;
	depthStencilBufferDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilBufferDepthStencilViewDesc, &mDepthStencilView);
	if (FAILED(hr))
		ErrorBox("unable to create depth stencil view for depth stencil buffer");

	// create shader resource view 
	D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilBufferShaderResourceViewDesc;
	depthStencilBufferShaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthStencilBufferShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthStencilBufferShaderResourceViewDesc.Texture2D.MipLevels = 1;
	depthStencilBufferShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateShaderResourceView(mDepthStencilBuffer, &depthStencilBufferShaderResourceViewDesc, &mDepthStencilBufferShaderResourceView);
	if (FAILED(hr))
		ErrorBox("unable to create shader resource view for depth stencil buffer");

	// create a viewport with the framebuffer dimensions
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = mWidth;
	mViewport.Height = mHeight;
	mViewport.MinDepth = 0.0;
	mViewport.MaxDepth = 1.0;   
}

FrameBuffer::~FrameBuffer()
{
	// release resources if framebuffer hasn't been unset
	if (mColorBuffer)
		mColorBuffer->Release();

	if (mDepthStencilBuffer)
		mDepthStencilBuffer->Release();
}

void FrameBuffer::Set(bool depthOnly)
{
	// save the screen render target view and depth stencil view (increment reference count)
	GraphicsSystem::GetInstance().GetDeviceContext()->OMGetRenderTargets(1, &mOldRenderTargetView, &mOldDepthStencilView);

	// save viewport
	unsigned int numViewports = 1;
	GraphicsSystem::GetInstance().GetDeviceContext()->RSGetViewports(&numViewports, &mOldViewport);

	// set new render target view and depth stencil view
	if (depthOnly)
	{
		ID3D11RenderTargetView *nullRenderTarget = nullptr;
		GraphicsSystem::GetInstance().GetDeviceContext()->OMSetRenderTargets(1, &nullRenderTarget, mDepthStencilView);
	}
	else
		GraphicsSystem::GetInstance().GetDeviceContext()->OMSetRenderTargets(1, &mColorBufferRenderTargetView, mDepthStencilView);

	// set new viewport
	GraphicsSystem::GetInstance().GetDeviceContext()->RSSetViewports(1, &mViewport);

	// clear targets
	const float color[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	GraphicsSystem::GetInstance().GetDeviceContext()->ClearRenderTargetView(mColorBufferRenderTargetView, color);
	GraphicsSystem::GetInstance().GetDeviceContext()->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FrameBuffer::Unset()
{
	// restore render target view and depth stencil view
	GraphicsSystem::GetInstance().GetDeviceContext()->OMSetRenderTargets(1, &mOldRenderTargetView, mOldDepthStencilView);

	// release interfaces (decrement reference count)
	mOldRenderTargetView->Release();
	mOldDepthStencilView->Release();
	mOldRenderTargetView = nullptr;
	mOldDepthStencilView = nullptr;

	// restore viewport
	GraphicsSystem::GetInstance().GetDeviceContext()->RSSetViewports(1, &mOldViewport);
}
