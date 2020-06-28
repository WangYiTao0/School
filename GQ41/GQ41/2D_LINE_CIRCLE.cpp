///*
//	新規プロジェクトの作成について
//
//	１　Windowsデスクトップウィザード選択
//	２　アプリケーションの種類　＞＞　デスクトップアプリケーション
//	　　　　＆空のプロジェクトにチェック
//
//	３　場所や名前を決めて作成
//	４　できたフォルダにこのCPPファイルをコピーして追加してから使う。
//
//
//
//	bool HitCheck()関数の中の処理をプリントをもとに作成してみよう。
//
//
//*/
//
//
//
//
////=======================================================================================
////
////	  [ 2D_LINE_CIRCLE ] : 線分と円との衝突判定
////
////=======================================================================================
//
////----- インクルードファイル
//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_NON_CONFORMING_SWPRINTFS
//#include <tchar.h>
//#include <windows.h>
//#include <stdio.h>
//
////----- 構造体
//typedef struct {
//	int		pos_x0, pos_y0;	//端点の座標０
//	int		pos_x1, pos_y1;	//端点の座標１
//} LINE;
//
//typedef struct {
//	int		pos_x, pos_y;	//円の中心座標
//	int		radius;			//円の半径
//} CIRCLE;
//
////----- 定数定義
//#define SCREEN_WIDTH			800				// スクリーン　幅
//#define SCREEN_HEIGHT			600				// スクリーン　高さ
//#define AXIS_X_OFFSET			SCREEN_WIDTH/2	// Ｘ座標オフセット
//#define AXIS_Y_OFFSET			SCREEN_HEIGHT/2	// Ｙ座標オフセット
//
////----- グローバル変数
//const TCHAR szClassName[] = _T("2DLineCircle");
//const TCHAR szAppName[] = _T("[ 2D_LINE_CIRCLE ] 線分と円との衝突判定");
//
//LINE		g_Line;
//CIRCLE		g_Circle;
//bool		g_bOnHit;
//int			g_Vx, g_Vy;
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
//void DrawCircle();
//void DrawLine();
//void DrawHitCheckLine();
//void DispInfo();
//
////---------------------------------------------------------------------------------------
//// メイン
////---------------------------------------------------------------------------------------
//int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR args, int cmdShow)
//{
//	MSG		msg;
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
//		SCREEN_WIDTH + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
//		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
//		NULL, NULL, hInst, NULL);
//	if (!g_hWndApp) return false;
//	ShowWindow(g_hWndApp, cmdShow);
//	UpdateWindow(g_hWndApp);
//
//	// 変数初期化
//	g_bOnInfo = true;			// 情報表示スイッチをＯＦＦ
//
//	g_Line.pos_x0 = -300;
//	g_Line.pos_y0 = -200;
//	g_Line.pos_x1 = 100;
//	g_Line.pos_y1 = 200;
//	g_Circle.pos_x = 100;
//	g_Circle.pos_y = 0;
//	g_Circle.radius = 100;
//	g_bOnHit = HitCheck();		// 衝突判定
//	InvalidateRect(g_hWndApp, NULL, TRUE);
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
//		if (MouseButtonFlg) {
//			g_Circle.pos_x = LOWORD(lParam) - AXIS_X_OFFSET;
//			g_Circle.pos_y = (HIWORD(lParam) - AXIS_Y_OFFSET) * -1;
//			InvalidateRect(g_hWndApp, NULL, true);
//			g_bOnHit = HitCheck();		// 衝突判定
//			return 0;
//		}
//		break;
//	case WM_LBUTTONDOWN:		// マウス左ボタンが押された
//		g_Circle.pos_x = LOWORD(lParam) - AXIS_X_OFFSET;
//		g_Circle.pos_y = (HIWORD(lParam) - AXIS_Y_OFFSET) * -1;
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
//		DrawCircle();				// 円を描画
//		DrawLine();					// 直線を描画
//		DrawHitCheckLine();			// 衝突判定ラインを描画
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
//// 衝突判定チェックモジュール　あたり＝true
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
//	// 線分の端点0と円の中心までの距離でチェック
//	dx = g_Circle.pos_x-g_Line.pos_x0;
//	dy = g_Circle.pos_y-g_Line.pos_y0;
//	if ((dx * dx + dy * dy) <= g_Circle.radius * g_Circle.radius) ans = true;//あたり
//
//	// 線分の端点1と円の中心までの距離でチェック
//	dx = g_Circle.pos_x - g_Line.pos_x1;
//	dy = g_Circle.pos_y - g_Line.pos_y1;
//	if ((dx * dx + dy * dy) <= g_Circle.radius * g_Circle.radius) ans = true;//あたり
//
//	//line Start pos x0,y0  
//	//line  Vector Vx Vy 
//	//line v P = v At+b;
//
//
//	// ※8　t0/t1を計算　線分と円との最短距離でチェック
//	t0 = (g_Circle.pos_x-g_Line.pos_x0)*(g_Line.pos_x1-g_Line.pos_x0)+
//		(g_Circle.pos_y - g_Line.pos_y0) * (g_Line.pos_y1 - g_Line.pos_y0);
//	t1 = (g_Line.pos_y0 - g_Line.pos_y1) * (g_Line.pos_y0 - g_Line.pos_y1)+ 
//		(g_Line.pos_x0 - g_Line.pos_x1) * (g_Line.pos_x0 - g_Line.pos_x1);
//
//	if (t1 != 0)	// この場合0にはならないけど念のため
//	{
//		t = (float)t0 / t1; //※8
//
//		if (0.0f <= t && t <= 1.0f) {// 0<= t <= 1 ならあたりの可能性
//			//※9 tが決まると直線上の交点（g_Vx, g_Vy）を作れる。
//			//（画面座標なので整数型へキャスト）
//			g_Vx = (int)((g_Line.pos_x1 - g_Line.pos_x0) * t + g_Line.pos_x0);
//			g_Vy = (int)((g_Line.pos_y1 - g_Line.pos_y0) * t + g_Line.pos_y0);
//
//			//最短距離の交点と円の中心との距離が半径より小さければあたり
//			dx = g_Circle.pos_x - g_Vx;
//			dy = g_Circle.pos_y - g_Vy;
//
//			if ((dx * dx + dy * dy) <= g_Circle.radius * g_Circle.radius) ans = true;//あたり
//		}
//	}
//	return ans;
//}
//
//
////---------------------------------------------------------------------------------------
//// グリッド表示モジュール
////---------------------------------------------------------------------------------------
//void DrawGrid()
//{
//	HPEN	PenGrid0, PenGrid1;		// グリッドのペン 
//	int		x, y;
//
//	PenGrid0 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
//	PenGrid1 = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
//
//	// 枠を描画
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
//// サークル描画モジュール
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
//// ライン描画モジュール
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
//// 衝突判定ライン描画モジュール
////---------------------------------------------------------------------------------------
//void DrawHitCheckLine()
//{
//	HPEN	PenBand;
//	PenBand = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
//	SelectObject(g_hMemDC, PenBand);
//
//	// 線分の端から円の中心へ
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Line.pos_x0, AXIS_Y_OFFSET - g_Line.pos_y0, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Circle.pos_x, AXIS_Y_OFFSET - g_Circle.pos_y);
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Line.pos_x1, AXIS_Y_OFFSET - g_Line.pos_y1, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Circle.pos_x, AXIS_Y_OFFSET - g_Circle.pos_y);
//
//	// 線分と円との最短距離
//	MoveToEx(g_hMemDC, AXIS_X_OFFSET + g_Vx, AXIS_Y_OFFSET - g_Vy, NULL);
//	LineTo(g_hMemDC, AXIS_X_OFFSET + g_Circle.pos_x, AXIS_Y_OFFSET - g_Circle.pos_y);
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