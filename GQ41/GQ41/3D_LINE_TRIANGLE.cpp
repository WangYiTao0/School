//=======================================================================================
//
//	  [ 3D_LINE_TRIANGLE ] : �R�c��Ԃɂ���������ƎO�p�`�Ƃ̌�������i���K�p�j
//
//=======================================================================================

//-------- �C���N���[�h�w�b�_
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define STRICT
//-------- ���C�u���������N
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>		// Windows�v���O�����ɂ͂����t����
#include <tchar.h>			// �ėp�e�L�X�g �}�b�s���O
#include <stdio.h>
#include <math.h>			// �O�p�֐��Ŏg�p
#include <mmsystem.h>		// ���ԊǗ��Ŏg�p
#include <d3d9.h>			// DirectX Graphics �֘A�̃w�b�_
#include <d3dx9.h>			// DirectX Graphics �֘A�̃w�b�_

#include "Collision.h"


//-------- �}�N��
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if(x){ x->Release(); x=NULL; }
#endif

//-------- �萔��`
#define CLASS_NAME		_T("3D_LINE_TRIANGLE")											// �E�B���h�E�N���X��
//#define CAPTION_NAME	_T("[ 3D_LINE_TRIANGLE ] �R�c��Ԃɂ���������ƎO�p�`�Ƃ̌�������")	// �L���v�V������
#define CAPTION_NAME	_T("AT14C184_06_70732_�I�E�C�b�g�E")	// �L���v�V������

#define SCREEN_WIDTH	640									// �X�N���[���̕�
#define SCREEN_HEIGHT	480									// �X�N���[���̍���
#define FRAME_RATE		1000.0f/60.0f						// �t���[�����[�g
#define ASPECT_RATIO	(float)SCREEN_WIDTH/SCREEN_HEIGHT	// �A�X�y�N�g��
#define NEAR_CLIP		100.0f								// �j�A�N���b�v���s������
#define FAR_CLIP		30000.0f							// �t�@�[�N���b�v���s������
#define LINE_MOVE_SPEED	0.2f								// ���C���ړ����x

//-------- ���_�t�H�[�}�b�g
typedef struct {
	D3DXVECTOR3	pos;
	DWORD		diffuse;
} VERTEX;

#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

//-------- �ϐ�
LPDIRECT3D9				g_pD3D;					// Direct3D9 �I�u�W�F�N�g
LPDIRECT3DDEVICE9		g_pD3DDevice;			// Direct3DDevice9 �I�u�W�F�N�g
D3DPRESENT_PARAMETERS	g_D3DPresentParam;		// PRESENT PARAMETERS
LPD3DXFONT				g_pD3DXFont;			// D3DXFont �I�u�W�F�N�g
HWND					g_hWnd;					// �E�B���h�E�n���h��
TCHAR					g_szDebug[1024];		// �f�o�b�O�p�X�g�����O�f�[�^

int						g_MousePosX   , g_MousePosY;	// �}�E�X�ɂ���]�p
int						g_MouseOldPosX, g_MouseOldPosY;
int						g_MouseSpeedX , g_MouseSpeedY;
bool					g_MouseButtonL;
float					g_CameraDeg;

D3DXMATRIX				g_MatView;						// �r���[�}�g���b�N�X

VERTEX					g_VertexTriangle[3];			// �O�p�`�̒��_
VERTEX					g_VertexLine[2];				// �����̒��_

bool					g_bOnHit;					// ������`�F�b�N�t���O

//-------- �v���g�^�C�v�錾
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitializeGraphics(HWND hWnd, bool bWindow);
HRESULT CleanupGraphics();
HRESULT RenderGraphics();
void ExecMain();
void DrawMain();
bool HitCheck();

