#include "GraphicsSystem.h"
#include "Error.h"

GraphicsSystem &GraphicsSystem::GetInstance()
{
	static GraphicsSystem instance;

	return instance;
}

#define DEBUG

void GraphicsSystem::Initialize(HWND window)
{
	HRESULT hr;

	/* check DirectX11 support and create device and device context */
	D3D_FEATURE_LEVEL featureLevel;
#ifdef DEBUG
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG, nullptr, 0, D3D11_SDK_VERSION, &mDevice, &featureLevel, &mDeviceContext);
#else
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, nullptr, 0, D3D11_SDK_VERSION, &mDevice, &featureLevel, &mDeviceContext);
#endif
	if (FAILED(hr))
		ErrorBox("cannot create device");
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		ErrorBox("DirectX 11 not supported");

	/* check Multi Sampling support */
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 4;
	mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleDesc.Count, &sampleDesc.Quality);
	if (sampleDesc.Quality <= 0)
		ErrorBox("MSAA not supported");

	mSampleCount = sampleDesc.Count;
	mSampleQuality = sampleDesc.Quality - 1;

	/* get window area */
	RECT rect;
	GetClientRect(window, &rect);

	mDisplayWidth = rect.right;
	mDisplayHeight = rect.bottom;

	/* describe the swap chain */
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	DXGI_MODE_DESC modeDesc = {};
	modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	modeDesc.Width = mDisplayWidth;
	modeDesc.Height = mDisplayHeight;
	modeDesc.RefreshRate.Numerator = 60;
	modeDesc.RefreshRate.Denominator = 1;
	modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc = modeDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;  // one back buffer for double buffering
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = true;
	swapChainDesc.Flags = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if (mMSAAEnabled)
	{
		swapChainDesc.SampleDesc.Count = sampleDesc.Count;
		swapChainDesc.SampleDesc.Quality = sampleDesc.Quality - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	/* create the swap chain */
	IDXGIDevice *dxgiDevice;
	mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter *dxgiAdapter;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory *dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	
	hr = dxgiFactory->CreateSwapChain(mDevice, &swapChainDesc, &mSwapChain);
	if (FAILED(hr))
		ErrorBox("could not create swap chain");

	dxgiDevice->Release();    // release resources used to create swap chain
	dxgiAdapter->Release();
	dxgiFactory->Release();

	/* create the render target view */
	ID3D11Texture2D *backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);       // Texture2D resource is the swap chain back buffer
	
	hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);  // render target view from Texture2D resource
	if (FAILED(hr))
		ErrorBox("couldn't create render target view");
	backBuffer->Release();     // release backbuffer resource

	/* describe and create depth/stencil buffer 2D texture resource (same size as back buffer) */
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = mDisplayWidth;
	textureDesc.Height = mDisplayHeight;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	if (mMSAAEnabled)
	{
		textureDesc.SampleDesc.Count = sampleDesc.Count;
		textureDesc.SampleDesc.Quality = sampleDesc.Quality - 1;
	}
	else
	{
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
	}
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *depthStencilBuffer;
	mDevice->CreateTexture2D(&textureDesc, nullptr, &depthStencilBuffer);

	/* create depth/stencil view */
	hr = mDevice->CreateDepthStencilView(depthStencilBuffer, nullptr, &mDepthStencilView);
	if (FAILED(hr))
		ErrorBox("couldn't create depth/stencil view");

	depthStencilBuffer->Release();   // release depth and stencil buffer resource

	/* bind render target view and depth stencil view to the Output Merger stage of the rendering pipeline */
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	/* set the viewport */
	D3D11_VIEWPORT viewport = {};
	mViewport.x = viewport.TopLeftX = 0.0f;
	mViewport.y = viewport.TopLeftY = 0.0f;
	mViewport.width = viewport.Width = (float)mDisplayWidth;
	mViewport.height = viewport.Height = (float)mDisplayHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mDeviceContext->RSSetViewports(1, &viewport);

	/*** describe and create all render states at initialization ***/

	/*** rasterizer states ***/

	ID3D11RasterizerState *rasterizerState;

	/* solid raterizer state */
	D3D11_RASTERIZER_DESC fillRasterizerDesc = {};
	fillRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	fillRasterizerDesc.CullMode = D3D11_CULL_BACK;
	fillRasterizerDesc.FrontCounterClockwise = false;
	fillRasterizerDesc.DepthClipEnable = true;

	hr = mDevice->CreateRasterizerState(&fillRasterizerDesc, &rasterizerState);
	if (FAILED(hr))
		ErrorBox("rasterizer state creation failed");

	mRasterizerStateGroup.push_back(rasterizerState);

	/* wireframe rasterizer state */
	D3D11_RASTERIZER_DESC wireframeRasterizerDesc = {};
	wireframeRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRasterizerDesc.CullMode = D3D11_CULL_BACK;
	fillRasterizerDesc.FrontCounterClockwise = false;
	wireframeRasterizerDesc.DepthClipEnable = true;

	hr = mDevice->CreateRasterizerState(&wireframeRasterizerDesc, &rasterizerState);
	if (FAILED(hr))
		ErrorBox("rasterizer state creation failed");

	mRasterizerStateGroup.push_back(rasterizerState);

	/* set default rasterizer state */
	mDeviceContext->RSSetState(mRasterizerStateGroup[0]);

	/*** blend states ***/

	ID3D11BlendState *blendState;

	/* blending disabled (default blend state) */
	D3D11_BLEND_DESC blendDisabledDesc = {};
	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDisabledDesc;
	renderTargetBlendDisabledDesc.BlendEnable = false;
	renderTargetBlendDisabledDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDisabledDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendDisabledDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetBlendDisabledDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendDisabledDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTargetBlendDisabledDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetBlendDisabledDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDisabledDesc.RenderTarget[0] = renderTargetBlendDisabledDesc;

	hr = mDevice->CreateBlendState(&blendDisabledDesc, &blendState);
	if (FAILED(hr))
		ErrorBox("blend state creation failed");

	mBlendStateGroup.push_back(blendState);

	/* blending enabled */
	D3D11_BLEND_DESC blendEnabledDesc = {};
	blendEnabledDesc.AlphaToCoverageEnable = false;
	blendEnabledDesc.IndependentBlendEnable = false;
	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendEnabledDesc;
	renderTargetBlendEnabledDesc.BlendEnable = true;
	renderTargetBlendEnabledDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendEnabledDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendEnabledDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetBlendEnabledDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendEnabledDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendEnabledDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetBlendEnabledDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendEnabledDesc.RenderTarget[0] = renderTargetBlendEnabledDesc;

	hr = mDevice->CreateBlendState(&blendEnabledDesc, &blendState);
	if (FAILED(hr))
		ErrorBox("blend state creation failed");

	mBlendStateGroup.push_back(blendState);

	/*** depth and stencil states ***/

	ID3D11DepthStencilState *depthStencilState;

	/* depth test enabled */
	D3D11_DEPTH_STENCIL_DESC depthStencilDepthDesc;
	depthStencilDepthDesc.DepthEnable = TRUE;
	depthStencilDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDepthDesc.StencilEnable = FALSE;
	depthStencilDepthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDepthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	hr = mDevice->CreateDepthStencilState(&depthStencilDepthDesc, &depthStencilState);
	if (FAILED(hr))
		ErrorBox("depth stencil state creation failed");

	mDepthStencilStateGroup.push_back(depthStencilState);

	/* depth test disabled */
	D3D11_DEPTH_STENCIL_DESC depthStencilNoDepthDesc;
	depthStencilNoDepthDesc.DepthEnable = FALSE;
	depthStencilNoDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilNoDepthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilNoDepthDesc.StencilEnable = FALSE;
	depthStencilNoDepthDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilNoDepthDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	depthStencilNoDepthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilNoDepthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilNoDepthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilNoDepthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilNoDepthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilNoDepthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilNoDepthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilNoDepthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	hr = mDevice->CreateDepthStencilState(&depthStencilNoDepthDesc, &depthStencilState);
	if (FAILED(hr))
		ErrorBox("depth stencil state creation failed");

	mDepthStencilStateGroup.push_back(depthStencilState);

	/* set default depth and stencil state */
	mDeviceContext->OMSetDepthStencilState(mDepthStencilStateGroup[0], 0);
}

void GraphicsSystem::SetRasterizerState(RasterizerState state)
{
	mDeviceContext->RSSetState(mRasterizerStateGroup[static_cast<int>(state)]);
}

void GraphicsSystem::SetBlendState(BlendState state)
{
	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDeviceContext->OMSetBlendState(mBlendStateGroup[static_cast<int>(state)], blendFactor, 0xFFFFFFFF);
}

void GraphicsSystem::SetDepthStencilState(DepthStencilState state)
{
	mDeviceContext->OMSetDepthStencilState(mDepthStencilStateGroup[static_cast<int>(state)], 0);
}

void GraphicsSystem::ClearScreen(const float color[])
{
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsSystem::Present()
{
	mSwapChain->Present(0, 0);
}

