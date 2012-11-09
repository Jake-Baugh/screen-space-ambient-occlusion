#include "Direct3D_11.h"

D3D::D3D()
	: m_swapChain(0), m_device(0), m_deviceContext(0), m_renderTargetView(0), m_depthStencilBuffer(0), m_depthStencilView(0),
	m_rasterState(0), m_depthStencilState(0)
{
}

D3D::~D3D()
{
	ReleaseCOM(m_device);
	ReleaseCOM(m_deviceContext);
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_swapChain);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);

	m_enumAdapters.Release();
}

void D3D::InitD3D(int screenWidth, int screenHeight, HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd;
	D3D_FEATURE_LEVEL featureLevel;
	UINT createDeviceFlags = 0;

	m_enumAdapters.EnumerateAdapters();

	//Initialize swap chain
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width					= screenWidth;
	scd.BufferDesc.Height					= screenHeight;
	scd.BufferDesc.RefreshRate.Numerator	= 0;
	scd.BufferDesc.RefreshRate.Denominator	= 1;
	scd.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	//Turn off multi-sampling
	scd.SampleDesc.Count					= 1;
	scd.SampleDesc.Quality					= 0;
	scd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow						= hWnd;
	scd.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags								= 0;
	scd.Windowed							= true;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//Create swapchain, device, and device context
	HR(D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 1, 
		D3D11_SDK_VERSION, &scd, &m_swapChain, &m_device, NULL, &m_deviceContext));

	//Set render target, depth/stencil buffer and view
	D3DResize(screenWidth, screenHeight);
	
	//Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsdesc;
	ZeroMemory(&dsdesc, sizeof(dsdesc));

	dsdesc.DepthEnable = true;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsdesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsdesc.StencilEnable = true;
	dsdesc.StencilReadMask = 0xFF;
	dsdesc.StencilWriteMask = 0xFF;
	dsdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(m_device->CreateDepthStencilState(&dsdesc, &m_depthStencilState));

	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	D3D11_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = false;
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.FrontCounterClockwise = false;
	rd.MultisampleEnable = false;
	rd.ScissorEnable = false;
	rd.SlopeScaledDepthBias = 0.0f;

	HR(m_device->CreateRasterizerState(&rd, &m_rasterState));

	m_deviceContext->RSSetState(m_rasterState);

	m_device->CreateDeferredContext(0, &m_deferredContext);
	
}

/**
	Dependent on having your depth buffer set
**/
void D3D::CreateDepthStencilView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	ReleaseCOM(m_depthStencilView);
	
	//Check if we have a depth stencil buffer
	if(!m_depthStencilBuffer) return;

	//Initialize depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension		= D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	HR(m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView));

}

/**
**/
void D3D::CreateDepthStencilBuffer(int screenWidth, int screenHeight)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;

	ReleaseCOM(m_depthStencilBuffer);

	//Initialize depth stencil buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width				= screenWidth;
	depthBufferDesc.Height				= screenHeight;
	depthBufferDesc.MipLevels			= 1;
	depthBufferDesc.ArraySize			= 1;
	depthBufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count	= 1;
	depthBufferDesc.SampleDesc.Quality	= 0;
	depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags		= 0;
	depthBufferDesc.MiscFlags			= 0;

	HR(m_device->CreateTexture2D(&depthBufferDesc, 0, &m_depthStencilBuffer));
}

/** 
**/
void D3D::CreateRenderTargetView(int screenWidth, int screenHeight)
{
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_depthStencilView);
	ReleaseCOM(m_depthStencilBuffer);
	

	//Get pointer to backbuffer and attach to swapchain
	m_swapChain->ResizeBuffers(1, screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D11Texture2D* backBufferPtr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	m_device->CreateRenderTargetView(backBufferPtr, 0, &m_renderTargetView);
	
	ReleaseCOM(backBufferPtr);
}