//---------------------------------------------------------------------------------------
//	���C��
//---------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int iCmdShow)
{
	MSG		msg;
	DWORD	dwExecLastTime;
	DWORD	dwCurrentTime;

	// �E�B���h�E�N���X�̓o�^
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc,
		0, 0, hInstance, LoadIcon(hInstance, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, CLASS_NAME, NULL};
	if (RegisterClassEx(&wc) == 0) return false;	// �E�B���h�E�N���X��o�^

	// �E�B���h�E���쐬
	g_hWnd = CreateWindow(
		CLASS_NAME, CAPTION_NAME, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL, NULL, hInstance, NULL);
	if (g_hWnd == NULL) return false;
	ShowWindow(g_hWnd, iCmdShow);		// �E�B���h�E��\��
	UpdateWindow(g_hWnd);				// �E�B���h�E���A�b�v�f�[�g

	// �O���t�B�b�N���̏�����
	bool	bWindow = false;
	if (IDYES == MessageBox(g_hWnd, _T("�E�B���h�E���[�h�Ŏ��s���܂����H"), _T("��ʃ��[�h"), MB_YESNO))
		bWindow = true;
	if (FAILED(InitializeGraphics(g_hWnd, bWindow))) return false;		// �����ŏ�����

	// �e�평����
	timeBeginPeriod(1);									// �^�C�}�[�̕���\���ŏ��ɃZ�b�g
	dwExecLastTime = timeGetTime();						// ���݂̃V�X�e���^�C�}�[���擾
	g_MouseButtonL = false;
	g_CameraDeg = 0.0f;

	// ���b�Z�[�W���[�v
	msg.message = WM_NULL;
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {		// ���b�Z�[�W���`�F�b�N
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			dwCurrentTime = timeGetTime();						// ���݂̃^�C�}�[�l���擾
			if (dwCurrentTime - dwExecLastTime > FRAME_RATE) {
				ExecMain();										// ���C������
				RenderGraphics();								// �����_�����O����	
			}
		}
		Sleep(1);					// ��������Windows�ɐ����߂�
	}

	// �O���t�B�b�N���̃N���[���A�b�v
	CleanupGraphics();
	timeEndPeriod(1);				// �^�C�}�[�̕���\�����ɖ߂�

	return msg.wParam;
}

