//=======================================================================================
//
//	  [ 2D_BSP ] : Binary Space Partitioning
//
//=======================================================================================
#define  _CRT_SECURE_NO_WARNINGS
//----- �C���N���[�h�t�@�C��
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include "Vector.h"

//----- �萔��`
#define SCREEN_WIDTH			800				// �X�N���[���@��
#define SCREEN_HEIGHT			600				// �X�N���[���@����
#define AXIS_X_OFFSET			SCREEN_WIDTH/2	// �w���W�I�t�Z�b�g
#define AXIS_Y_OFFSET			SCREEN_HEIGHT/2	// �x���W�I�t�Z�b�g

#if 1

//����
//�ǂ̓r���ŕ��f����Ȃ��悤�ɂ��ׂĈ�ӂ�50�̕ǂō\������B
POINT	poly_vertex[] =
{			// ���_���W
	//  x    y}
	{-150	,	-50},
	{-150	,	-100},
	{-150	,	-150},
	{-150	,	-200},
	{-100	,	-200},
	{-50	,	-200},
	{0	,	-200},
	{50	,	-200},
	{50	,	-150},
	{50	,	-100},
	{50	,	-50},
	{50	,	0},
	{50	,	50},
	{0	,	50},
	{0	,	0},
	{0	,	-50},
	{0	,	-100},
	{-50	,	-100},
	{-50	,	-50},
	{-100	,	-50},

	{-150	,	-50},//�X�^�[�g�̒��_�֖߂����߂̃f�[�^��K���Ō�ɂ���

};
#define POLY_MAX	20				// �ʐ�

#else

// ��2����͂���Ȃ�
POINT	poly_vertex[] = {			// ���_���W
	//  x    y
	{  50, 200},		// 0
	{-100, 200},		// 1
	{-100,  50},		// 2
	{ 175,  50},		// 3
	{ 175, -50},		// 4
	{  25, -50},		// 5
	{  25,-150},		// 6
	{ 300,-150},		// 7
	{ 300, 200},		// 8
	{ 150, 200},		// 9
	{ 150, 100},		// 10
	{  50, 100},		// 11

	{  50, 200},		// 12
};
#define POLY_MAX	12				// �ʐ�

#endif

//----- �J�x�\���̒�`
typedef struct {
	POINT	p0, p1;			// ���W
	float	nx, ny;			// �@���x�N�g��
	TCHAR	name[256];		// �ʂ̖��O
	bool	select;
} FACE;

struct BSPnode {
	BSPnode	*front;			// �\���A�����m�[�h�ւ̃|�C���^
	BSPnode	*back;			// �\���A�����m�[�h�ւ̃|�C���^
	int		face;			// ������
	int		*front_face;	// �\���ʃ��X�g�ւ̃|�C���^
	int		*back_face;		// �����ʃ��X�g�ւ̃|�C���^
	int		front_num;		// �\���̖ʐ�
	int		back_num;		// �����̖ʐ�
};

//----- �O���[�o���ϐ�
const TCHAR szClassName[] = _T("2D_BSP");
//const TCHAR szAppName[]   = _T("[ 2D_BSP ] Binary Space Partitioning�@�������`��");//������ύX���Ă���
const TCHAR szAppName[] = _T("AT14C184_06_70732_�I�E�C�b�g�E");//������ύX���Ă���


FACE		g_Face[POLY_MAX * POLY_MAX];		// �ʏ��
int			g_FaceIndex[POLY_MAX * POLY_MAX];	// �ʎg�p�����
BSPnode		*g_BSPnode;							// ���[�g�m�[�h�p�|�C���^

HWND		g_hWndApp;					// �E�B���h�E�n���h��
HDC			g_hMemDC;					// ����ʂc�b
HBITMAP		g_hBitmap;					// ����ʃr�b�g�}�b�v
bool		g_bOnInfo;					// �x�N�g���E���l���\���X�C�b�`

int			g_DispTreeX;				// �a�r�o�c���[�\���w���W
int			g_DispTreeY;				// �a�r�o�c���[�\���x���W

int			g_nCursor = -1;

HPEN		PenGrid0, PenGrid1;
HPEN		PenBand, PenDot, PenCursor, PenSelect, PenNormal, PenSep;

