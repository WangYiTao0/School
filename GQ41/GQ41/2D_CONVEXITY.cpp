////=======================================================================================
////
////	  [ 2D_CONVEXITY ] : 三角形と点との衝突判定
////
////=======================================================================================
//
////----- インクルードファイル
//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_NON_CONFORMING_SWPRINTFS
//#include <tchar.h>
//#include <windows.h>
//#include <stdio.h>
//#include "Collision.h"
//
////----- 定数定義
//#define SCREEN_WIDTH			800				// スクリーン　幅
//#define SCREEN_HEIGHT			600				// スクリーン　高さ
//#define AXIS_X_OFFSET			SCREEN_WIDTH/2	// Ｘ座標オフセット
//#define AXIS_Y_OFFSET			SCREEN_HEIGHT/2	// Ｙ座標オフセット
//
//POINT	poly_vertex[] = {
//	{ 100, 200},	//V0
//	{-200,-200},	//V1
//	{ 200,-200},	//V2
//
//	{ 100, 200},
//};
//#define POLY_MAX 3	//頂点数
//
////----- グローバル変数
//const TCHAR szClassName[] = _T("2DConvexity");
////const TCHAR szAppName[]   = _T("[ 2D_CONVEXITY ] 三角形と点との衝突判定");
//const TCHAR szAppName[] = _T("AT14C184_06_70732_オウイットウ");
//
//
//POINT		g_Point;
//int			g_PointMoveFlg;
//int			g_HitCount;
//bool		g_bOnHit;					// ヒット確認フラグ
//int			g_Vx, g_Vy;
//int			g_PolyHit[POLY_MAX];
//
//HWND		g_hWndApp;					// ウィンドウハンドル
//int			g_iClientWidth;				// クライアント領域サイズ
//int			g_iClientHeight;			// クライアント領域サイズ
//RECT		g_ClientRect;				// クライアント領域
//HDC			g_hMemDC;					// 裏画面ＤＣ
//HBITMAP		g_hBitmap;					// 裏画面ビットマップ
//bool		g_bOnInfo;					// ベクトル・数値情報表示スイッチ
//
////----- プロトタイプ宣言
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
//// メイン
////---------------------------------------------------------------------------------------
//int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR args, int cmdShow)
//{
//	MSG			msg;
//
//	// ウィンドウクラスを登録
//	WNDCLASS wndClass = {
//		0, WndProc, 0, 0, hInst,
//		LoadIcon(hInst, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
//		0, NULL, szClassName
//	};
//	if (RegisterClass(&wndClass) == 0) return false;
//
//	// ウインドウを作成
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
//	// 変数初期化
//	g_bOnInfo = true;			// 情報表示スイッチをＯＦＦ
//	g_bOnHit  = false;			// ヒットしていない
//	g_PointMoveFlg = -1;
//
//	g_Point.x = 300;
//	g_Point.y = 200;
//	for (int i = 0; i < POLY_MAX; i++) g_PolyHit[i] = 0;
//	g_HitCount = HitCheck();		// 衝突判定
//	InvalidateRect(g_hWndApp, NULL, true);
//
//	// メインループ
//	while (GetMessage(&msg, 0, 0, 0)) {
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//	return 0;
//}
//
////-----------------------------------------------------------------------------
//// メッセージ処理
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
//		g_hMemDC = CreateCompatibleDC(hdc);		// 裏画面の作成
//		g_hBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
//		SelectObject(g_hMemDC, g_hBitmap);
//		SetBkMode(g_hMemDC, TRANSPARENT);
//		SetTextColor(g_hMemDC, RGB(0, 0, 0));
//		ReleaseDC(hWnd, hdc);
//		MouseButtonFlg = false;
//		return 0;
//	case WM_KEYDOWN:			// キーが押された
//		switch (wParam) {
//		case VK_ESCAPE:	DestroyWindow(hWnd);	return 0;	// 終了
//		case 'I':		g_bOnInfo = !g_bOnInfo;	return 0;	// 情報表示スイッチの切り替え
//		}
//		break;
//	case WM_MOUSEMOVE:			// マウスが動いた
//		if (MouseButtonFlg == true) {
//			mx =  LOWORD(lParam) - AXIS_X_OFFSET;
//			my = (HIWORD(lParam) - AXIS_Y_OFFSET) * -1;		
//			if (g_PointMoveFlg != -1) {
//				poly_vertex[g_PointMoveFlg].x = mx;
//				poly_vertex[g_PointMoveFlg].y = my;
//				if (g_PointMoveFlg == 0) {
//					poly_vertex[POLY_MAX].x = mx;	// 先頭のポイントなら終端もそろえる必要あり
//					poly_vertex[POLY_MAX].y = my;
//				}
//			} else {
//				g_Point.x = mx;
//				g_Point.y = my;
//			}
//			InvalidateRect(g_hWndApp, NULL, TRUE);
//			g_bOnHit = HitCheck();		// 衝突判定
//			return 0;
//		}
//		break;
//	case WM_LBUTTONDOWN:		// マウス左ボタンが押された
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
//		g_bOnHit = HitCheck();		// 衝突判定
//		MouseButtonFlg = true;
//		break;
//	case WM_LBUTTONUP:			// マウス左ボタンが離された
//		MouseButtonFlg = false;
//		break;
//	case WM_PAINT:				// 描画
//		hdc = BeginPaint(hWnd, &ps);
//		PatBlt(g_hMemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITENESS);
//		DrawGrid();					// グリッドを描画
//		DrawPolygon();				// 多角形を描画
//		DrawPoint();				// 直線を描画
//		if (g_bOnInfo) DispInfo();	// 情報表示
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
//// 衝突判定チェックモジュール
//// in  : ---
//// out : bool TRUE = ヒットした , FALSE = ヒットしなかった
////---------------------------------------------------------------------------------------
//
//
//
//bool HitCheck()		//全角スペースによるエラーに注意！
//{
//	POINT vec1;			// 頂点から次の頂点へのベクトル
//	POINT vec2;			// 次の頂点からその次の頂点へのベクトル
//	float cross_z[POLY_MAX];	// vec1とvec2の外積のZ成分を保存しとく(頂点ごとに格納)
//
//	Vector A = { (float)poly_vertex[0].x,(float)poly_vertex[0].y,0 };
//	Vector B = { (float)poly_vertex[1].x,(float)poly_vertex[1].y,0 };
//	Vector C = { (float)poly_vertex[2].x,(float)poly_vertex[2].y,0 };
//	Vector P = { (float)g_Point.x,(float)g_Point.y,0 };
//
//	return PointTrangleIntersection(A, B, C, P);
//
//	// 頂点から次の頂点へのベクトル　　vec1
//
////	for(int i = 0; i < POLY_MAX; i++)
////	{
////		
////		// 次の頂点からその次の頂点へのベクトル  vec2
////
////		// vec1 X vec2の外積ベクトルのZ成分を保存
////		// 外積はネットで計算式を調べてOK
////		//　各頂点のZ座標は0で計算
////	}
////
////	// 上で保存された外積のZ成分が全て同符号なら、点は多角形の内部にある
////
////	
//////	return true;	//　あたり
////	return false;	//　はずれ　
//
//}
//
//
////---------------------------------------------------------------------------------------
//// グリッド表示モジュール
////---------------------------------------------------------------------------------------
//void DrawGrid()
//{
//	HPEN	PenGrid0, PenGrid1;		// グリッドのペン 
//	PenGrid0 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
//	PenGrid1 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
//
//	// 枠を描画
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
//// 多角形描画モジュール
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
//// ポイント描画モジュール
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
//// 情報表示モジュール
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