//---------------------------------------------------------------------------------------
//	�E�B���h�E�v���V�[�W��
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:				//----- �E�B���h�E�j���w��������
		PostQuitMessage(0);				// �V�X�e���ɃX���b�h�̏I����v��
		return 0;
	case WM_KEYDOWN:				//----- �L�[�{�[�h�������ꂽ
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
			return 0;
		}
		break;
	case WM_MOUSEMOVE:				//----- �}�E�X�J�[�\�����ړ�����
		if (g_MouseButtonL) {
			g_MousePosX = LOWORD(lParam);
			g_MousePosY = HIWORD(lParam);
		}
		return 0;
	case WM_LBUTTONDOWN:			//----- �}�E�X�̍��{�^���������ꂽ
		g_MouseButtonL = true;
		g_MouseOldPosX = g_MousePosX = LOWORD(lParam);
		g_MouseOldPosY = g_MousePosY = HIWORD(lParam);
		return 0;
	case WM_LBUTTONUP:				//----- �}�E�X�̍��{�^���������ꂽ
		g_MouseButtonL = false;
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//---------------------------------------------------------------------------------------
// �O���t�B�b�N���̏�����
//---------------------------------------------------------------------------------------
HRESULT InitializeGraphics(HWND hWnd, bool bWindow)
{
	// �f�o�C�X���e�[�u��
	struct TCreateDevice {
		D3DDEVTYPE type;			// �f�o�C�X�̎��
		DWORD      behavior;		// �f�o�C�X�̓���
	};
	const int c_nMaxDevice = 3;
	const TCreateDevice device[c_nMaxDevice] = {
		{ D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING },	// �n�[�h�E�F�A�V�F�[�_�[���g��
		{ D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING },	// HAL
		{ D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING },	// REF
	};
	int		nDev;			// �f�o�C�X��ʃJ�E���^

	// Direct3D�I�u�W�F�N�g�𐶐�
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
		MessageBox(hWnd, _T("Direct3D�I�u�W�F�N�g�̍쐬�Ɏ��s���܂���"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	// Direct3D �������p�����[�^�̐ݒ�
	ZeroMemory(&g_D3DPresentParam, sizeof(D3DPRESENT_PARAMETERS));
	g_D3DPresentParam.BackBufferWidth        = SCREEN_WIDTH;			// �Q�[����ʃT�C�Y
	g_D3DPresentParam.BackBufferHeight       = SCREEN_HEIGHT;
	g_D3DPresentParam.Windowed               = bWindow;					// �E�B���h�E���[�h��?
	g_D3DPresentParam.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	g_D3DPresentParam.EnableAutoDepthStencil = TRUE;					// �y�o�b�t�@�L��
	g_D3DPresentParam.AutoDepthStencilFormat = D3DFMT_D24S8;			// �y�o�b�t�@�t�H�[�}�b�g
	if (bWindow) {
		// �E�B���h�E���[�h
		g_D3DPresentParam.BackBufferFormat           = D3DFMT_UNKNOWN;				// �o�b�N�o�b�t�@
		g_D3DPresentParam.FullScreen_RefreshRateInHz = 0;							// ���t���b�V�����[�g
		g_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// ����������҂��Ȃ�
	} else {
		// �t���X�N���[�����[�h
		g_D3DPresentParam.BackBufferFormat           = D3DFMT_X8R8G8B8;				// �o�b�N�o�b�t�@
		g_D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// ���t���b�V�����[�g
		g_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;	// ����������҂�
	}

	// �f�o�C�X�I�u�W�F�N�g���쐬
	for (nDev = 0; nDev < c_nMaxDevice; nDev++) {
		if (SUCCEEDED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, device[nDev].type, hWnd,	// �f�o�C�X���쐬
			device[nDev].behavior, &g_D3DPresentParam, &g_pD3DDevice))) break;
	}
	if (nDev >= c_nMaxDevice) {
		MessageBox(hWnd, _T("�f�o�C�X�̍쐬�Ɏ��s���܂���"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// ���\���p�t�H���g��ݒ�
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Terminal"), &g_pD3DXFont);

	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	D3DXMATRIX	matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(30), ASPECT_RATIO, NEAR_CLIP, FAR_CLIP);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// ���[���h�}�g���b�N�X�ݒ�
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// �����_�����O�X�e�[�g��ݒ�
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		// �����v�Z���n�e�e
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// �J�����O�����Ȃ�
	
	// ���_�o�b�t�@�A���_�t�H�[�}�b�g�t���O��ݒ�
	g_pD3DDevice->SetFVF(FVF_VERTEX);

	// �I�u�W�F�N�g�̏�����
	// �O�p�`�F���_�o�b�t�@�ɒ��_�l��ݒ�
	g_VertexTriangle[0].pos = D3DXVECTOR3(-100.0f,  100.0f, 100.0f);
	g_VertexTriangle[1].pos = D3DXVECTOR3( 100.0f,  100.0f,  50.0f);
	g_VertexTriangle[2].pos = D3DXVECTOR3(-100.0f, -100.0f,   0.0f);
	
	// �����F���_�o�b�t�@�ɒ��_�l��ݒ�
	g_VertexLine[0].pos     = D3DXVECTOR3(-200.0f,  100.0f, -200.0f);
	g_VertexLine[1].pos     = D3DXVECTOR3(-200.0f, -100.0f,    0.0f);
	g_VertexLine[0].diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	g_VertexLine[1].diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	return S_OK;
}

//---------------------------------------------------------------------------------------
// �O���t�B�b�N���̃N���[���A�b�v
//---------------------------------------------------------------------------------------
HRESULT CleanupGraphics()
{
	// �I�u�W�F�N�g�̌�n��
	SAFE_RELEASE(g_pD3DXFont);			// D3DXFont �I�u�W�F�N�g�����
	SAFE_RELEASE(g_pD3DDevice);			// Direct3DDevice �I�u�W�F�N�g�����
	SAFE_RELEASE(g_pD3D);				// Direct3D �I�u�W�F�N�g�����
	return S_OK;
}

//---------------------------------------------------------------------------------------
// �O���t�B�b�N���̃����_�����O
//---------------------------------------------------------------------------------------
HRESULT RenderGraphics()
{
	// �r���[�|�[�g���N���A
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xff,0xff,0xff), 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene())) {	// �V�[���J�n

		DrawMain();									// �`�揈��

		g_pD3DDevice->EndScene();					// �V�[���I��
	}

	// �o�b�N�o�b�t�@�ɕ`�悵�����e��\��
	if (FAILED(g_pD3DDevice->Present(NULL, NULL, NULL, NULL))) {
		g_pD3DDevice->Reset(&g_D3DPresentParam);
	}
	return S_OK;
}


