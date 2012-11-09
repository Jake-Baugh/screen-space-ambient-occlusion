#ifndef _render_target_h_
#define _render_target_h_

#include <d3d11.h>

class RenderTarget {
public:
	RenderTarget(ID3D11Device* device, ID3D11DeviceContext* dc);
	~RenderTarget();

	void Resize(int width, int height);
	void CreateRenderTarget(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	void SetRenderTarget(ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DepthStencilView* depthStencilView);
	void BindSRV(int slot);
	void UnbindSRV();
private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;
	ID3D11RenderTargetView* _targetView;
	ID3D11Texture2D* _targetTexture;
	ID3D11ShaderResourceView* _srv;
	int _srvSlot;
};

#endif