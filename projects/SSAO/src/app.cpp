#include "app.h"
#include "AntTweakBar.h"
#include <d3dx11.h>
using namespace std;

App::~App()
{
	ReleaseCOM(_deviceContext);
	ReleaseCOM(_device);
	ReleaseCOM(_pfb);
}
void App::RunApp() {
	InitApp();
	Run();
}

void App::InitApp()
{
	_occOn = false;

	_dirty = true;
	HRESULT hret;
	XMStoreFloat4x4(&_rot, XMMatrixIdentity());
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMMATRIX xmat = XMMatrixRotationX(0);
	XMMATRIX ymat = XMMatrixRotationY(0);
	XMMATRIX zmat = XMMatrixRotationZ(0);
	XMStoreFloat4x4(&_world, xmat * ymat * zmat * XMLoadFloat4x4(&_world));

	_timer = new Timer();
	_timer->Initialize();

	_window = new Window();
	_window->createWindow(L"AmbientOcclusion");

	_direct3D = new D3D();
	_direct3D->InitD3D(_window->getWidth(), _window->getHeight(), _window->getHandle());

	_device = _direct3D->GetDevice();
	_deviceContext = _direct3D->GetDeviceContext();

	_camera = new Camera();
	_camera->InitCamera(_window->getWidth(), _window->getHeight());
	_camera->Translate(XMFLOAT3(0.0, 0.0, 3));

	_mesh = new Mesh(_device, _deviceContext);
	_mesh->LoadConvertedObj("media/converted/hairball.obj.converted");

	RegisterMesh("hairball", _mesh);
	SwitchActive("hairball");

	_mesh = new Mesh(_device, _deviceContext);
	_mesh->LoadConvertedObj("media/converted/buddha.obj.converted");
	RegisterMesh("buddha", _mesh);
	_mesh = new Mesh(_device, _deviceContext);
	_mesh->LoadConvertedObj("media/converted/sib.obj.converted");
	RegisterMesh("sib", _mesh);
	
	SwitchActive("hairball");


	_basic = new ShaderProgram(_device, _deviceContext);
	_basic->CompileShaders(L"media/hlsl/basic.vsh", L"media/hlsl/basic.psh");

	_ssao = new SSAO(_device, _deviceContext, _direct3D, this);
	_ssao->Initialize(_window->getWidth(), _window->getHeight(), 
		_camera->GetFar(), _camera->GetNear(), _camera->GetFoV());




	D3D11_BUFFER_DESC pfb;
	pfb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pfb.ByteWidth = sizeof(PerFrame);
	pfb.Usage = D3D11_USAGE_DYNAMIC;
	pfb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pfb.MiscFlags = 0;
	pfb.StructureByteStride = 0;

	_device->CreateBuffer(&pfb, 0, &_pfb);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;


	_device->CreateSamplerState(&samplerDesc, &_sampler);
	_deviceContext->PSSetSamplers(0, 1, &_sampler);
	_deviceContext->VSSetConstantBuffers(0, 1, &_pfb);
	_deviceContext->PSSetConstantBuffers(0, 1, &_pfb);
}

void App::SwitchActive(string name) {
	_activeMesh = _meshes[name];
}

void App::RegisterMesh(string name, Mesh* m) {
	_meshes[name] = m;
}

void App::Update() {

	static float constyrot = 0;
	static float xrot = 0;
	static float yrot = 0;
	static float zrot = 0;
	float movefactor = 0.00125 * _timer->GetTime();
	constyrot += movefactor * 0.05;
	static bool keystate = false;
	if(constyrot > 6.28) constyrot = 0;
	if(GetAsyncKeyState(0x26)) { //up
		_camera->Walk(movefactor);
	}
	if(GetAsyncKeyState(0x25)) { //left
		_camera->Strafe(-movefactor);
	}
	if(GetAsyncKeyState(0x28)) { //down
		_camera->Walk(-movefactor);
	}
	if(GetAsyncKeyState(0x27)) { //right
		_camera->Strafe(movefactor);
	}
	if(GetAsyncKeyState(0x58)) { //X
		_camera->Levitate(movefactor);
	}
	if(GetAsyncKeyState(0x5A)) { //Z
		_camera->Levitate(-movefactor);
	}
	if(GetAsyncKeyState(0x57)) { //W
		_camera->Pitch(movefactor);
	}
	if(GetAsyncKeyState(0x53)) { //S
		_camera->Pitch(-movefactor);
	}
	if(GetAsyncKeyState(0x41)) { //A
		_camera->RotateY(movefactor);
	}
	if(GetAsyncKeyState(0x44)) { //D
		_camera->RotateY(-movefactor);
	}
	if(GetAsyncKeyState(0x45)) { //E
		_camera->Roll(-movefactor);
	}
	if(GetAsyncKeyState(0x51)) { //Q
		_camera->Roll(movefactor);
	}
	if(GetAsyncKeyState(0x43)) { //C
		if(!keystate) {
			keystate = true;
			
			_occOn = !_occOn;
		}
	}
	else {keystate = false;}
	XMVECTOR xaxis = XMLoadFloat4(&XMFLOAT4(1.f, 0.f, 0.f, 0.f));
	XMMATRIX xrotation = XMMatrixRotationAxis(xaxis, xrot);
	XMVECTOR yaxis = XMLoadFloat4(&XMFLOAT4(0.f, 1.f, 0.f, 0.f));
	XMMATRIX yrotation = XMMatrixRotationAxis(yaxis, yrot);
	XMVECTOR zaxis = XMLoadFloat4(&XMFLOAT4(0.f, 0.f, 1.f, 0.f));
	XMMATRIX zrotation = XMMatrixRotationAxis(zaxis, zrot);
	XMMATRIX consty = XMMatrixRotationAxis(yaxis, constyrot);
	XMMATRIX wor = XMLoadFloat4x4(&_world);
	XMStoreFloat4x4(&_rot, consty * xrotation * yrotation * zrotation * wor);
	_dirty = true;
	UpdatePFB(*_camera->GetView(), *_camera->GetProj(), _rot, *_camera->GetOrtho());
}