//---------------------------------------------------------------------------------------
// ���C�������i���C�����[�v����R�[�������j
//---------------------------------------------------------------------------------------
void ExecMain()
{
	TCHAR		str[256];

	g_szDebug[0] = _T('\0');		// �f�o�b�O�X�g�����O������

	_stprintf(str, _T("�}�E�X�̍��E�ŃJ������]\n"));
	lstrcat(g_szDebug, str);
	_stprintf(str, _T("�J�[�\���L�[�Ń��C���̈ړ��i�w�y�����j\n"));
	lstrcat(g_szDebug, str);

	//-------- �������J�[�\���ňړ�
	if (GetAsyncKeyState(VK_LEFT ) & 0x8000) {
		g_VertexLine[0].pos.x -= LINE_MOVE_SPEED;
		g_VertexLine[1].pos.x -= LINE_MOVE_SPEED;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		g_VertexLine[0].pos.x += LINE_MOVE_SPEED;
		g_VertexLine[1].pos.x += LINE_MOVE_SPEED;
	}
	if (GetAsyncKeyState(VK_UP   ) & 0x8000) {
		g_VertexLine[0].pos.z += LINE_MOVE_SPEED;
		g_VertexLine[1].pos.z += LINE_MOVE_SPEED;
	}
	if (GetAsyncKeyState(VK_DOWN ) & 0x8000) {
		g_VertexLine[0].pos.z -= LINE_MOVE_SPEED;
		g_VertexLine[1].pos.z -= LINE_MOVE_SPEED;
	}

	//----- �}�E�X�ړ��ŃJ�����𓮂���
	g_MouseSpeedX = g_MousePosX - g_MouseOldPosX;
	g_MouseSpeedY = g_MousePosY - g_MouseOldPosY;
	g_MouseOldPosX = g_MousePosX;
	g_MouseOldPosY = g_MousePosY;
	g_CameraDeg -= (float)g_MouseSpeedX * 0.01f;

	//----- �����蔻��I
	g_bOnHit = HitCheck();
}

//---------------------------------------------------------------------------------------
// �`�揈���iRenderGraphics ����R�[�������j
//---------------------------------------------------------------------------------------
void DrawMain()
{
	// �r���[�}�g���b�N�X�ݒ�
	float px, pz;
	px =  sinf(g_CameraDeg) * 1000.0f;
	pz = -cosf(g_CameraDeg) * 1000.0f;
	D3DXMatrixLookAtLH(&g_MatView,
						&D3DXVECTOR3(   px,  0.0,     pz),		// ���_���W
						&D3DXVECTOR3(  0.0,  0.0,    0.0),		// �����_���W
						&D3DXVECTOR3(  0.0,  1.0,    0.0));		// �A�b�v�x�N�g��
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &g_MatView);

	// �����蔻��t���O�ɂ���ĎO�p�`�̃J���[��ω�������
	DWORD	color;
	if (g_bOnHit) color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	else		  color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	g_VertexTriangle[0].diffuse = color;
	g_VertexTriangle[1].diffuse = color;
	g_VertexTriangle[2].diffuse = color;

	// �O�p�`�A�����̕`������s
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &g_VertexTriangle[0], sizeof(VERTEX));	// �O�p�`
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &g_VertexLine[0], sizeof(VERTEX));			// ����
	
	// �f�o�b�O�X�g�����O�`��
	RECT	strRect = {0, 0, 640, 480};
	g_pD3DXFont->DrawText(NULL, g_szDebug, -1, &strRect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0x00));
}

