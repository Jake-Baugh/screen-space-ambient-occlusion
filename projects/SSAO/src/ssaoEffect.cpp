#include "ssaoEffect.h"


SSAO::SSAO(ID3D11Device* dev, ID3D11DeviceContext* dc, D3D* d3d, App* app) 
	: _device(dev), _deviceContext(dc), _direct3D(d3d), _app(app)
{

}

void SSAO::BuildUI() {
	TwInit(TW_DIRECT3D11, _device);
	TwWindowSize(_screenWidth, _screenHeight);
	_bar = TwNewBar("AmbientOcclusion");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
	int barSize[2] = {300, 375};
	TwSetParam(_bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	TwAddVarCB(_bar, "Rad", TW_TYPE_FLOAT, SSAO::SetRad, SSAO::GetRad, this, "group=ShaderParams min=0 max=20 step=0.001 keyincr=+ keydecr=-");
	TwAddVarCB(_bar, "TotalStr", TW_TYPE_FLOAT, SSAO::SetTotStr, SSAO::GetTotStr, this, "group=ShaderParams min=0 max=50 step=0.1 keyincr=+ keydecr=-");
	TwAddVarCB(_bar, "Strength", TW_TYPE_FLOAT, SSAO::SetStrength, SSAO::GetStrength, this, "group=ShaderParams min=0.1 max=100 step=0.1 keyincr=+ keydecr=-");
	TwAddVarCB(_bar, "Offset", TW_TYPE_FLOAT, SSAO::SetOffset, SSAO::GetOffset, this, "group=ShaderParams min=0 max=100 step=0.5 keyincr=+ keydecr=-");
	TwAddVarCB(_bar, "Falloff", TW_TYPE_FLOAT, SSAO::SetFalloff, SSAO::GetFalloff, this, "group=ShaderParams min=0 max=0.01 step=0.00001 keyincr=+ keydecr=-");
	TwAddVarCB(_bar, "BlurSize", TW_TYPE_FLOAT, SSAO::SetBlur, SSAO::GetBlur, this, "group=ShaderParams min=1 max=16 step=1 keyincr=+ keydecr=-");
	TwAddButton(_bar, "Reset", SSAO::ResetButton, this, "");
	TwAddButton(_bar, "ShowDebug", SSAO::ShowDebug, this, "");

	TwEnumVal* a = new TwEnumVal[3];
	a[0].Value = 0; a[0].Label = "buddha";
	a[1].Value = 1; a[1].Label = "hairball";
	a[2].Value = 2; a[2].Label = "sib";
	_index = 0;

	TwType actMesh = TwDefineEnum("mesh", a, 3);

	TwAddVarCB(_bar, "ActiveVol", actMesh, SSAO::SetMesh, SSAO::GetMesh, this, 
		" group='Mesh' keyIncr=Backspace keyDecr=SHIFT+Backspace help='Stop or change the rotation mode.' ");

}

void SSAO::Initialize(int screenWidth, int screenHeight, float farC, float nearC, float fov) {
	_app->SwitchActive("sib");
	_screenHeight = screenHeight;
	_screenWidth = screenWidth;
	_showDebug = false;
	_near = nearC;
	_far  = farC;
	_fov = fov;
	_kernalSize = 16;
	_noiseSize =  64;
	_rad = 0.065;
	_totStrength = 1.6;
	_strength = 0.7;
	_offset = 18;
	_falloff = 0.0001;
	_defrad = 0.02;
	_deftotStrength = 3.38;
	_defstrength = 0.7;
	_defoffset = 18;
	_deffalloff = 0.0002;
	_blurSize = 4;
	_defblurSize = 4;

	srand(time(NULL));

	//create cosntant buffer
	D3D11_BUFFER_DESC ssaoBuff;
	ssaoBuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ssaoBuff.ByteWidth = sizeof(SSAOBuffer);
	ssaoBuff.Usage = D3D11_USAGE_DYNAMIC;
	ssaoBuff.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ssaoBuff.MiscFlags = 0;
	ssaoBuff.StructureByteStride = 0;

	_device->CreateBuffer(&ssaoBuff, 0, &_ssaoBuffer);
	//create cosntant buffer
	D3D11_BUFFER_DESC ssaoBuff2;
	ssaoBuff2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ssaoBuff2.ByteWidth = sizeof(SSAOBuffer2);
	ssaoBuff2.Usage = D3D11_USAGE_DYNAMIC;
	ssaoBuff2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ssaoBuff2.MiscFlags = 0;
	ssaoBuff2.StructureByteStride = 0;

	HRESULT hret = _device->CreateBuffer(&ssaoBuff2, 0, &_ssaoBuffer2);

	_deviceContext->PSSetConstantBuffers(11, 1, &_ssaoBuffer2);
	_deviceContext->PSSetConstantBuffers(10, 1, &_ssaoBuffer);
	_deviceContext->VSSetConstantBuffers(10, 1, &_ssaoBuffer);
	

	//initialize render targets
	_normalAndDepthBuffer = new RenderTarget(_device, _deviceContext);
	_normalAndDepthBuffer->CreateRenderTarget(_screenWidth, _screenHeight, DXGI_FORMAT_R32G32B32A32_FLOAT);
	_ssaoRT = new RenderTarget(_device, _deviceContext);
	_ssaoRT->CreateRenderTarget(_screenWidth, _screenHeight, DXGI_FORMAT_R32_FLOAT);
	_blurRT = new RenderTarget(_device, _deviceContext);
	_blurRT->CreateRenderTarget(_screenWidth, _screenHeight, DXGI_FORMAT_R32_FLOAT);

	BuildFullScreenQuad();
	BuildSampleKernal();
	BuildNoise();
	BuildFrustumCorner();
	BuildTextures();
	BuildUI();

	_ndPass= new ShaderProgram(_device, _deviceContext);
	_ndPass->CompileShaders(L"media/hlsl/normalAndDepthPass.vsh", L"media/hlsl/normalAndDepthPass.psh");
	_ssaoPass= new ShaderProgram(_device, _deviceContext);
	_ssaoPass->CompileShaders(L"media/hlsl/ssaoPass.vsh", L"media/hlsl/ssaoPass.psh");
	_blurPass= new ShaderProgram(_device, _deviceContext);
	_blurPass->CompileShaders(L"media/hlsl/blurrPass.vsh", L"media/hlsl/blurrPass.psh");
	_debug= new ShaderProgram(_device, _deviceContext);
	_debug->CompileShaders(L"media/hlsl/debug.vsh", L"media/hlsl/debug.psh");


	UpdateBuffer();
	UpdateBuffer2();
}

void SSAO::SSAOPass() {
	_ssaoRT->ClearRenderTarget(_direct3D->GetDepthStencilView());
	_ssaoRT->SetRenderTarget(_direct3D->GetDepthStencilView());
	_normalAndDepthBuffer->BindSRV(9);
	_ssaoPass->SetActive();
	DrawFSQ();
	_normalAndDepthBuffer->UnbindSRV();

	_blurRT->ClearRenderTarget(_direct3D->GetDepthStencilView());
	_blurRT->SetRenderTarget(_direct3D->GetDepthStencilView());
	_ssaoRT->BindSRV(9);
	_blurPass->SetActive();
	DrawFSQ();
	_ssaoRT->UnbindSRV();
}

void SSAO::DrawSSAO() {
	_ssaoRT->BindSRV(9);
	_blurPass->SetActive();
	DrawFSQ();
	_ssaoRT->UnbindSRV();
}
void SSAO::NormalAndDepthPass() {
	_normalAndDepthBuffer->ClearRenderTarget(_direct3D->GetDepthStencilView());
	_normalAndDepthBuffer->SetRenderTarget(_direct3D->GetDepthStencilView());
	_ndPass->SetActive();
}
void SSAO::BindNormalDepthPass(int slot) { _normalAndDepthBuffer->BindSRV(slot); }
void SSAO::UnbindNormalDepthPass()		 { _normalAndDepthBuffer->UnbindSRV(); }
void SSAO::BindSSAOBlurPass(int slot)		 { _blurRT->BindSRV(slot); }
void SSAO::UnindSSAOBlurPass()				 { _blurRT->UnbindSRV(); }
void SSAO::BindSSAOPass(int slot)		 { _ssaoRT->BindSRV(slot); }
void SSAO::UnbindSSAOPass()				 { _ssaoRT->UnbindSRV(); }


void SSAO::UpdateBuffer2() {
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	HRESULT hret;

	hret = _deviceContext->Map(_ssaoBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if(FAILED(hret))
	{}

	SSAOBuffer2* pf = (SSAOBuffer2*)mappedResources.pData;
	pf->falloff = _falloff;
	pf->offset = _offset;
	pf->rad = _rad;
	pf->strength = _strength;
	pf->totStrength = _totStrength;
	pf->blurSize = _blurSize;

	_deviceContext->Unmap(_ssaoBuffer2, 0);
}

void SSAO::UpdateBuffer() {
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	HRESULT hret;

	hret = _deviceContext->Map(_ssaoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if(FAILED(hret))
	{}

	SSAOBuffer* pf = (SSAOBuffer*)mappedResources.pData;
	pf->farClip = _far;
	pf->nearClip = _near;
	pf->kernal_size = float(_kernalSize);
	pf->width = float(_screenWidth);
	pf->height = float(_screenHeight);
	pf->noiseScale = _noiseScale;
	pf->frustumCorner = _frustumCorner;
	memcpy(pf->sampleKernal, _kernal, sizeof(_kernal));

	_deviceContext->Unmap(_ssaoBuffer, 0);
}

void SSAO::BuildSampleKernal() {
	for(int i = 0; i < _kernalSize; ++i) {
		XMVECTOR k = XMLoadFloat3(&vec3(Random(-1.f, 1.f), Random(-1.f, 1.f), Random(0.f, 1.f)));
		k = XMVector3Normalize(k);
		k *= Random(0.f, 1.f);
		float scale = float(i) / float(_kernalSize);
		scale = Lerp(0.1f, 1.0f, scale * scale);
		k *= scale;
		XMStoreFloat3(&_kernal[i], k);
	}
}

void SSAO::BuildNoise() {
	_noiseScale = vec2((float)_screenWidth/_noiseSize, (float)_screenHeight/_noiseSize);
	_noise = new vec3[_noiseSize*_noiseSize];
	for(int i = 0; i < _noiseSize*_noiseSize; ++i) {
		XMVECTOR k = XMLoadFloat3(&vec3(Random(-1.f, 1.f), Random(-1.f, 1.f), 0.f));
		k = XMVector3Normalize(k);
		XMStoreFloat3(&_noise[i], k);
	}
}
float SSAO::Random(float low, float high) {
	float random = (float)rand() / (float) RAND_MAX ;
	return random * (high - low) + low;
}

float SSAO::Lerp(float factor, float a, float b) {
	return factor*a + (1.0f-factor)*b;
}

void SSAO::BuildFullScreenQuad() {
	Vertex v[4];
	//texcoord stores frustumcorner information
	v[0] = Vertex(vec4(-1.f,  1.f, 0.f, 1.f), vec3(0.f, 1.f, 0.f)); //tl
	v[1] = Vertex(vec4( 1.f,  1.f, 0.f, 1.f), vec3(1.f, 1.f, 1.f));  //tr
	v[2] = Vertex(vec4( 1.f, -1.f, 0.f, 1.f), vec3(1.f, 0.f, 2.f));  //br
	v[3] = Vertex(vec4(-1.f, -1.f, 0.f, 1.f), vec3(0.f, 0.f, 3.f));  //bl

	unsigned short i[6];
	i[0] = 0; i[1] = 1; i[2] = 3;
	i[3] = 3; i[4] = 1; i[5] = 2;

	HRESULT hret;
	D3D11_BUFFER_DESC vbd;
	memset(&vbd, 0, sizeof(D3D11_BUFFER_DESC));
	vbd.ByteWidth = sizeof(Vertex) * 4;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vsrd;
	memset(&vsrd, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	vsrd.pSysMem = v;

	D3D11_BUFFER_DESC ibd;
	memset(&ibd, 0, sizeof(D3D11_BUFFER_DESC));
	ibd.ByteWidth = sizeof(unsigned short) * 6;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA isrd;
	memset(&isrd, 0, sizeof(D3D11_SUBRESOURCE_DATA));
	isrd.pSysMem = i;

	hret = _device->CreateBuffer(&vbd, &vsrd, &_fsqVb);
	if(FAILED(hret)) {}

	hret = _device->CreateBuffer(&ibd, &isrd, &_fsqIb);
	if(FAILED(hret)) {}

	_indexCount = 6;
}

void SSAO::DrawFSQ() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	_deviceContext->IASetVertexBuffers(0, 1, &_fsqVb, &stride, &offset);
	_deviceContext->IASetIndexBuffer(_fsqIb, DXGI_FORMAT_R16_UINT, 0);

	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->DrawIndexed(_indexCount, 0, 0);
}

void SSAO::BuildFrustumCorner() {
	_frustumCorner = vec3(0,0,0);
	_frustumCorner.y = tanf(_fov / 2.0f) * _far;
	_frustumCorner.x = _frustumCorner.y * float(_screenWidth) / float(_screenHeight);
	_frustumCorner.z = _far;
}

void SSAO::BuildTextures() {
	//Create the noise texture
	D3D11_TEXTURE2D_DESC td;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	HRESULT hret;

	ZeroMemory(&td, sizeof(td));
	td.Width			= _noiseSize;
	td.Height			= _noiseSize;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_R32G32B32_FLOAT;
	td.SampleDesc.Count	= 1;
	td.Usage			= D3D11_USAGE_IMMUTABLE;
	td.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags	= 0;
	td.MiscFlags		= 0;

	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = _noise;
	srd.SysMemPitch = _noiseSize * sizeof(vec3);

	hret = _device->CreateTexture2D(&td, &srd, &_noiseTex);
	if(FAILED(hret)) {
		return;
	}

	srvd.Format						= td.Format;
	srvd.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip	= 0;
	srvd.Texture2D.MipLevels		= 1;
	hret = _device->CreateShaderResourceView(_noiseTex, &srvd, &_noiseSRV);
	if(FAILED(hret)) {
		return;

	}

	//create kernal texture
	ZeroMemory(&td, sizeof(td));
	ZeroMemory(&srd, sizeof(srd));
	ZeroMemory(&srvd, sizeof(srvd));
	td.Width			= _kernalSize / 2;
	td.Height			= _kernalSize / 2;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_R32G32B32_FLOAT;
	td.SampleDesc.Count	= 1;
	td.Usage			= D3D11_USAGE_IMMUTABLE;
	td.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags	= 0;
	td.MiscFlags		= 0;


	srd.pSysMem = _kernal;
	srd.SysMemPitch = _kernalSize / 2 * sizeof(vec3);

	hret = _device->CreateTexture2D(&td, &srd, &_kernalTex);
	if(FAILED(hret)) {
		return;
	}

	srvd.Format						= td.Format;
	srvd.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MostDetailedMip	= 0;
	srvd.Texture2D.MipLevels		= 1;
	hret = _device->CreateShaderResourceView(_kernalTex, &srvd, &_kernalSRV);
	if(FAILED(hret)) {
		return;

	}


	_deviceContext->PSSetShaderResources(8, 1, &_noiseSRV);
	_deviceContext->PSSetShaderResources(7, 1, &_kernalSRV);
}


void SSAO::Debug() {
	if(_showDebug == false) return;
	_normalAndDepthBuffer->BindSRV(22);
	_ssaoRT->BindSRV(21);
	_blurRT->BindSRV(20);
	_debug->SetActive();
	DrawFSQ();
	_normalAndDepthBuffer->UnbindSRV();
	_ssaoRT->UnbindSRV();
	_blurRT->UnbindSRV();

}

void TW_CALL SSAO::SetRad(const void *value, void * clientData)
{
	((SSAO*)clientData)->_rad = *(float*)value;
	((SSAO*)clientData)->UpdateBuffer2();

}
void TW_CALL SSAO::GetRad(void *value, void * clientData)
{
	*(float*)value = ((SSAO*)clientData)->_rad;
}

void TW_CALL SSAO::SetTotStr(const void *value, void * clientData)
{
	((SSAO*)clientData)->_totStrength = *(float*)value;
	((SSAO*)clientData)->UpdateBuffer2();

}
void TW_CALL SSAO::GetTotStr(void *value, void * clientData)
{
	*(float*)value = ((SSAO*)clientData)->_totStrength;
}


void TW_CALL SSAO::SetStrength(const void *value, void * clientData)
{
	((SSAO*)clientData)->_strength = *(float*)value;
	((SSAO*)clientData)->UpdateBuffer2();

}
void TW_CALL SSAO::GetStrength(void *value, void * clientData)
{
	*(float*)value = ((SSAO*)clientData)->_strength;
}

void TW_CALL SSAO::SetOffset(const void *value, void * clientData)
{
	((SSAO*)clientData)->_offset = *(float*)value;
	((SSAO*)clientData)->UpdateBuffer2();

}
void TW_CALL SSAO::GetOffset(void *value, void * clientData)
{
	*(float*)value = ((SSAO*)clientData)->_offset;
}

void TW_CALL SSAO::SetFalloff(const void *value, void * clientData)
{
	((SSAO*)clientData)->_falloff = *(float*)value;
	((SSAO*)clientData)->UpdateBuffer2();

}
void TW_CALL SSAO::GetFalloff(void *value, void * clientData)
{
	*(float*)value = ((SSAO*)clientData)->_falloff;
}

void TW_CALL SSAO::SetBlur(const void *value, void * clientData)
{
	((SSAO*)clientData)->_blurSize = *(float*)value;
	((SSAO*)clientData)->UpdateBuffer2();

}
void TW_CALL SSAO::GetBlur(void *value, void * clientData)
{
	*(float*)value = ((SSAO*)clientData)->_blurSize;
}


void TW_CALL SSAO::SetMesh(const void *value, void * clientData)
{
	int index = *(int*)value;
	if(index == 0) ((SSAO*)clientData)->_app->SwitchActive("buddha");
	if(index == 1) ((SSAO*)clientData)->_app->SwitchActive("hairball");
	if(index == 2) ((SSAO*)clientData)->_app->SwitchActive("sib");
	((SSAO*)clientData)->_index = index;
}
void TW_CALL SSAO::GetMesh(void *value, void * clientData)
{
	*(int*)value = ((SSAO*)clientData)->_index;
}

void TW_CALL SSAO::ResetButton(void * clientData)
{
	((SSAO*)clientData)->_rad = ((SSAO*)clientData)->_defrad;
	((SSAO*)clientData)->_totStrength = ((SSAO*)clientData)->_deftotStrength;
	((SSAO*)clientData)->_strength = ((SSAO*)clientData)->_defstrength;
	((SSAO*)clientData)->_offset = ((SSAO*)clientData)->_defoffset;
	((SSAO*)clientData)->_falloff = ((SSAO*)clientData)->_deffalloff;
	((SSAO*)clientData)->UpdateBuffer2();
}

void TW_CALL SSAO::ShowDebug(void * clientData)
{
	((SSAO*)clientData)->_showDebug = !((SSAO*)clientData)->_showDebug;
}