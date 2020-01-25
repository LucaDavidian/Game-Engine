#pragma once

#include <d3d11.h>
#include <vector>

class GraphicsSystem
{
public:
	struct Viewport
	{
		unsigned x;
		unsigned y;
		unsigned width;
		unsigned height;
	};
public:
	enum class RasterizerState { SOLID, WIREFRAME, };        // render states are created at initialization and stored into dynamic arrays in same order as enumerators
	enum class BlendState { DISABLED, ADDITIVE, };
	enum class DepthStencilState { ENABLED, DISABLED, };
	static GraphicsSystem &GetInstance();
	~GraphicsSystem() = default;
	void Initialize(HWND window);
	unsigned GetDisplayWidth() const { return mDisplayWidth; }
	unsigned GetDisplayHeight() const { return mDisplayHeight; }
	Viewport GetViewport() const { return mViewport; }
	void SetRasterizerState(RasterizerState state);
	void SetBlendState(BlendState state);
	void SetDepthStencilState(DepthStencilState state);
	void ClearScreen(const float color[]);
	void Present();
	ID3D11Device *GetDevice() { return mDevice; }
	ID3D11DeviceContext *GetDeviceContext() { return mDeviceContext; }
	bool IsMSAAEnabled() const { return mMSAAEnabled; }
	int GetSampleCount() const { return mSampleCount; }
	int GetSampleQuality() const { return mSampleQuality; }
private:
	GraphicsSystem() = default;
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;
	IDXGISwapChain *mSwapChain;
	int mSampleCount;
	int mSampleQuality;
	ID3D11RenderTargetView *mRenderTargetView;
	ID3D11DepthStencilView *mDepthStencilView;
	unsigned int mDisplayWidth;
	unsigned int mDisplayHeight;
	Viewport mViewport;
	bool mMSAAEnabled = true;
	std::vector<ID3D11RasterizerState*> mRasterizerStateGroup;
	std::vector<ID3D11BlendState*> mBlendStateGroup;
	std::vector<ID3D11DepthStencilState*> mDepthStencilStateGroup;
};




