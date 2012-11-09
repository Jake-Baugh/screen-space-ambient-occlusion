#include "renderTarget.h"

RenderTarget::RenderTarget(ID3D11Device* device, ID3D11DeviceContext* dc)
	: _deviceContext(dc), _device(device)
{
	_srvSlot = -1;
}

RenderTarget::~RenderTarget() {
	if(_targetTexture) { _targetTexture->Release(); _targetTexture = 0; }
	if(_device) { _device->Release(); _device = 0; }
	if(_deviceContext) { _deviceContext->Release(); _deviceContext = 0; }
	if(_targetView) { _targetView->Release(); _targetView = 0; }
	if(_srv) { _srv->Release(); _srv = 0; }
}

void RenderTarget::Resize(int width, int height) {

}

void RenderTarget::CreateRenderTarget(int width, int height, DXGI_FORMAT format /*=DXGI_FORMAT_R32G32B32A32_FLOAT*/) {
	D3D11_RENDER_TARGET_VIEW_DESC rtd;
	D3D11_TEXTURE2D_DESC td;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	HRESULT hret;

	ZeroMemory(&td, sizeof(td));
	td.Width			= width;
	td.Height			= height;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= format;
	td.SampleDesc.Count	= 1;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags	= 0;
	td.MiscFlags		= 0;

	hret = _device->CreateTexture2D(&td, NULL, &_targetTexture);
	if(FAILED(hret)) {
		return;
	}

	rtd.Format				= td.Format;
	rtd.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	rtd.Texture2D.MipSlice	= 0;

	hret = _device->CreateRenderTargetView(_targetTexture, &rtd, &_targetView);
	if(FAILED(hret)) {
		return;
	}

	srvd.Format						= td.Format;
	srvd.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip	= 0;
	srvd.Texture2D.MipLevels		= 1;
	hret = _device->CreateShaderResourceView(_targetTexture, &srvd, &_srv);
	if(FAILED(hret)) {
		return;

	}
}

void RenderTarget::SetRenderTarget(ID3D11DepthStencilView* depthStencilView) {
	_deviceContext->OMSetRenderTargets(1, &_targetView, depthStencilView);
}

void RenderTarget::ClearRenderTarget(ID3D11DepthStencilView* depthStencilView) {
	float color[4] = {0.f, 0.f, 0.f, 1.f};
	_deviceContext->ClearRenderTargetView(_targetView, color);
	_deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void RenderTarget::BindSRV(int slot) {
	_srvSlot = slot;
	_deviceContext->PSSetShaderResources(slot, 1, &_srv);
}

void RenderTarget::UnbindSRV() {
	ID3D11ShaderResourceView* nullsrv = NULL;
	_deviceContext->PSSetShaderResources(_srvSlot, 1, &nullsrv);
}