/** 
**/
void D3D::SetViewport(int width, int height)
{
	D3D11_VIEWPORT viewport;

	viewport.Height = (float)height;
	viewport.Width  = (float)width;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

/** 
**/
void D3D::D3DResize(int screenWidth, int screenHeight)
{
	
	//ReleaseCOM(m_renderTargetView);
	//ReleaseCOM(m_depthStencilBuffer);
	//ReleaseCOM(m_depthStencilView);	
	

	CreateRenderTargetView(screenWidth, screenHeight);
	CreateDepthStencilBuffer(screenWidth, screenHeight);
	CreateDepthStencilView();
	m_renderTargetView->AddRef();
	int ref = m_renderTargetView->Release();
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	SetViewport(screenWidth, screenHeight);
}

ID3D11Device* D3D::GetDevice()					{ return m_device; }
ID3D11DeviceContext* D3D::GetDeviceContext()	{ return m_deviceContext; }

/**
	Clears the render target and depth stencil for rendering
**/
void D3D::BeginScene(float r /* = 0.0f */, float g /* = 0.0f */, 
	float b /* = 0.0f */, float a /* = 1.0f */)
{
	float color[4]; //clear color
	color[0] = r; color[1] = g; color[2] = b; color[3] = a;

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

/** 
	Presents the back buffer
	@To-do: handle Vsync
**/ 
void D3D::EndScene()
{
	//Check for Vsync
	//m_swapChain->Present(1, 0); //Vsync enabled

	m_swapChain->Present(0,0); //Vsync disabled
}

ID3D11DepthStencilView* D3D::GetDepthStencilView() { return m_depthStencilView; }
void D3D::ResetBackbufferRenderTarget() {
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

}



//-------------------------------------------------------------------------------
// Enumerate Adapters
//-------------------------------------------------------------------------------

EnumAdapters::EnumAdapters()
{}

EnumAdapters::~EnumAdapters()
{}

/** 
	Enumerate through each adapter and its outputs
	adding references to the adapters and outputs
	@see EnumerateOutputs
**/
void EnumAdapters::EnumerateAdapters()
{
	UINT i = 0;
	IDXGIFactory* factory;
	IDXGIAdapter* padapter;

	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));
	while(factory->EnumAdapters(i, &padapter) != DXGI_ERROR_NOT_FOUND)
	{
		Adapter adapter(padapter);
		m_adapters.push_back(adapter);
		i++;
	}

	std::vector<Adapter>::iterator it;
	for(it = m_adapters.begin(); it != m_adapters.end(); it++)
	{
		it->EnumerateOutputs();
	}

	factory->Release();
	factory = 0;
}

/** 
	Release the COM that each Adapter Class holds
	@see ReleaseOutputs
	@see ReleaseAdapter
**/
void EnumAdapters::Release()
{
	std::vector<Adapter>::iterator it;
	for(it = m_adapters.begin(); it != m_adapters.end(); it++)
	{
		it->ReleaseOutputs();
		it->ReleaseAdapter();
	}
	m_adapters.clear();
}

/**
	Add a reference to each output for the adapter
	then get the all possible DisplayModes for the output
	@see GetDisplayMode
**/
void Adapter::EnumerateOutputs()
{
	UINT i = 0;
	IDXGIOutput* poutput;
	
	HR(m_adapter->GetDesc(&m_adapterDesc));

	while(m_adapter->EnumOutputs(i, &poutput) != DXGI_ERROR_NOT_FOUND)
	{
		Output output(poutput);
		m_outputs.push_back(output);
		i++;
	}

	std::vector<Output>::iterator it;
	for(it = m_outputs.begin(); it != m_outputs.end(); it++)
	{
		it->GetDisplayModes();
	}
}

/** 
	Release the COM that each output holds
	@see ReleaseOutput
**/
void Adapter::ReleaseOutputs()
{
	std::vector<Output>::iterator it;
	for(it = m_outputs.begin(); it != m_outputs.end(); it++)
	{
		it->ReleaseOuput();
	}
	m_outputs.clear();
}

/** 
	Find the number of displayModes for the given output.
	For each display mode, creates a DisplayMode object 
	and pushes it onto a vector.
**/
void Output::GetDisplayModes()
{
	HR(m_output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&m_numModes, NULL));
	
	DXGI_MODE_DESC* displayModes = new DXGI_MODE_DESC[m_numModes];
	if(displayModes == NULL) { OutputDebugString(L"Boom"); }

	HR(m_output->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&m_numModes, displayModes));

	for(UINT i = 0; i < m_numModes; i++)
	{
		DisplayMode mode(displayModes[i]);
		m_displayModes.push_back(mode);
	}

	delete [] displayModes;
	displayModes = 0;

}
