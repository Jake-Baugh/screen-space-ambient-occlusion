#ifndef _app_h_
#define _app_h_

#include "util.h"
#include "window.h"
#include "camera.h"
#include "direct3D_11.h"
#include "timer.h"
#include "shaderProgram.h"
#include <vector>
#include <iostream>
#include "mesh.h"
#include "ssaoEffect.h"
#include <map>

using namespace std;

class SSAO;

struct PerFrame {
	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;
	XMFLOAT4X4 world;
	XMFLOAT4X4 worldView;
	XMFLOAT4X4 worldViewProj;
	XMFLOAT4X4 invTransWV;
	XMFLOAT4X4 ortho;
	XMFLOAT4X4 toTex;
	XMFLOAT3 viewPos;
	float on;
};


class App
{
public:
	~App();
	void RunApp();
	void SwitchActive(string name);
private:
	void InitApp();
	void Update();
	void Render();
	void Run();
	void UpdatePFB(XMFLOAT4X4 view, XMFLOAT4X4 proj, XMFLOAT4X4 world, XMFLOAT4X4 orthoMat);
	
	void RegisterMesh(string name, Mesh* m);
	
private:
	Window* _window;
	D3D* _direct3D;
	Timer* _timer;
	Camera* _camera;

	ID3D11DeviceContext* _deviceContext;
	ID3D11Device* _device;
	ID3D11Buffer* _pfb;
	ID3D11SamplerState* _sampler;
	
	bool _dirty;
	
	XMFLOAT4X4 _world;XMFLOAT4X4 _rot;

	Mesh* _mesh;
	Mesh* _activeMesh;
	ShaderProgram* _basic;

	SSAO* _ssao;
	bool _occOn;

	ID3D11ShaderResourceView* _statueSRV;

	std::map<string, Mesh*> _meshes;
};

#endif