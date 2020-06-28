#pragma once


class CCamera : public CGameObject
{
private:

	static CCamera* m_Instance;

	RECT m_Viewport;

	XMFLOAT4X4	m_ViewMatrix;
	XMFLOAT4X4	m_InvViewMatrix;
	XMFLOAT4X4	m_ProjectionMatrix;

	XMMATRIX	m_mViewMatrix;


	XMFLOAT3 m_CameraLookat;
	XMFLOAT3 m_CameraUp;

	XMVECTOR m_Eye;
	XMVECTOR m_LookAt;
	XMVECTOR m_Up;
public:
	CCamera() { m_Instance = this; }
	static CCamera* GetInstance() { return m_Instance; }

	void Init();
	void Uninit();
	void Update(float dt);
	void Draw();
	void DrawShadow();


	XMFLOAT4X4	GetViewMatrix();
	XMFLOAT4X4	GetInvViewMatrix() { return m_InvViewMatrix; }
	XMFLOAT4X4	GetProjectionMatrix() { return m_ProjectionMatrix; }

	XMFLOAT4 GetCameraDirection() { return { m_CameraLookat.x,m_CameraLookat.y,m_CameraLookat.z,1.0 }; }

};