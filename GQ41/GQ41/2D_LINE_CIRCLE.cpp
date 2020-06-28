///*
//	�V�K�v���W�F�N�g�̍쐬�ɂ���
//
//	�P�@Windows�f�X�N�g�b�v�E�B�U�[�h�I��
//	�Q�@�A�v���P�[�V�����̎�ށ@�����@�f�X�N�g�b�v�A�v���P�[�V����
//	�@�@�@�@����̃v���W�F�N�g�Ƀ`�F�b�N
//
//	�R�@�ꏊ�▼�O�����߂č쐬
//	�S�@�ł����t�H���_�ɂ���CPP�t�@�C�����R�s�[���Ēǉ����Ă���g���B
//
//
//
//	bool HitCheck()�֐��̒��̏������v�����g�����Ƃɍ쐬���Ă݂悤�B
//
//
//*/
//
//
//
//
////=======================================================================================
////
////	  [ 2D_LINE_CIRCLE ] : �����Ɖ~�Ƃ̏Փ˔���
////
////=======================================================================================
//
////----- �C���N���[�h�t�@�C��
//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_NON_CONFORMING_SWPRINTFS
//#include <tchar.h>
//#include <windows.h>
//#include <stdio.h>
//
////----- �\����
//typedef struct {
//	int		pos_x0, pos_y0;	//�[�_�̍��W�O
//	int		pos_x1, pos_y1;	//�[�_�̍��W�P
//} LINE;
//
//typedef struct {
//	int		pos_x, pos_y;	//�~�̒��S���W
//	int		radius;			//�~�̔��a
//} CIRCLE;
//
////----- �萔��`
//#define SCREEN_WIDTH			800				// �X�N���[���@��
//#define SCREEN_HEIGHT			600				// �X�N���[���@����
//#define AXIS_X_OFFSET			SCREEN_WIDTH/2	// �w���W�I�t�Z�b�g
//#define AXIS_Y_OFFSET			SCREEN_HEIGHT/2	// �x���W�I�t�Z�b�g
//
////----- �O���[�o���ϐ�
//const TCHAR szClassName[] = _T("2DLineCircle");
//const TCHAR szAppName[] = _T("[ 2D_LINE_CIRCLE ] �����Ɖ~�Ƃ̏Փ˔���");
//
//LINE		g_Line;
//CIRCLE		g_Circle;
//bool		g_bOnHit;
//int			g_Vx, g_Vy;
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
//void DrawCircle();
//void DrawLine();
//void DrawHitCheckLine();
//void DispInfo();
//
////---------------------------------------------------------------------------------------
//// ���C��
////---------------------------------------------------------------------------------------
//int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR args, int cmdShow)
//{
//	MSG		msg;
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
//		SCREEN_WIDTH + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
//		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
//		NULL, NULL, hInst, NULL);
//	if (!g_hWndApp) return false;
//	ShowWindow(g_hWndApp, cmdShow);
//	UpdateWindow(g_hWndApp);
//
//	// �ϐ�������
//	g_bOnInfo = true;			// ���\���X�C�b�`���n�e�e
//
//	g_Line.pos_x0 = -300;
//	g_Line.pos_y0 = -200;
//	g_Line.pos_x1 = 100;
//	g_Line.pos_y1 = 200;
//	g_Circle.pos_x = 100;
//	g_Circle.pos_y = 0;
//	g_Circle.radius = 100;
//	g_bOnHit = HitCheck();		// �Փ˔���
//	InvalidateRect(g_hWndApp, NULL, TRUE);
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
//		if (MouseButtonFlg) {
//			g_Circle.pos_x = LOWORD(lParam) - AXIS_X_OFFSET;
//			g_Circle.pos_y = (HIWORD(lParam) - AXIS_Y_OFFSET) * -1;
//			InvalidateRect(g_hWndApp, NULL, true);
//			g_bOnHit = HitCheck();		// �Փ˔���
//			return 0;
//		}
//		break;
//	case WM_LBUTTONDOWN:		// �}�E�X���{�^���������ꂽ
//		g_Circle.pos_x = LOWORD(lParam) - AXIS_X_OFFSET;
//		g_Circle.pos_y = (HIWORD(lParam) - AXIS_Y_OFFSET) * -1;
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
//		DrawCircle();				// �~��`��
//		DrawLine();					// ������`��
//		DrawHitCheckLine();			// �Փ˔��胉�C����`��
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
//// �Փ˔���`�F�b�N���W���[���@�����聁true
////---------------------------------------------------------------------------------------
//bool HitCheck()
//{
//	int		dx, dy;
//	int		t0, t1;
//	float	t;
//	bool	ans;
//
//	ans = false;
//
//	// �����̒[�_0�Ɖ~�̒��S�܂ł̋����Ń`�F�b�N
//	dx = g_Circle.pos_x-g_Line.pos_x0;
//	dy = g_Circle.pos_y-g_Line.pos_y0;
//	if ((dx * dx + dy * dy) <= g_Circle.radius * g_Circle.radius) ans = true;//������
//
//	// �����̒[�_1�Ɖ~�̒��S�܂ł̋����Ń`�F�b�N
//	dx = g_Circle.pos_x - g_Line.pos_x1;
//	dy = g_Circle.pos_y - g_Line.pos_y1;
//	if ((dx * dx + dy * dy) <= g_Circle.radius * g_Circle.radius) ans = true;//������
//
//	//line Start pos x0,y0  
//	//line  Vector Vx Vy 
//	//line v P = v At+b;
//
//
//	// ��8�@t0/t1���v�Z�@�����Ɖ~�Ƃ̍ŒZ�����Ń`�F�b�N
//	t0 = (g_Circle.pos_x-g_Line.pos_x0)*(g_Line.pos_x1-g_Line.pos_x0)+
//		(g_Circle.pos_y - g_Line.pos_y0) * (g_Line.pos_y1 - g_Line.pos_y0);
//	t1 = (g_Line.pos_y0 - g_Line.pos_y1) * (g_Line.pos_y0 - g_Line.pos_y1)+ 
//		(g_Line.pos_x0 - g_Line.pos_x1) * (g_Line.pos_x0 - g_Line.pos_x1);
//
//	if (t1 != 0)	// ���̏ꍇ0�ɂ͂Ȃ�Ȃ����ǔO�̂���
//	{
//		t = (float)t0 / t1; //��8
//
//		if (0.0f <= t && t <= 1.0f) {// 0<= t <= 1 �Ȃ炠����̉\��
//			//��9 t�����܂�ƒ�����̌�_�ig_Vx, g_Vy�j������B
//			//�i��ʍ��W�Ȃ̂Ő����^�փL���X�g�j
//			g_Vx = (int)((g_Line.pos_x1 - g_Line.pos_x0) * t + g_Line.pos_x0);
//			g_Vy = (int)((g_Line.pos_y1 - g_Line.pos_y0) * t + g_Line.pos_y0);
//
//			//�ŒZ�����̌�_�Ɖ~�̒��S�Ƃ̋��������a��菬������΂�����
//			dx = g_Circle.pos_x - g_Vx;
//			dy = g_Circle.pos_y - g_Vy;
//
//			if ((dx * dx + dy * dy) <= g_Circle.radius * g_Circle.radius) ans = true;//������
//		}
//	}
//	return ans;
//}
//
//
////---------------------------------------------------------------------------------------
//// �O���b�h�\�����W���[��
////---------------------------------------------------------------------------------------
//void DrawGrid()
//{
//	HPEN	PenGrid0, PenGrid1;		// �O���b�h�̃y�� 
//	int		x, y;
//
//	PenGrid0 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
//	PenGrid1 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
//
//	// �g��`��
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
//// �T�[�N���`�惂�W���[��
////---------------------------------------------------------------------------------------
//void DrawCircle()
//{
//	LOGBRUSH	br;
//	HPEN		PenBand, oldpen;
//	HBRUSH		brush, oldbrush;
//
//	br.lbStyle = BS_SOLID;
//	br.lbColor = (g_bOnHit) ? RGB(255, 128, 128) : RGB(0, 255, 255);
//
//	PenBand = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
//	brush = CreateBrushIndirect(&br);
//	oldpen = (HPEN)SelectObject(g_hMemDC, PenBand);
//	oldbrush = (HBRUSH)SelectObject(g_hMemDC, brush);
//
//	Ellipse(g_hMemDC,
//		AXIS_X_OFFSET + g_Circle.pos_x - g_Circle.radius, AXIS_Y_OFFSET - g_Circle.pos_y - g_Circle.radius,
//		AXIS_X_OFFSET + g_Circle.pos_x + g_Circle.radius, AXIS_Y_OFFSET - g_Circle.pos_y + g_Circle.radius);
//
//	SelectObject(g_hMemDC, oldpen);
//	SelectObject(g_hMemDC, oldbrush);
//	DeleteObject(PenBand);
//	DeleteObject(brush);
//}
//
////---------------------------------------------------------------------------------------
//// ���C���`�惂�W���[��
////---------------------------------------------------------------------------------------
//void DrawLine()
//{
//	HPEN	PenBand;
//	PenBand = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
//	SelectObject(g_hMemDC, PenBand);
//
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Line.pos_x0, AXIS_Y_OFFSET - g_Line.pos_y0, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Line.pos_x1, AXIS_Y_OFFSET - g_Line.pos_y1);
//
//	DeleteObject(PenBand);
//}
//
////---------------------------------------------------------------------------------------
//// �Փ˔��胉�C���`�惂�W���[��
////---------------------------------------------------------------------------------------
//void DrawHitCheckLine()
//{
//	HPEN	PenBand;
//	PenBand = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
//	SelectObject(g_hMemDC, PenBand);
//
//	// �����̒[����~�̒��S��
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Line.pos_x0, AXIS_Y_OFFSET - g_Line.pos_y0, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Circle.pos_x, AXIS_Y_OFFSET - g_Circle.pos_y);
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Line.pos_x1, AXIS_Y_OFFSET - g_Line.pos_y1, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Circle.pos_x, AXIS_Y_OFFSET - g_Circle.pos_y);
//
//	// �����Ɖ~�Ƃ̍ŒZ����
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Vx, AXIS_Y_OFFSET - g_Vy, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Circle.pos_x, AXIS_Y_OFFSET - g_Circle.pos_y);
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
//	SelectObject(g_hMemDC, GetStockObject(SYSTEM_FIXED_FONT));
//
//	len = _stprintf(str, _T("LINE   (%4d,%4d)-(%4d,%4d)"),
//		g_Line.pos_x0, g_Line.pos_y0, g_Line.pos_x1, g_Line.pos_y1);
//	TextOut(g_hMemDC, 0, 0, str, len);
//	len = _stprintf(str, _T("CIRCLE (%4d,%4d) r = %4d"),
//		g_Circle.pos_x, g_Circle.pos_y, g_Circle.radius);
//	TextOut(g_hMemDC, 0, 20, str, len);
//	if (g_bOnHit) {
//		len = _stprintf(str, _T("HIT !!"));
//		TextOut(g_hMemDC, 0, 40, str, len);
//	}
//}
//
//
////=======================================================================================
////	end
////=======================================================================================