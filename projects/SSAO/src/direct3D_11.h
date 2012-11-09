#ifndef _d3d_h_
#define _d3d_h_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <stdlib.h>
#include <vector>

#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
#define HR(x) (x)
#define DEBUG

//-------------------------------------------------------------------------------
// Enumerate Adapters
//-------------------------------------------------------------------------------

//---DisplayMode---
class DisplayMode
{
public:
	DisplayMode(DXGI_MODE_DESC displayMode) { m_displayMode = displayMode; }
	~DisplayMode() {}

	UINT						GetWidth()			  { return m_displayMode.Width; }
	UINT						GetHeight()		      { return m_displayMode.Height; }
	UINT						GetRefreshRateNum()   { return m_displayMode.RefreshRate.Numerator; }
	UINT						GetRefreshRateDenom() { return m_displayMode.RefreshRate.Denominator; }
	DXGI_FORMAT					GetFormat()			  { return m_displayMode.Format; }
	DXGI_MODE_SCALING			GetScaling()		  { return m_displayMode.Scaling; }
	DXGI_MODE_SCANLINE_ORDER	GetScanlineOrdering() { return m_displayMode.ScanlineOrdering; }
private:
	DXGI_MODE_DESC m_displayMode;
};

//---Output---
class Output
{
public:
	Output(IDXGIOutput* output) { m_output = output; }
	~Output() {}

	void ReleaseOuput()	{ ReleaseCOM(m_output); m_displayModes.clear(); }
	void GetDisplayModes();
private:
	UINT			m_numModes;
	IDXGIOutput*	m_output;
	std::vector<DisplayMode> m_displayModes;
};

//---Adapter---
class Adapter
{
public:
	Adapter(IDXGIAdapter* adapter) { m_adapter = adapter; }
	~Adapter() {}

	void ReleaseAdapter() { ReleaseCOM(m_adapter); }
	void ReleaseOutputs();
	void EnumerateOutputs();
private:
	IDXGIAdapter*		m_adapter;
	DXGI_ADAPTER_DESC	m_adapterDesc;
	std::vector<Output> m_outputs;
};

//---EnumAdapters---
class EnumAdapters
{
public:
	EnumAdapters();
	~EnumAdapters();

	void EnumerateAdapters();
	void Release();
private:
	std::vector<Adapter>  m_adapters;
};

//-------------------------------------------------------------------------------
// Direct3D
//------------------------------------------------------------------------------
class D3D
{
public:
	D3D();
	~D3D();

	void InitD3D(int screenWidth, int screenHeight, HWND hWnd);

	void D3DResize(int screenWidth, int screenHeight);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11DepthStencilView* GetDepthStencilView();
	void ResetBackbufferRenderTarget();
	void BeginScene(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
	void EndScene();
private:
	void CreateDepthStencilBuffer(int screenWidth, int screenHeight);
	void CreateDepthStencilView();
	void CreateRenderTargetView(int screenWidth, int screenHeight);
	void SetViewport(int width, int height);
private:
	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;
	ID3D11DeviceContext*	m_deferredContext;
	ID3D11RenderTargetView* m_renderTargetView;
	IDXGISwapChain*			m_swapChain;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D*		m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11RasterizerState*	m_rasterState;

	EnumAdapters			m_enumAdapters;


};



#endif