#pragma once


#include "shader.h"


class CPolygon1 : public CGameObject
{
private:

	ID3D11Buffer*	m_VertexBuffer = NULL;
	CShader*		m_Shader;

	float iTime;

public:
	void Init();
	void Uninit();
	void Update(float dt);
	void Draw();

	float dt;

};