// ���������
POINT		g_ptSep[POLY_MAX * POLY_MAX + 4][2] =
{
	// �O�����͂ޕ��������ŏ��ɍ쐬
	{ { -SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 }, { -SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 } },
	{ {  SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 }, {  SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 } },
	{ { -SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 }, {  SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 } },
	{ { -SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 }, {  SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 } },
};

int			g_nSep = 4;	// �������̖{��(�����l�͊O�����͂ޕ�����4�{��)

//----- �v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void MainModule();
void DrawGrid();
void DrawPolygon();
void DrawPoint();
void DispInfo();
void CalcNormal();
int CheckFrontBack(FACE *face0, FACE *face1);
void MakeBSPtree(BSPnode *node, int *face_list, int face_num, int index);
void DeleteBSPtree(BSPnode *node);
void DispBSPtree(BSPnode *node, int indent, const TCHAR* prefix);
void DispFace();
void Init();
void ClearBSPtree(BSPnode* node);
BSPnode* SearchFace(BSPnode* node, int face);

//---------------------------------------------------------------------------------------
// ���C��
//---------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR args, int cmdShow)
{
	MSG		msg;

	// �ϐ�������
	g_bOnInfo = true;			// ���\���X�C�b�`���n�e�e
	g_BSPnode = NULL;

	// �E�B���h�E�N���X��o�^
	WNDCLASS wndClass = {
		0, WndProc, 0, 0, hInst,
		LoadIcon(hInst, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		0, NULL, szClassName
	};
	if (RegisterClass(&wndClass) == 0) return false;

	// �E�C���h�E���쐬
	g_hWndApp = CreateWindow(
		szClassName, szAppName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		SCREEN_WIDTH  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL, NULL, hInst, NULL);
	if (!g_hWndApp) return false;
	ShowWindow(g_hWndApp, cmdShow);
	UpdateWindow(g_hWndApp);

	// ���C�����[�v
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// ���b�Z�[�W����
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC				hdc;
	int				mx, my;
	PAINTSTRUCT		ps;

	switch (uMsg) {
	case WM_CREATE:
		hdc = GetDC(hWnd);
		g_hMemDC = CreateCompatibleDC(hdc);		// ����ʂ̍쐬
		g_hBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
		SelectObject(g_hMemDC, g_hBitmap);
		SetBkMode(g_hMemDC, TRANSPARENT);
		SetTextColor(g_hMemDC, RGB(0, 0, 0));
		ReleaseDC(hWnd, hdc);

		PenGrid0	= CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
		PenGrid1	= CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		PenBand		= CreatePen(PS_SOLID, 4, RGB(0, 0, 255));
		PenDot		= CreatePen(PS_SOLID, 4, RGB(255, 0, 192));
		PenCursor	= CreatePen(PS_SOLID, 4, RGB(0, 255, 255));
		PenSelect	= CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		PenNormal	= CreatePen(PS_SOLID, 4, RGB(192, 192, 192));
		PenSep		= CreatePen(PS_SOLID, 2, RGB(255, 0, 0));

		// �e�평���l�ݒ�
		Init();
		InvalidateRect(g_hWndApp, NULL, TRUE);
		return 0;
	case WM_KEYDOWN:			// �L�[�������ꂽ
		switch (wParam) {
		case VK_ESCAPE:	DestroyWindow(hWnd);	return 0;	// �I��
		case 'I':				// ���\���X�C�b�`�̐؂�ւ�
			g_bOnInfo = !g_bOnInfo;
			InvalidateRect(g_hWndApp, NULL, TRUE);
			return 0;

		case 'C':
			Init();
			InvalidateRect(g_hWndApp, NULL, TRUE);
			return 0;
		}
		break;

	case WM_MOUSEMOVE:			// �}�E�X��������
		mx =  (short)LOWORD(lParam) - AXIS_X_OFFSET;
		my = ((short)HIWORD(lParam) - AXIS_Y_OFFSET) * -1;
		for (int i = 0; i < POLY_MAX * POLY_MAX; i++) {
			if (g_FaceIndex[i] > -1) {
				int x0, x1, y0, y1;
				if(g_Face[g_FaceIndex[i]].p0.x > g_Face[g_FaceIndex[i]].p1.x)
				{
					x0 = g_Face[g_FaceIndex[i]].p1.x;
					x1 = g_Face[g_FaceIndex[i]].p0.x;
				}
				else
				{
					x0 = g_Face[g_FaceIndex[i]].p0.x;
					x1 = g_Face[g_FaceIndex[i]].p1.x;
				}
				if(g_Face[g_FaceIndex[i]].p0.y > g_Face[g_FaceIndex[i]].p1.y)
				{
					y0 = g_Face[g_FaceIndex[i]].p1.y;
					y1 = g_Face[g_FaceIndex[i]].p0.y;
				}
				else
				{
					y0 = g_Face[g_FaceIndex[i]].p0.y;
					y1 = g_Face[g_FaceIndex[i]].p1.y;
				}

				if (mx >= x0 - 4 && mx <= x1 + 4 && my >= y0 - 4 && my <= y1 + 4)
				{
					if(x0 != x1 && y0 != y1)
					{
						int cx, cy;
						cx = mx;
						cy = y0 + (y1 - y0) * (mx - x0) / (x1 - x0);
						if(abs(cx - mx) > 4 || abs(cy - my) > 4)
						{
							continue;
						}
					}

					if(g_nCursor != g_FaceIndex[i])
					{
						g_nCursor = g_FaceIndex[i];
						InvalidateRect(g_hWndApp, NULL, TRUE);
					}
					return 0;
				}
			}
		}
		if(g_nCursor >= 0)
		{
			g_nCursor = -1;
			InvalidateRect(g_hWndApp, NULL, TRUE);
		}
		return 0;

	case WM_LBUTTONDOWN:		// �}�E�X���{�^���������ꂽ
		if(g_nCursor >= 0)
		{
			g_Face[g_nCursor].select = true;
			if(!g_BSPnode)
			{
				g_BSPnode = new BSPnode;
				ZeroMemory(g_BSPnode, sizeof(BSPnode));

				for(int i = 0; i < POLY_MAX; i++)
				{
					if(g_FaceIndex[i] == g_nCursor)
					{
						MakeBSPtree(g_BSPnode, g_FaceIndex, POLY_MAX, i);
						break;
					}
				}
			}
			else
			{
				BSPnode* node = SearchFace(g_BSPnode, g_nCursor);
				if(node)
				{
					for(int i = 0; i < node->front_num; i++)
					{
						if(node->front_face[i] == g_nCursor)
						{
							node->front = new BSPnode;
							ZeroMemory(node->front, sizeof(BSPnode));
							MakeBSPtree(node->front, node->front_face, node->front_num, i);
							delete[] node->front_face;
							node->front_face = NULL;
							node->front_num = 0;
						}
					}
					for(int i = 0; i < node->back_num; i++)
					{
						if(node->back_face[i] == g_nCursor)
						{
							node->back = new BSPnode;
							ZeroMemory(node->back, sizeof(BSPnode));
							MakeBSPtree(node->back, node->back_face, node->back_num, i);
							delete[] node->back_face;
							node->back_face = NULL;
							node->back_num = 0;
						}
					}
				}
			}
			InvalidateRect(g_hWndApp, NULL, TRUE);
		}
		return 0;

	case WM_PAINT:				// �`��
		hdc = BeginPaint(hWnd, &ps);
		PatBlt(g_hMemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITENESS);
		DrawGrid();					// �O���b�h��`��
		DrawPolygon();				// ���p�`��`��
		if (g_bOnInfo) DispInfo();	// ���\��
		BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, g_hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if (g_BSPnode) DeleteBSPtree(g_BSPnode);	// �a�r�o�c���[���폜
		DeleteObject(g_hBitmap);
		DeleteDC(g_hMemDC);
		DeleteObject(PenGrid1);
		DeleteObject(PenGrid0);
		DeleteObject(PenBand);
		DeleteObject(PenDot);
		DeleteObject(PenCursor);
		DeleteObject(PenSelect);
		DeleteObject(PenNormal);
		DeleteObject(PenSep);
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//---------------------------------------------------------------------------------------
// �O���b�h�\�����W���[��
//---------------------------------------------------------------------------------------
void DrawGrid()
{
	// �g��`��
	int		x, y;
	SelectObject(g_hMemDC, PenGrid0);
	for (x = 0; x < SCREEN_WIDTH; x += 20) {
		MoveToEx(g_hMemDC, x, 0, NULL);
		LineTo(g_hMemDC, x, SCREEN_HEIGHT);
	}
	for (y = 0; y < SCREEN_HEIGHT; y += 20) {
		MoveToEx(g_hMemDC, 0, y, NULL);
		LineTo(g_hMemDC, SCREEN_WIDTH, y);
	}

	// ����킵�����ߎ��\���Ȃ�
/*
	SelectObject(g_hMemDC, PenGrid1);
	MoveToEx(g_hMemDC, 0, AXIS_Y_OFFSET, NULL);
	LineTo(g_hMemDC, SCREEN_WIDTH, AXIS_Y_OFFSET);
	MoveToEx(g_hMemDC, AXIS_X_OFFSET, 0, NULL);
	LineTo(g_hMemDC, AXIS_X_OFFSET, SCREEN_HEIGHT);
*/
}

//---------------------------------------------------------------------------------------
// ���p�`�`�惂�W���[��
//---------------------------------------------------------------------------------------
void DrawPolygon()
{
	TCHAR	str[256];
	int		px0, py0, px1, py1;
	int		nx0, ny0, nx1, ny1;
	int		len;

	HPEN oldpen   = (HPEN)SelectObject(g_hMemDC, PenSep);

	for(int i = 0; i < g_nSep; i++)
	{
		MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_ptSep[i][0].x, AXIS_Y_OFFSET - g_ptSep[i][0].y, NULL);
		LineTo(g_hMemDC, AXIS_X_OFFSET + g_ptSep[i][1].x, AXIS_Y_OFFSET - g_ptSep[i][1].y);
	}

	for (int i = 0; i < POLY_MAX * POLY_MAX; i++) {
		if (g_FaceIndex[i] > -1) {
			px0 = AXIS_X_OFFSET + g_Face[g_FaceIndex[i]].p0.x;
			py0 = AXIS_Y_OFFSET - g_Face[g_FaceIndex[i]].p0.y;
			px1 = AXIS_X_OFFSET + g_Face[g_FaceIndex[i]].p1.x;
			py1 = AXIS_Y_OFFSET - g_Face[g_FaceIndex[i]].p1.y;
			nx0 = (px0 + px1) / 2;
			ny0 = (py0 + py1) / 2;
			nx1 = nx0 + (int)(g_Face[g_FaceIndex[i]].nx * 20);
			ny1 = ny0 - (int)(g_Face[g_FaceIndex[i]].ny * 20);

			// �@����`��
			SelectObject(g_hMemDC, PenNormal);
			MoveToEx(g_hMemDC, nx0, ny0, NULL);
			LineTo(g_hMemDC, nx1, ny1);

			// �ӂ�`��
			if(g_Face[g_FaceIndex[i]].select)
			{
				SelectObject(g_hMemDC, PenSelect);
			}
			else if(g_nCursor == g_FaceIndex[i])
			{
				SelectObject(g_hMemDC, PenCursor);
			}
			else
			{
				SelectObject(g_hMemDC, PenBand);
			}
			MoveToEx(g_hMemDC, px0, py0, NULL);
			LineTo(g_hMemDC, px1, py1);

			// �ӂ̒[�_��`��
			SelectObject(g_hMemDC, PenDot);
			Ellipse(g_hMemDC, px0 - 3, py0 - 3, px0 + 3, py0 + 3);
			Ellipse(g_hMemDC, px1 - 3, py1 - 3, px1 + 3, py1 + 3);

			// �C���f�b�N�X��`��
			len = _stprintf(str, _T("%s"), g_Face[g_FaceIndex[i]].name);
			TextOut(g_hMemDC, (px1 + px0) / 2, (py1 + py0) / 2, str, len);
		}
	}

	SelectObject(g_hMemDC, oldpen);
}

//---------------------------------------------------------------------------------------
// ���\�����W���[��
//---------------------------------------------------------------------------------------
void DispInfo()
{
	TCHAR help[] = _T("C�L�[�ŏ�����");
	TextOut(g_hMemDC, 0, 0, help, _tcslen(help));

	g_DispTreeX = 0;
	g_DispTreeY = 120;
	DispBSPtree(g_BSPnode, 0, _T("�@"));			// �a�r�o�c���[��`��
}


// �O�ς�Z����
int cross_z(POINT* v1, POINT* v2)
{
	return v1->x * v2->y - v1->y * v2->x;
}

// �����ƒ����̌�������(�����������`�F�b�N�p)
bool CheckCross(POINT* sp0, POINT* sp1, POINT* lp0, POINT* lp1, POINT* pc)
{
	POINT v1, v2;
	v1.x = sp1->x - sp0->x;
	v1.y = sp1->y - sp0->y;
	v2.x = lp1->x - lp0->x;
	v2.y = lp1->y - lp0->y;

	int cross1, cross2;
	if((cross1 = cross_z(&v2, &v1)) == 0)
	{
		// ���s�Ȃ��ߌ������Ȃ�
		return false;
	}
	v1.x = lp0->x - sp0->x;
	v1.y = lp0->y - sp0->y;
	cross2 = cross_z(&v2, &v1);
	
	// ��_�������̊O���ɂ���ꍇ�͌������Ȃ��Ɣ���
	float t = (float)cross2 / cross1;
	if(t < 0 || t > 1) return false;

	// ��_���v�Z
	// t�����̂܂܎g���������A�덷���o�邽�ߌv�Z������
	pc->x = (long)(sp0->x + (float)(sp1->x - sp0->x) * cross2 / cross1);
	pc->y = (long)(sp0->y + (float)(sp1->y - sp0->y) * cross2 / cross1);
	return true;
}

// 2�_�Ԃ̋�����2��
int distance(POINT* p1, POINT* p2)
{
	return (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y);
}

//---------------------------------------------------------------------------------------
// �a�r�o�c���[�\�z
//
// in  : BSPnode *node �Ώۃm�[�h
//       int *face_list �m�[�h�Ɋ܂܂��ʔԍ��z��
//       int face_num �m�[�h�Ɋ܂܂��ʂ̐�
//		 int index �����ʂ̃C���f�b�N�X
//---------------------------------------------------------------------------------------
void MakeBSPtree(BSPnode *node, int *face_list, int face_num, int index)
{
	int		ans, ans2, ans3;
	float	t0, t1;
	int		px, py;
	POINT	p0, p1;
	FACE	*f0, *f1;
	bool	g_bFront;
	int		i;

	// �V�����ʃ��X�g���\�z���邽�߃��������m��
	node->front_face = new int[face_num];
	node->back_face  = new int[face_num];
	node->front_num  = 0;					// �\�ʃm�[�h�͂O��
	node->back_num   = 0;					// ���ʃm�[�h�͂O��
	for (i = 0; i < face_num; i++) {
		node->front_face[i] = -1;
		node->back_face[i]  = -1;
	}

	// �����ʂ�����i���X�g�̐擪�ɂ���ʁj
	node->face = face_list[index];

	// �������̏���(�\���p�̂��߁A���������ɂ͊֌W�Ȃ�)
	POINT pc;
	int d, d0 = -1, d1 = -1;
	for(i = 0; i < g_nSep; i++)
	{
		// ����������������ꍇ�͂����Ŏ~�߂�
		if(CheckCross(&g_ptSep[i][0], &g_ptSep[i][1], &g_Face[face_list[index]].p0, &g_Face[face_list[index]].p1, &pc))
		{
			// �ǂ̂ǂ���̒[���̌�_���A�x�N�g�����ς̕����Ń`�F�b�N
			POINT v1, v2;
			v1.x = g_Face[face_list[index]].p1.x - g_Face[face_list[index]].p0.x;
			v1.y = g_Face[face_list[index]].p1.y - g_Face[face_list[index]].p0.y;
			v2.x = pc.x - g_Face[face_list[index]].p0.x;
			v2.y = pc.y - g_Face[face_list[index]].p0.y;
			if(v1.x * v2.x + v1.y * v2.y <= 0)
			{
				// p0���̌�_
				if((d = distance(&g_Face[face_list[index]].p0, &pc)) < d0 || d0 < 0)
				{
					// �ł��߂���_�Ŏ~�߂�
					g_ptSep[g_nSep][0] = pc;
					d0 = d;
				}
			}
			else
			{
				// p1���̌�_
				if((d = distance(&g_Face[face_list[index]].p1, &pc)) < d1 || d1 < 0)
				{
					// �ł��߂���_�Ŏ~�߂�
					g_ptSep[g_nSep][1] = pc;
					d1 = d;
				}
			}
		}
	}
	g_nSep++;

	// ���X�g���ɂ���ʐ��������[�v
	for (i = 0; i < face_num; i++) {
		if(i == index) continue;

		// ��ʂƃ��X�g���̖ʂƂŕ\������
		ans = CheckFrontBack(&g_Face[face_list[i]], &g_Face[face_list[index]]);
		switch (ans) {
		case 0:			// �\�ʂɑ�����̂ŕ\�����X�g�ɒǉ�
			node->front_face[node->front_num++] = face_list[i];
			break;
		case 1:			// ���ʂɑ�����̂ŗ������X�g�ɒǉ�
			node->back_face[node->back_num++] = face_list[i];
			break;
			f0 = &g_Face[face_list[index]];
			f1 = &g_Face[face_list[i]];

			// ��_���Wpx,py�����߂�
			t0 = (f0->p0.x - f1->p0.x) * f0->nx +
				 (f0->p0.y - f1->p0.y) * f0->ny;
			t1 = (f1->p1.x - f1->p0.x) * f0->nx +
				 (f1->p1.y - f1->p0.y) * f0->ny;
			px = (int)(f1->p0.x + (f1->p1.x - f1->p0.x) * (t0/t1));
			py = (int)(f1->p0.y + (f1->p1.y - f1->p0.y) * (t0/t1));

			// p0��p1�̕\������p�̓��όv�Z�iCheckFrontBack�֐����Q�Ɓj
			ans2 = (int)((f1->p0.x - f0->p0.x) * f0->nx + 
						 (f1->p0.y - f0->p0.y) * f0->ny);
			ans3 = (int)((f1->p1.x - f0->p0.x) * f0->nx + 
						 (f1->p1.y - f0->p0.y) * f0->ny);

			g_bFront = false;
			for (int j = 0; j < POLY_MAX * POLY_MAX; j++) {
				// ���g�p�̖ʂ�
				if (g_FaceIndex[j] == -1) {
					// �܂��\�ʂ𐶐��ł��Ă��Ȃ���
					if (g_bFront == false) {
						// �󂫂��������̂ŐV�����\�ʂ��쐬���Ēǉ�
						g_FaceIndex[j] = j;
						// �������Ă�������ɂ���ĕ\���A�����ɂ�����W�����
						if (ans2 > 0 || ans3 < 0) {
							// p0 ���\���Ap1 ������
							p0.x = g_Face[face_list[i]].p0.x;
							p0.y = g_Face[face_list[i]].p0.y;
							p1.x = px;
							p1.y = py;
						} else {
							// p0 �������Ap1 ���\��
							p0.x = px;
							p0.y = py;
							p1.x = g_Face[face_list[i]].p1.x;
							p1.y = g_Face[face_list[i]].p1.y;
						}
						// ���������ʂ̍��W��ݒ�
						g_Face[j].p0.x = p0.x;
						g_Face[j].p0.y = p0.y;
						g_Face[j].p1.x = p1.x;
						g_Face[j].p1.y = p1.y;
						_tcscpy(g_Face[j].name, f1->name);
						_tcscat(g_Face[j].name, _T("1"));
						node->front_face[node->front_num++] = j;
						g_bFront = true;
					} else {
						// �󂫂��������̂ŐV�������ʂ��쐬���Ēǉ�
						g_FaceIndex[j] = j;
						// �������Ă�������ɂ���ĕ\���A�����ɂ�����W�����
						if(ans2 > 0 || ans3 < 0) {
							// p0 ���\���Ap1 ������
							p0.x = px;
							p0.y = py;
							p1.x = g_Face[face_list[i]].p1.x;
							p1.y = g_Face[face_list[i]].p1.y;
						} else {
							// p0 �������Ap1 ���\��
							p0.x = g_Face[face_list[i]].p0.x;
							p0.y = g_Face[face_list[i]].p0.y;
							p1.x = px;
							p1.y = py;
						}
						// ���������ʂ̍��W��ݒ�
						g_Face[j].p0.x = p0.x;
						g_Face[j].p0.y = p0.y;
						g_Face[j].p1.x = p1.x;
						g_Face[j].p1.y = p1.y;
						_tcscpy(g_Face[j].name, f1->name);
						_tcscat(g_Face[j].name, _T("2"));
						node->back_face[node->back_num++] = j;
						j = POLY_MAX * POLY_MAX;			// ���[�v�E�o
					}
				}
			}
			// ���̖ʂ͕������ꂽ�Ƃ����t���O��ݒ�
			g_FaceIndex[face_list[i]] = -2;
			break;
		}
	}

	CalcNormal();
}

//---------------------------------------------------------------------------------------
// �a�r�o�c���[�폜
//---------------------------------------------------------------------------------------
void DeleteBSPtree(BSPnode *node)
{
	if (node->front != NULL) DeleteBSPtree(node->front);	// �\���Ƀm�[�h������΂������č폜
	if (node->back  != NULL) DeleteBSPtree(node->back );	// �����Ƀm�[�h������΂������č폜
	// �ʃ��X�g�����������
	if (node->front_face) delete[] node->front_face;
	if (node->back_face ) delete[] node->back_face;
	// �m�[�h���g�����
	delete node;
}

//---------------------------------------------------------------------------------------
// �a�r�o�c���[�\��
//---------------------------------------------------------------------------------------
void DispBSPtree(BSPnode *node, int indent, const TCHAR* prefix)
{
	TCHAR str[256];

	if(node == NULL) return;

	if(node->back != NULL)
	{
		DispBSPtree(node->back, indent + 40, _T("�^"));
	}
	else
	{
		if(node->back_num > 0)
		{
			_tcscpy(str, _T("�^"));
			for(int i = 0; i < node->back_num; i++)
			{
				_tcscat(str, g_Face[node->back_face[i]].name);
				_tcscat(str, _T(" "));
			}
			TextOut(g_hMemDC, g_DispTreeX + indent + 40, g_DispTreeY, str, _tcslen(str));
			g_DispTreeY += 20;
		}
	}

	_tcscpy(str, prefix);
	_tcscat(str, g_Face[node->face].name);
	TextOut(g_hMemDC, g_DispTreeX + indent, g_DispTreeY, str, _tcslen(str));
	g_DispTreeY += 20;

	if(node->front != NULL)
	{
		DispBSPtree(node->front, indent + 40, _T("�_"));
	}
	else
	{
		if(node->front_num > 0)
		{
			_tcscpy(str, _T("�_"));
			for(int i = 0; i < node->front_num; i++)
			{
				_tcscat(str, g_Face[node->front_face[i]].name);
				_tcscat(str, _T(" "));
			}
			TextOut(g_hMemDC, g_DispTreeX + indent + 40, g_DispTreeY, str, _tcslen(str));
			g_DispTreeY += 20;
		}
	}
}

//---------------------------------------------------------------------------------------
// ������
//---------------------------------------------------------------------------------------
void Init()
{
	if(g_BSPnode)
	{
		DeleteBSPtree(g_BSPnode);
		g_BSPnode = NULL;
	}

	ZeroMemory(g_Face, sizeof(FACE) * POLY_MAX * POLY_MAX);
	for (int i = 0; i < POLY_MAX * POLY_MAX; i++)
	{
		g_FaceIndex[i] = -1;
	}
	for (int i = 0; i < POLY_MAX; i++) {
		g_Face[i].p0.x = poly_vertex[i  ].x;
		g_Face[i].p0.y = poly_vertex[i  ].y;
		g_Face[i].p1.x = poly_vertex[i+1].x;
		g_Face[i].p1.y = poly_vertex[i+1].y;
		g_Face[i].name[0] = 'A' + i;
		g_FaceIndex[i] = i;
	}

	CalcNormal();

	g_nSep = 4;
}

//---------------------------------------------------------------------------------------
// �����ޖʂ��������A�����Ă���m�[�h��Ԃ�
//---------------------------------------------------------------------------------------
BSPnode* SearchFace(BSPnode* node, int face)
{
	BSPnode* ret = NULL;
	if(node->front && (ret = SearchFace(node->front, face)))
	{
		return ret;
	}
	else if(node->back && (ret = SearchFace(node->back, face)))
	{
		return ret;
	}
	else
	{
		for(int i = 0; i < node->front_num; i++)
		{
			if(node->front_face[i] == face)
			{
				return node;
			}
		}
		for(int i = 0; i < node->back_num; i++)
		{
			if(node->back_face[i] == face)
			{
				return node;
			}
		}
	}
	return NULL;
}


//=======================================================================================
//---------------------------------------------------------------------------------------
// �@���v�Z���W���[��
//---------------------------------------------------------------------------------------
//=======================================================================================
void CalcNormal()
{
	// �x�N�g��v�F�ǂ̃x�N�g��
	int vx, vy, vz;
	Vector wall;;
	// �x�N�g��w�F���zZ���x�N�g��
	int wx, wy, wz;
	Vector vZ = {0,0,-1};
	// �x�N�g���̒���
	float mg;
	
	//���ׂĂ̖������̕ǂɑ΂��ď������s��
	for (int i = 0; i < POLY_MAX * POLY_MAX; i++) {
		if (g_FaceIndex[i] > -1) 
		{
			// �ǂ̃x�N�g���������v�փR�s�[����@Z��0�Ƃ��Ă���
			wall.x = g_Face[g_FaceIndex[i]].p1.x - g_Face[g_FaceIndex[i]].p0.x;
			wall.y = g_Face[g_FaceIndex[i]].p1.y - g_Face[g_FaceIndex[i]].p0.y;
			wall.z = 0;

			// w��Z���ɕ��s��Z���������̃x�N�g�������Ă����i0,0,-1�j
\
			// v��w�̊O�ς��v�Z�i�@�������̃x�N�g�����ł���j
			Vector normal = wall.Cross(vZ);
			//�@���𐳋K������
			normal.Normalize();

			g_Face[g_FaceIndex[i]].nx = normal.x;//float�փL���X�g���đ��
			g_Face[g_FaceIndex[i]].ny = normal.y;//�@�@�@�@//
			//.nz��0�Ȃ̂ŏȗ�

			// ��ō�����@���𐳋K������̂ŁA�܂����������߂�
			//mg = sqrtf(g_Face[g_FaceIndex[i]].nx * g_Face[g_FaceIndex[i]].nx +g_Face[g_FaceIndex[i]].ny * g_Face[g_FaceIndex[i]].ny);
			
			// �e�x�N�g���̗v�f�𒷂��Ŋ��遁���K��
			//[                          ];
			//[                          ];
			//.nz��0�Ȃ̂ŏȗ�
		}
	}
}

//=======================================================================================
//---------------------------------------------------------------------------------------
// �������m�̕\���ʒu���胂�W���[��
//
// in  : FACE *face0 ���肷��m�[�h
//       FACE *face1 ����̃x�[�X�ƂȂ�m�[�h�i���̃m�[�h�ɑ΂��ĕ\�������𔻒�j
// out : int 0 = face0 �� face1 �̕\��
//           1 = face0 �� face1 �̗���
//           2 = face0 �� face1 �ƌ���
//
//---------------------------------------------------------------------------------------
//=======================================================================================
int CheckFrontBack(FACE *face0, FACE *face1)
{
	int	ans0 = 0;
	int ans1 = 0;

	// face1��P0�_����face0��P0�ւ̃x�N�g�� v0 = face0->p0 - face1->p0
	int v0x = face0->p0.x - face1->p0.x;
	int v0y = face0->p0.y - face1->p0.y;

	Vector v0 = { (float)v0x,(float)v0y,0 };
	Vector n = { face1->nx,face1->ny,0 };
	// v0��face1�̖@���Ƃ̓���ans0 = v0�Eface1.n
	ans0 = (int)v0.Dot(n);

	// face1��P0�_����face0��P1�ւ̃x�N�g�� v1 = face0->p1 - face1->p0
	int v1x = face0->p1.x - face1->p0.x;
	int v1y = face0->p1.y - face1->p0.y;

	Vector v1 = { (float)v1x,(float)v1y,0 };
	// v1��face1�̖@���x�N�g���Ƃ̓��� ans1 = v1�Eface1->n
	ans1 = (int)v1.Dot(n);

	// ��L�������A���L���肪���藧��
	if ((ans0 * ans1) < 0) return 2;	// ans0��ans1�̕������قȂ�ꍇ�͕ǂ�����������͖���
	if (ans0 > 0 || ans1 > 0) return 0;	// �o�������̏ꍇ�͕\��
	else return 1;						// �o����0�ȉ��̏ꍇ�͗���
		//�i�o��0�̏ꍇ�͗��������S�ɏd�Ȃ��Ă��邱�Ƃ���������͂��̏ꍇ�������ƌ��Ȃ��j
}

/*
	�Ƃ肠�����T���v���̃}�b�v�ł��܂��ł�����
	�}�b�v�����ς��Ă݂悤�B
	�z�z�̃G�N�Z���Ńf�U�C�����Ă�����W������o���Ĕz��ɕ��ׂ�B
	�ǂ̓r���ŕǂ��������Ȃ��悤�ɕK���ǂ̈�ӂ��T�O�̐��ō��B

*/
//=======================================================================================
//	end of file
//=======================================================================================