void App::UpdatePFB(XMFLOAT4X4 viewMat, XMFLOAT4X4 projMat, XMFLOAT4X4 worldMat, XMFLOAT4X4 orthoMat) {
	D3D11_MAPPED_SUBRESOURCE mappedResources;
	HRESULT hret;
	XMFLOAT4X4 view, proj, world, worldView, worldViewProj, invTransWV, ortho, toTex;
	XMMATRIX view_mat, proj_mat, world_mat, worldView_mat, worldViewProj_mat, invTransWV_mat, ortho_mat, toTex_mat;
	//Transpose the matrices before sending to shader
	
	toTex = XMFLOAT4X4(0.5, 0.0, 0.0, 0.0,
					   0.0, 0.5, 0.0, 0.0,
					   0.0, 0.0, 1.0, 0.0,
					   0.5, 0.5, 1.0, 1.0);
	
	toTex_mat			= XMLoadFloat4x4(&toTex); 
	view_mat			= XMLoadFloat4x4(&viewMat); 
	proj_mat			= XMLoadFloat4x4(&projMat); 
	world_mat			= XMLoadFloat4x4(&worldMat);
	worldView_mat		= world_mat * view_mat;
	worldViewProj_mat	= world_mat * view_mat * proj_mat;
	invTransWV_mat		= XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(worldView_mat), worldView_mat));
	ortho_mat			= XMLoadFloat4x4(&orthoMat); 

	toTex_mat			= XMMatrixTranspose(toTex_mat);
	view_mat			= XMMatrixTranspose(view_mat);
	proj_mat			= XMMatrixTranspose(proj_mat);
	world_mat			= XMMatrixTranspose(world_mat);
	worldView_mat		= XMMatrixTranspose(worldView_mat);
	worldViewProj_mat	= XMMatrixTranspose(worldViewProj_mat);
	invTransWV_mat		= XMMatrixTranspose(invTransWV_mat);
	ortho_mat			= XMMatrixTranspose(ortho_mat);

	XMStoreFloat4x4(&toTex, toTex_mat);
	XMStoreFloat4x4(&view, view_mat);
	XMStoreFloat4x4(&proj, proj_mat);
	XMStoreFloat4x4(&world, world_mat);
	XMStoreFloat4x4(&worldView, worldView_mat);
	XMStoreFloat4x4(&worldViewProj, worldViewProj_mat);
	XMStoreFloat4x4(&invTransWV, invTransWV_mat);
	XMStoreFloat4x4(&ortho, ortho_mat);

	hret = _deviceContext->Map(_pfb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if(FAILED(hret))
	{}

	PerFrame* pf = (PerFrame*)mappedResources.pData;
	pf->proj = proj;
	pf->view = view;
	pf->world = world;
	pf->worldView = worldView;
	pf->worldViewProj = worldViewProj;
	pf->invTransWV = invTransWV;
	pf->ortho = ortho;
	pf->toTex = toTex;
	pf->viewPos = _camera->GetPosition();
	pf->on = _occOn ? 1.0 : 0.0;

	_deviceContext->Unmap(_pfb, 0);
}

void App::Render() {

	

	_ssao->NormalAndDepthPass();
	_activeMesh->Draw();
	_ssao->SSAOPass();
	_direct3D->ResetBackbufferRenderTarget();

	_direct3D->BeginScene();
	_ssao->BindSSAOBlurPass(5);
	_basic->SetActive();
	_activeMesh->Draw();
	_ssao->UnindSSAOBlurPass();




	_ssao->Debug();
	//_ssao->DrawSSAO();


	

	TwDraw();
	_direct3D->EndScene();

	
}

void App::Run()
{
	bool done = false;
	MSG msg;

	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(TwEventWin(_window->getHandle(), msg.message, msg.wParam, msg.lParam)) {
				_dirty = true;
				continue;
			}
			if(msg.message == WM_QUIT) { done = true; }
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			_timer->Frame();
			Update();
			if(_dirty) {
				Render();
				_dirty = false;
			}
			
		}
	}
}