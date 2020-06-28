////=======================================================================================
////
////	  [ 2D_CONVEXITY ] : �O�p�`�Ɠ_�Ƃ̏Փ˔���
////
////=======================================================================================
//
////----- �C���N���[�h�t�@�C��
//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_NON_CONFORMING_SWPRINTFS
//#include <tchar.h>
//#include <windows.h>
//#include <stdio.h>
//#include "Collision.h"
//
////----- �萔��`
//#define SCREEN_WIDTH			800				// �X�N���[���@��
//#define SCREEN_HEIGHT			600				// �X�N���[���@����
//#define AXIS_X_OFFSET			SCREEN_WIDTH/2	// �w���W�I�t�Z�b�g
//#define AXIS_Y_OFFSET			SCREEN_HEIGHT/2	// �x���W�I�t�Z�b�g
//
//POINT	poly_vertex[] = {
//	{ 100, 200},	//V0
//	{-200,-200},	//V1
//	{ 200,-200},	//V2
//
//	{ 100, 200},
//};
//#define POLY_MAX 3	//���_��
//
////----- �O���[�o���ϐ�
//const TCHAR szClassName[] = _T("2DConvexity");
////const TCHAR szAppName[]   = _T("[ 2D_CONVEXITY ] �O�p�`�Ɠ_�Ƃ̏Փ˔���");
//const TCHAR szAppName[] = _T("AT14C184_06_70732_�I�E�C�b�g�E");
//
//
//POINT		g_Point;
//int			g_PointMoveFlg;
//int			g_HitCount;
//bool		g_bOnHit;					// �q�b�g�m�F�t���O
//int			g_Vx, g_Vy;
//int			g_PolyHit[POLY_MAX];
//
//HWND		g_hWndApp;					// �E�B���h�E�n���h��
//int			g_iClientWidth;				// �N���C�A���g�̈�T�C�Y
//int			g_iClientHeight;			// �N���C�A���g�̈�T�C�Y
//RECT		g_ClientRect;				// �N���C�A���g�̈�
//HDC			g_hMemDC;					// ����ʂc�b
//HBITMAP		g_hBitmap;					// ����ʃr�b�g�}�b�v
//bool		g_bOnInfo;					// �x�N�g���E���l���\���X�C�b�`
//
////----- �v���g�^�C�v�錾
//LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//void MainModule();
//bool HitCheck();
//void DrawGrid();
//void DrawPolygon();
//void DrawPoint();
//void DispInfo();
//
//
////---------------------------------------------------------------------------------------
//// ���C��
////---------------------------------------------------------------------------------------
//int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR args, int cmdShow)
//{
//	MSG			msg;
//
//	// �E�B���h�E�N���X��o�^
//	WNDCLASS wndClass = {
//		0, WndProc, 0, 0, hInst,
//		LoadIcon(hInst, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
//		0, NULL, szClassName
//	};
//	if (RegisterClass(&wndClass) == 0) return false;
//
//	// �E�C���h�E���쐬
//	g_hWndApp = CreateWindow(
//		szClassName, szAppName,
//		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
//		SCREEN_WIDTH  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
//		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
//		NULL, NULL, hInst, NULL);
//	if (!g_hWndApp) return false;
//	ShowWindow(g_hWndApp, cmdShow);
//	UpdateWindow(g_hWndApp);
//
//	// �ϐ�������
//	g_bOnInfo = true;			// ���\���X�C�b�`���n�e�e
//	g_bOnHit  = false;			// �q�b�g���Ă��Ȃ�
//	g_PointMoveFlg = -1;
//
//	g_Point.x = 300;
//	g_Point.y = 200;
//	for (int i = 0; i < POLY_MAX; i++) g_PolyHit[i] = 0;
//	g_HitCount = HitCheck();		// �Փ˔���
//	InvalidateRect(g_hWndApp, NULL, true);
//
//	// ���C�����[�v
//	while (GetMessage(&msg, 0, 0, 0)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	return 0;
//}
//
////-----------------------------------------------------------------------------
//// ���b�Z�[�W����
////-----------------------------------------------------------------------------
//LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	HDC				hdc;
//	int				i, mx, my;
//	PAINTSTRUCT		ps;
//	static bool		MouseButtonFlg;
//
//	switch (uMsg) {
//	case WM_CREATE:
//		hdc = GetDC(hWnd);
//		g_hMemDC = CreateCompatibleDC(hdc);		// ����ʂ̍쐬
//		g_hBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
//		SelectObject(g_hMemDC, g_hBitmap);
//		SetBkMode(g_hMemDC, TRANSPARENT);
//		SetTextColor(g_hMemDC, RGB(0, 0, 0));
//		ReleaseDC(hWnd, hdc);
//		MouseButtonFlg = false;
//		return 0;
//	case WM_KEYDOWN:			// �L�[�������ꂽ
//		switch (wParam) {
//		case VK_ESCAPE:	DestroyWindow(hWnd);	return 0;	// �I��
//		case 'I':		g_bOnInfo = !g_bOnInfo;	return 0;	// ���\���X�C�b�`�̐؂�ւ�
//		}
//		break;
//	case WM_MOUSEMOVE:			// �}�E�X��������
//		if (MouseButtonFlg == true) {
//			mx =  LOWORD(lParam) - AXIS_X_OFFSET;
//			my = (HIWORD(lParam) - AXIS_Y_OFFSET) * -1;		
//			if (g_PointMoveFlg != -1) {
//				poly_vertex[g_PointMoveFlg].x = mx;
//				poly_vertex[g_PointMoveFlg].y = my;
//				if (g_PointMoveFlg == 0) {
//					poly_vertex[POLY_MAX].x = mx;	// �擪�̃|�C���g�Ȃ�I�[�����낦��K�v����
//					poly_vertex[POLY_MAX].y = my;
//				}
//			} else {
//				g_Point.x = mx;
//				g_Point.y = my;
//			}
//			InvalidateRect(g_hWndApp, NULL, TRUE);
//			g_bOnHit = HitCheck();		// �Փ˔���
//			return 0;
//		}
//		break;
//	case WM_LBUTTONDOWN:		// �}�E�X���{�^���������ꂽ
//		mx =  (short)LOWORD(lParam) - AXIS_X_OFFSET;
//		my = ((short)HIWORD(lParam) - AXIS_Y_OFFSET) * -1;		
//		g_PointMoveFlg = -1;
//		for (i = 0; i < POLY_MAX; i++) {
//			if (poly_vertex[i].x - 4 < mx && poly_vertex[i].x + 4 > mx && 
//				poly_vertex[i].y - 4 < my && poly_vertex[i].y + 4 > my) {
//				g_PointMoveFlg = i;
//			}
//		}
//		if (g_PointMoveFlg == -1) {
//			g_Point.x = mx;
//			g_Point.y = my;
//		}
//		InvalidateRect(g_hWndApp, NULL, TRUE);
//		g_bOnHit = HitCheck();		// �Փ˔���
//		MouseButtonFlg = true;
//		break;
//	case WM_LBUTTONUP:			// �}�E�X���{�^���������ꂽ
//		MouseButtonFlg = false;
//		break;
//	case WM_PAINT:				// �`��
//		hdc = BeginPaint(hWnd, &ps);
//		PatBlt(g_hMemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITENESS);
//		DrawGrid();					// �O���b�h��`��
//		DrawPolygon();				// ���p�`��`��
//		DrawPoint();				// ������`��
//		if (g_bOnInfo) DispInfo();	// ���\��
//		BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, g_hMemDC, 0, 0, SRCCOPY);
//		EndPaint(hWnd, &ps);
//		return 0;
//	case WM_DESTROY:
//		DeleteObject(g_hBitmap);
//		DeleteDC(g_hMemDC);
//		PostQuitMessage(0);
//		return 0;
//	default:
//		break;
//	}
//	return DefWindowProc(hWnd, uMsg, wParam, lParam);
//}
//
////---------------------------------------------------------------------------------------
//// �Փ˔���`�F�b�N���W���[��
//// in  : ---
//// out : bool TRUE = �q�b�g���� , FALSE = �q�b�g���Ȃ�����
////---------------------------------------------------------------------------------------
//
//
//
//bool HitCheck()		//�S�p�X�y�[�X�ɂ��G���[�ɒ��ӁI
//{
//	POINT vec1;			// ���_���玟�̒��_�ւ̃x�N�g��
//	POINT vec2;			// ���̒��_���炻�̎��̒��_�ւ̃x�N�g��
//	float cross_z[POLY_MAX];	// vec1��vec2�̊O�ς�Z������ۑ����Ƃ�(���_���ƂɊi�[)
//
//	Vector A = { (float)poly_vertex[0].x,(float)poly_vertex[0].y,0 };
//	Vector B = { (float)poly_vertex[1].x,(float)poly_vertex[1].y,0 };
//	Vector C = { (float)poly_vertex[2].x,(float)poly_vertex[2].y,0 };
//	Vector P = { (float)g_Point.x,(float)g_Point.y,0 };
//
//	return PointTrangleIntersection(A, B, C, P);
//
//	// ���_���玟�̒��_�ւ̃x�N�g���@�@vec1
//
////	for(int i = 0; i < POLY_MAX; i++)
////	{
////		
////		// ���̒��_���炻�̎��̒��_�ւ̃x�N�g��  vec2
////
////		// vec1 X vec2�̊O�σx�N�g����Z������ۑ�
////		// �O�ς̓l�b�g�Ōv�Z���𒲂ׂ�OK
////		//�@�e���_��Z���W��0�Ōv�Z
////	}
////
////	// ��ŕۑ����ꂽ�O�ς�Z�������S�ē������Ȃ�A�_�͑��p�`�̓����ɂ���
////
////	
//////	return true;	//�@������
////	return false;	//�@�͂���@
//
//}
//
//
////---------------------------------------------------------------------------------------
//// �O���b�h�\�����W���[��
////---------------------------------------------------------------------------------------
//void DrawGrid()
//{
//	HPEN	PenGrid0, PenGrid1;		// �O���b�h�̃y�� 
//	PenGrid0 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
//	PenGrid1 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
//
//	// �g��`��
//	int		x, y;
//	SelectObject(g_hMemDC, PenGrid0);
//	for (x = 0; x < SCREEN_WIDTH; x += 20) {
//		MoveToEx(g_hMemDC, x, 0, NULL);
//		LineTo(g_hMemDC, x, SCREEN_HEIGHT);
//	}
//	for (y = 0; y < SCREEN_HEIGHT; y += 20) {
//		MoveToEx(g_hMemDC, 0, y, NULL);
//		LineTo(g_hMemDC, SCREEN_WIDTH, y);
//	}
//	SelectObject(g_hMemDC, PenGrid1);
//	MoveToEx(g_hMemDC, 0, AXIS_Y_OFFSET, NULL);
//	LineTo(g_hMemDC, SCREEN_WIDTH, AXIS_Y_OFFSET);
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET, 0, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET, SCREEN_HEIGHT);
//
//	DeleteObject(PenGrid1);
//	DeleteObject(PenGrid0);
//}
//
////---------------------------------------------------------------------------------------
//// ���p�`�`�惂�W���[��
////---------------------------------------------------------------------------------------
//void DrawPolygon()
//{
//	TCHAR		 str[256];
//	LOGBRUSH	br;
//	HPEN		PenBand, oldpen;
//	HBRUSH		brush, oldbrush;
//	int			i, len;
//
//	br.lbStyle = BS_SOLID;
//	br.lbColor = (g_bOnHit)? RGB(255, 128, 128): RGB(0, 255, 255);
//
//	PenBand = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
//	brush = CreateBrushIndirect(&br);
//	oldpen   = (HPEN)SelectObject(g_hMemDC, PenBand);
//	oldbrush = (HBRUSH)SelectObject(g_hMemDC, brush);
//
//	POINT poly[POLY_MAX+1];
//	for (i = 0; i <= POLY_MAX; i++) {
//		poly[i].x = AXIS_X_OFFSET + poly_vertex[i].x;
//		poly[i].y = AXIS_Y_OFFSET - poly_vertex[i].y;
//	}
//	Polygon(g_hMemDC, &poly[0], POLY_MAX);
//
//	for (i = 0; i < POLY_MAX; i++) {
//		len = _stprintf(str, _T("%2d"), i);
//		TextOut(g_hMemDC, (poly[i+1].x + poly[i].x) / 2, (poly[i+1].y + poly[i].y) / 2, str, len);
//	}
//
//	SelectObject(g_hMemDC, oldpen  );
//	SelectObject(g_hMemDC, oldbrush);
//	DeleteObject(PenBand);
//	DeleteObject(brush);
//}
//
////---------------------------------------------------------------------------------------
//// �|�C���g�`�惂�W���[��
////---------------------------------------------------------------------------------------
//void DrawPoint()
//{
//	HPEN	PenBand;
//	PenBand = CreatePen(PS_SOLID, 5, RGB(255, 0, 255));
//	SelectObject(g_hMemDC, PenBand);
//
//	Rectangle(g_hMemDC, AXIS_X_OFFSET + g_Point.x - 3, AXIS_Y_OFFSET - g_Point.y - 3,
//						AXIS_X_OFFSET + g_Point.x + 3, AXIS_Y_OFFSET - g_Point.y + 3);
//
//	DeleteObject(PenBand);
//}
//
////---------------------------------------------------------------------------------------
//// ���\�����W���[��
////---------------------------------------------------------------------------------------
//void DispInfo()
//{
//	TCHAR	str[256];
//	int		len;
//
//	len = _stprintf(str, _T("POINT   (%4d,%4d)"), g_Point.x, g_Point.y);
//	TextOut(g_hMemDC, 0, 0, str, len);
//
//	if (g_bOnHit)
//	{
//		len = _stprintf(str, _T("HIT !!"));
//		TextOut(g_hMemDC, 0, 40, str, len);
//	}
//}
//
//
////=======================================================================================
////	end
////=======================================================================================