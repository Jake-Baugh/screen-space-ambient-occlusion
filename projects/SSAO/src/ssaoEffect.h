#ifndef _ssaoeffect_h_
#define _ssaoeffect_h_

#include "util.h"
#include "renderTarget.h"
#include "direct3D_11.h"
#include "shaderProgram.h"
#include <time.h>
#include "AntTweakBar.h"
#include "app.h"

class App;

struct SSAOBuffer {
	float farClip;
	float nearClip;
	float kernal_size;
	float width;
	float height;
	vec2 noiseScale;
	vec3 frustumCorner;
	vec3 sampleKernal[16];
	vec2 padding;
};
struct SSAOBuffer2 {
	float rad;
	float totStrength;
	float strength;
	float offset;
	float falloff;
	float blurSize;
	vec2 padding;
};

class SSAO {
private:
	struct Vertex {
		Vertex(vec4 p, vec3 t) : pos(p), tex(t) {};
		Vertex() {};
		vec4 pos;
		vec3 tex;
	};
public:
	SSAO(ID3D11Device* dev, ID3D11DeviceContext* dc, D3D* d3d, App* app);
	void Initialize(int screenWidth, int screenHeight, 
		float far, float near, float fov);

	void NormalAndDepthPass();
	void SSAOPass();
	void BindNormalDepthPass(int slot);
	void UnbindNormalDepthPass();
	void BindSSAOPass(int slot);
	void UnbindSSAOPass(); 
	void BindSSAOBlurPass(int slot);
	void UnindSSAOBlurPass();
	void DrawFSQ();
	void Debug();
	void DrawSSAO();

	

private:
	void DrawNormalMap();
	void DrawSSAOMap();


	void UpdateBuffer();
	void UpdateBuffer2();
	void BuildUI();
	void BuildNoise();
	void BuildSampleKernal();
	void BuildFullScreenQuad();
	void BuildFrustumCorner();
	void BuildTextures();
	float Random(float low, float high);
	float Lerp(float factor, float a, float b);
private:
	static void TW_CALL SetRad(const void *value, void * clientData);
	static void TW_CALL GetRad(void *value, void * clientData);

	static void TW_CALL SetTotStr(const void *value, void * clientData);
	static void TW_CALL GetTotStr(void *value, void * clientData);

	static void TW_CALL SetStrength(const void *value, void * clientData);
	static void TW_CALL GetStrength(void *value, void * clientData);

	static void TW_CALL SetOffset(const void *value, void * clientData);
	static void TW_CALL GetOffset(void *value, void * clientData);

	static void TW_CALL SetFalloff(const void *value, void * clientData);
	static void TW_CALL GetFalloff(void *value, void * clientData);

	static void TW_CALL SetBlur(const void *value, void * clientData);
	static void TW_CALL GetBlur(void *value, void * clientData);

	static void TW_CALL ResetButton(void *clientData);
	static void TW_CALL ShowDebug(void *clientData);

	static void TW_CALL SetMesh(const void *value, void * clientData);
	static void TW_CALL GetMesh(void *value, void * clientData);
private:
	TwBar* _bar;

	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

	RenderTarget* _normalAndDepthBuffer;
	RenderTarget* _ssaoRT;
	RenderTarget* _blurRT;

	ID3D11Buffer* _ssaoBuffer;
	ID3D11Buffer* _ssaoBuffer2;
	ID3D11Buffer* _fsqVb;
	ID3D11Buffer* _fsqIb;
	int _indexCount;

	int _screenWidth;
	int _screenHeight;
	float _near;
	float _far;
	float _fov;
	int _kernalSize;
	int _noiseSize;
	vec2 _noiseScale;

	vec3 _kernal[16];
	vec3* _noise;
	vec3 _frustumCorner;

	D3D* _direct3D;

	ShaderProgram* _ndPass;
	ShaderProgram* _ssaoPass;
	ShaderProgram* _blurPass;
	ShaderProgram* _debug;

	ID3D11Texture2D* _noiseTex;
	ID3D11ShaderResourceView* _noiseSRV; //8
	ID3D11Texture2D* _kernalTex; 
	ID3D11ShaderResourceView* _kernalSRV; //7


public:
	float _defblurSize;
	float _defrad;
	float _deftotStrength;
	float _defstrength;
	float _defoffset;
	float _deffalloff;

	float _blurSize;
	float _rad;
	float _totStrength;
	float _strength;
	float _offset;
	float _falloff;
	App* _app;


	int _index;
	bool _showDebug;


};

#endif