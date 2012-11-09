#ifndef _camera_h_
#define _camera_h_

#include "util.h"

#define DEFAULT_NEARCLIP	 (0.1f)
#define DEFAULT_FARCLIP   (1000.0f)

class Camera
{
public:
	Camera();
	~Camera();

	void InitCamera(int width, int height, float nearClip = DEFAULT_NEARCLIP, 
		float farClip = DEFAULT_FARCLIP, float fov = XM_PI / 3);
	void SetView();
	void ResizeProjection(int width, int height);
	void UpdateView();
	void Translate(XMFLOAT3 translation);
	void PitchYawRoll(float pitchRad, float yawRad, float rollRad);

	void Pitch(float rad);
	void Yaw(float rad);
	void Roll(float rad);

	void RotateY(float rad);

	void Walk(float d);
	void Strafe(float d);
	void Levitate(float d);

	XMFLOAT4X4* GetView();
	XMFLOAT4X4* GetProj();
	XMFLOAT4X4* GetVP();
	XMFLOAT4X4* GetOrtho();
	XMFLOAT3	GetPosition();
	float GetNear() { return m_nearClip; };
	float GetFar() { return m_farClip; };
	float GetFoV() { return m_fov; };
private:
	XMFLOAT4X4 m_projection;
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_ortho;
	XMFLOAT4X4 m_view_proj; //View * Proj

	XMFLOAT3 m_position;
	XMFLOAT3 m_look;
	XMFLOAT3 m_up;
	XMFLOAT3 m_right;

	float m_fov;
	float m_nearClip;
	float m_farClip;
};

#endif