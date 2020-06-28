#pragma once


struct CONSTANT
{

	XMFLOAT4X4				WorldMatrix;
	XMFLOAT4X4				ViewMatrix;
	XMFLOAT4X4				ProjectionMatrix;

	XMFLOAT4				CameraPosition;
	XMFLOAT4				CameraDirection;

	XMFLOAT3				iResolution;
	float					iTime;
	XMFLOAT4				iMouse;
	//XMFLOAT2				padding;
};

class CShader
{
private:
	ID3D11VertexShader*     m_VertexShader;
	ID3D11PixelShader*      m_PixelShader;
	ID3D11InputLayout*      m_VertexLayout;

	ID3D11Buffer*			m_ConstantBuffer;
	CONSTANT				m_Constant;

	ID3D11Buffer*			m_LightBuffer;
	LIGHT					m_Light;

public:
	void Init( const char* VertexShader, const char* PixelShader );
	void Uninit();
	void Set();

	void SetWorldMatrix(XMFLOAT4X4* WorldMatrix) { m_Constant.WorldMatrix = Transpose( WorldMatrix ); }
	void SetViewMatrix(XMFLOAT4X4* ViewMatrix) { m_Constant.ViewMatrix = Transpose( ViewMatrix ); }
	void SetProjectionMatrix(XMFLOAT4X4* ProjectionMatrix) { m_Constant.ProjectionMatrix = Transpose( ProjectionMatrix ); }
	void SetResolutin(XMFLOAT3 resolution) { m_Constant.iResolution = resolution; }
	void SetTime(float time) { m_Constant.iTime = time; }
	void SetiMouse(XMFLOAT4 mousePos) { m_Constant.iMouse = mousePos; }
	
	//void SetPrameter(XMFLOAT2 Parameter) { m_Constant.padding = Parameter; }

	void SetCameraPosition(XMFLOAT3* CameraPosition)
	{
		m_Constant.CameraPosition.x = CameraPosition->x;
		m_Constant.CameraPosition.y = CameraPosition->y;
		m_Constant.CameraPosition.z = CameraPosition->z;
		m_Constant.CameraPosition.w = 1.0f;
	}

	void SetCameraDirection(XMFLOAT4* CameraDirection)
	{
		m_Constant.CameraDirection.x = CameraDirection->x;
		m_Constant.CameraDirection.y = CameraDirection->y;
		m_Constant.CameraDirection.z = CameraDirection->z;
		m_Constant.CameraDirection.w = CameraDirection->w;
	}


	XMFLOAT4X4 Transpose(XMFLOAT4X4* Matrix)
	{
		XMFLOAT4X4 outMatrix;

		outMatrix._11 = Matrix->_11;
		outMatrix._12 = Matrix->_21;
		outMatrix._13 = Matrix->_31;
		outMatrix._14 = Matrix->_41;

		outMatrix._21 = Matrix->_12;
		outMatrix._22 = Matrix->_22;
		outMatrix._23 = Matrix->_32;
		outMatrix._24 = Matrix->_42;

		outMatrix._31 = Matrix->_13;
		outMatrix._32 = Matrix->_23;
		outMatrix._33 = Matrix->_33;
		outMatrix._34 = Matrix->_43;

		outMatrix._41 = Matrix->_14;
		outMatrix._42 = Matrix->_24;
		outMatrix._43 = Matrix->_34;
		outMatrix._44 = Matrix->_44;

		return outMatrix;
	}

};