//---------------------------------------------------------------------------------------
// ��������`�F�b�N���W���[��
// in  : ---
//�@VERTEX�@g_VertexTriangle[3];	// �O�p�`�̒��_
//�@VERTEX�@g_VertexLine[2];		// �����̒��_
//
//
// out : bool true = �q�b�g���� , false = �q�b�g���Ȃ�����
//---------------------------------------------------------------------------------------
bool HitCheck()
{
	//projections
	D3DXVECTOR3 A = { g_VertexTriangle[0].pos.x,g_VertexTriangle[0].pos.y,g_VertexTriangle[0].pos.z };
	D3DXVECTOR3 B = { g_VertexTriangle[1].pos.x,g_VertexTriangle[1].pos.y,g_VertexTriangle[1].pos.z };
	D3DXVECTOR3 C = { g_VertexTriangle[2].pos.x,g_VertexTriangle[2].pos.y,g_VertexTriangle[2].pos.z };

	D3DXVECTOR3 L1 = { g_VertexLine[0].pos.x,g_VertexLine[0].pos.y,g_VertexLine[0].pos.z };
	D3DXVECTOR3 L2 = { g_VertexLine[1].pos.x,g_VertexLine[1].pos.y,g_VertexLine[1].pos.z };

	D3DXVECTOR3 Dir = L2 - L1;

	FLOAT fBary1, fBary2;
	FLOAT fDist;

	return IntersectTriangle( L1, Dir, A, B, C, &fDist,&fBary1,&fBary2);
	

	/*************************************************************************

	�����Ɍ�������̃v���O������ǉ�����

	�K�v�ƂȂ郍�[�J���ϐ��͎��R�ɍ��


	�iPhase 1)
	�E�O�p�`�̖@���x�N�g���i���K������j�����߂�
	�E�@���x�N�g���ɑ΂��Ē����𓊉e�������ʂ����߂�i�Q�_���݁B���ς��g���j
	�E�O�p�`�̒��_���W�𓊉e���ď�L�̒����𓊉e�����͈͓����ǂ�����������
	�E�͈͊O�Ȃ番���ʂ����݂���Ƃ��āu�Փ˂��Ă��Ȃ��v�Ƃ��ďI��

	�iPhase 2)
	�E�O�p�`�̃G�b�W��\���x�N�g���Ɛ����ɒ�������x�N�g�����O�ςŋ��߂�i���K������j
	�E�����x�N�g���ɎO�p�`�̂R���_�𓊉e����i���ς��g���j
	�E���e�����R�_����ŏ��l�A�ő�l�����߂āu�͈́v�����߂�
	�E�����̒��_�i�ǂ��炩��j���͈͓����ǂ�����������
	�E�͈͊O�Ȃ番���ʂ����݂���Ƃ��āu�Փ˂��Ă��Ȃ��v�Ƃ��ďI��
	�@��Phase2�̏������G�b�W�R���ɂ��ČJ��Ԃ���
	  
	�i�⑫�j
	�E�q�b�g������ true �A�q�b�g���Ȃ������� false �Ń��^�[������

	*************************************************************************/

	//return false;		// ��
}

//=======================================================================================
//	End of Files
//=======================================================================================