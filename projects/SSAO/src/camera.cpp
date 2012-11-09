#include "Camera.h"


Camera::Camera()
{
	XMMATRIX identity_matrix = XMMatrixIdentity();
	XMVECTOR zero_vector = XMVectorZero();

	XMStoreFloat4x4(&m_view_proj, identity_matrix);
	XMStoreFloat4x4(&m_projection, identity_matrix);
	XMStoreFloat4x4(&m_view, identity_matrix);
	XMStoreFloat3(&m_position, zero_vector);
	XMStoreFloat3(&m_up, zero_vector);
	XMStoreFloat3(&m_look, zero_vector);
}

Camera::~Camera()
{}

XMFLOAT3	Camera::GetPosition() { return m_position; }

void Camera::Translate(XMFLOAT3 translation) {
	XMStoreFloat3(&m_position, XMLoadFloat3(&translation) + XMLoadFloat3(&m_position));
	XMStoreFloat3(&m_look, XMLoadFloat3(&translation) + XMLoadFloat3(&m_look));
	XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(XMLoadFloat3(&m_position), XMLoadFloat3(&m_look), XMLoadFloat3(&m_up)));
}

void Camera::PitchYawRoll(float pitchRad, float yawRad, float rollRad) {
// 	XMMATRIX rot =	XMMatrixRotationRollPitchYaw(pitchRad, yawRad, rollRad);
// 	XMVECTOR look = XMLoadFloat3(&m_look);
// 	XMVECTOR pos = XMLoadFloat3(&m_position);
// 	XMVECTOR up = XMLoadFloat3(&m_up);
// 	XMVECTOR left = XMVector3Cross(look, up);
// 	
// 	//TODO: rotate up, look vectors instead
// 
// 	XMStoreFloat4x4(&m_view, XMLoadFloat4x4(&m_view) * rot);
// 	XMVector3TransformNormal()
// // 	XMMATRIX roll_mat = XMMatrixRotationAxis(up, rollRad);
// // 	XMMatrixTransformation()
	XMVECTOR look = XMLoadFloat3(&m_look);
	XMVECTOR pos = XMLoadFloat3(&m_position);
	XMVECTOR up = XMLoadFloat3(&m_up);
	XMVECTOR left = XMVector3Cross(up, look);
	XMFLOAT3 t;
	XMStoreFloat3(&t, left);

	XMMATRIX R = XMMatrixRotationAxis(left, pitchRad);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(up, R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(look, R));

	UpdateView();

// 	R = XMMatrixRotationAxis(look, rollRad);
// 	XMStoreFloat3(&m_up, XMVector3TransformNormal(up, R));
// 
// 	UpdateView();
// 
// 	R = XMMatrixRotationAxis(up, yawRad);
// 	XMStoreFloat3(&m_look, XMVector3TransformNormal(look, R));
// 
// 	UpdateView();

	

}

void Camera::RotateY(float rad) {
	XMMATRIX R = XMMatrixRotationY(-rad);
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
	UpdateView();
}

void Camera::Pitch(float rad) {
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), -rad);
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));

	UpdateView();
}
void Camera::Yaw(float rad) {
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_up), -rad);
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));

	UpdateView();
}
void Camera::Roll(float rad) {
// 	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_up), -rad);
// 	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
// 	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
// 
// 	UpdateView();
}

void Camera::Walk(float d) {
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&m_look);
	XMVECTOR p = XMLoadFloat3(&m_position);

	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s,l,p));

	UpdateView();
}
void Camera::Strafe(float d) {
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&m_right);
	XMVECTOR p = XMLoadFloat3(&m_position);

	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s,r,p));

	UpdateView();
}
void Camera::Levitate(float d) {
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR u = XMLoadFloat3(&m_up);
	XMVECTOR p = XMLoadFloat3(&m_position);

	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s,u,p));

	UpdateView();
}

void Camera::UpdateView() {
	XMVECTOR L = XMLoadFloat3(&m_look);
	XMVECTOR P = XMLoadFloat3(&m_position);
	XMVECTOR U = XMLoadFloat3(&m_up);
	XMVECTOR R = XMLoadFloat3(&m_right);

	//re-orthogonalize
	L = XMVector3Normalize((L));

	U = XMVector3Normalize(XMVector3Cross(L, R));
	R = XMVector3Cross(U, L);

	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_look, L);

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	m_view(0, 0) = m_right.x;
	m_view(1, 0) = m_right.y;
	m_view(2, 0) = m_right.z;
	m_view(3, 0) = x;

	m_view(0, 1) = m_up.x;
	m_view(1, 1) = m_up.y;
	m_view(2, 1) = m_up.z;
	m_view(3, 1) = y;

	m_view(0, 2) = m_look.x;
	m_view(1, 2) = m_look.y;
	m_view(2, 2) = m_look.z;
	m_view(3, 2) = z;

	m_view(0, 3) = 0.f;
	m_view(1, 3) = 0.f;
	m_view(2, 3) = 0.f;
	m_view(3, 3) = 1.f;
}

void Camera::InitCamera(int width, int height, float nearClip /*= DEFAULT_NEARCLIP*/, 
	float farClip /*= DEFAULT_FARCLIP*/, float fov /*= XM_PI / 3 */)
{
	m_nearClip = nearClip;
	m_farClip = farClip;
	m_fov = fov;
	ResizeProjection(width, height);
	SetView();
}

void Camera::SetView()
{
	XMVECTOR pos, up, look, right;
	XMMATRIX matrix, proj;

	pos	= XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	XMStoreFloat3(&m_position, pos);

	up	= XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMStoreFloat3(&m_up, up);

	look = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMStoreFloat3(&m_look, look);

	right = XMVector3Cross(up, look);
	XMStoreFloat3(&m_right, right);

	matrix = XMMatrixLookAtLH(pos, look, up);
	XMStoreFloat4x4(&m_view, matrix);

	//update view_proj matrix
	proj = XMLoadFloat4x4(&m_projection);
	matrix = matrix * proj;
	XMStoreFloat4x4(&m_view_proj, matrix);

	UpdateView();
}

void Camera::ResizeProjection(int width, int height)
{
	XMMATRIX matrix, view; 

	float aspect = (float)width / (float)height;
	matrix = XMMatrixPerspectiveFovLH(m_fov, aspect, m_nearClip, m_farClip);
	XMStoreFloat4x4(&m_projection, matrix);

	matrix = XMMatrixOrthographicLH(width, height, m_nearClip, m_farClip);
	XMStoreFloat4x4(&m_ortho, matrix);

	//update view_proj matrix
	view = XMLoadFloat4x4(&m_view);
	matrix = view * matrix;
	XMStoreFloat4x4(&m_view_proj, matrix);
}

XMFLOAT4X4* Camera::GetView()	{ return &m_view; }
XMFLOAT4X4* Camera::GetProj()	{ return &m_projection; }
XMFLOAT4X4* Camera::GetVP()		{ return &m_view_proj; }
XMFLOAT4X4* Camera::GetOrtho()	{ return &m_ortho; }
