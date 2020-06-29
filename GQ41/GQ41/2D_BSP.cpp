//=======================================================================================
//
//	  [ 2D_BSP ] : Binary Space Partitioning
//
//=======================================================================================
#define  _CRT_SECURE_NO_WARNINGS
//----- インクルードファイル
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>
#include "Vector.h"

//----- 定数定義
#define SCREEN_WIDTH			800				// スクリーン　幅
#define SCREEN_HEIGHT			600				// スクリーン　高さ
#define AXIS_X_OFFSET			SCREEN_WIDTH/2	// Ｘ座標オフセット
#define AXIS_Y_OFFSET			SCREEN_HEIGHT/2	// Ｙ座標オフセット

#if 1

//注意
//壁の途中で分断されないようにすべて一辺が50の壁で構成する。
POINT	poly_vertex[] =
{			// 頂点座標
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

	{-150	,	-50},//スタートの頂点へ戻すためのデータを必ず最後につける

};
#define POLY_MAX	20				// 面数

#else

// 問2これはつかわない
POINT	poly_vertex[] = {			// 頂点座標
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
#define POLY_MAX	12				// 面数

#endif

//----- カベ構造体定義
typedef struct {
	POINT	p0, p1;			// 座標
	float	nx, ny;			// 法線ベクトル
	TCHAR	name[256];		// 面の名前
	bool	select;
} FACE;

struct BSPnode {
	BSPnode	*front;			// 表側、裏側ノードへのポインタ
	BSPnode	*back;			// 表側、裏側ノードへのポインタ
	int		face;			// 分割面
	int		*front_face;	// 表側面リストへのポインタ
	int		*back_face;		// 裏側面リストへのポインタ
	int		front_num;		// 表側の面数
	int		back_num;		// 裏側の面数
};

//----- グローバル変数
const TCHAR szClassName[] = _T("2D_BSP");
//const TCHAR szAppName[]   = _T("[ 2D_BSP ] Binary Space Partitioning　＊完成形＊");//ここを変更しておく
const TCHAR szAppName[] = _T("AT14C184_06_70732_オウイットウ");//ここを変更しておく


FACE		g_Face[POLY_MAX * POLY_MAX];		// 面情報
int			g_FaceIndex[POLY_MAX * POLY_MAX];	// 面使用中情報
BSPnode		*g_BSPnode;							// ルートノード用ポインタ

HWND		g_hWndApp;					// ウィンドウハンドル
HDC			g_hMemDC;					// 裏画面ＤＣ
HBITMAP		g_hBitmap;					// 裏画面ビットマップ
bool		g_bOnInfo;					// ベクトル・数値情報表示スイッチ

int			g_DispTreeX;				// ＢＳＰツリー表示Ｘ座標
int			g_DispTreeY;				// ＢＳＰツリー表示Ｙ座標

int			g_nCursor = -1;

HPEN		PenGrid0, PenGrid1;
HPEN		PenBand, PenDot, PenCursor, PenSelect, PenNormal, PenSep;

// 分割線情報
POINT		g_ptSep[POLY_MAX * POLY_MAX + 4][2] =
{
	// 外周を囲む分割線を最初に作成
	{ { -SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 }, { -SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 } },
	{ {  SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 }, {  SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 } },
	{ { -SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 }, {  SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2 } },
	{ { -SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 }, {  SCREEN_WIDTH / 2,  SCREEN_HEIGHT / 2 } },
};

int			g_nSep = 4;	// 分割線の本数(初期値は外周を囲む分割線4本分)

//----- プロトタイプ宣言
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
// メイン
//---------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR args, int cmdShow)
{
	MSG		msg;

	// 変数初期化
	g_bOnInfo = true;			// 情報表示スイッチをＯＦＦ
	g_BSPnode = NULL;

	// ウィンドウクラスを登録
	WNDCLASS wndClass = {
		0, WndProc, 0, 0, hInst,
		LoadIcon(hInst, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		0, NULL, szClassName
	};
	if (RegisterClass(&wndClass) == 0) return false;

	// ウインドウを作成
	g_hWndApp = CreateWindow(
		szClassName, szAppName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		SCREEN_WIDTH  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL, NULL, hInst, NULL);
	if (!g_hWndApp) return false;
	ShowWindow(g_hWndApp, cmdShow);
	UpdateWindow(g_hWndApp);

	// メインループ
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

//-----------------------------------------------------------------------------
// メッセージ処理
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC				hdc;
	int				mx, my;
	PAINTSTRUCT		ps;

	switch (uMsg) {
	case WM_CREATE:
		hdc = GetDC(hWnd);
		g_hMemDC = CreateCompatibleDC(hdc);		// 裏画面の作成
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

		// 各種初期値設定
		Init();
		InvalidateRect(g_hWndApp, NULL, TRUE);
		return 0;
	case WM_KEYDOWN:			// キーが押された
		switch (wParam) {
		case VK_ESCAPE:	DestroyWindow(hWnd);	return 0;	// 終了
		case 'I':				// 情報表示スイッチの切り替え
			g_bOnInfo = !g_bOnInfo;
			InvalidateRect(g_hWndApp, NULL, TRUE);
			return 0;

		case 'C':
			Init();
			InvalidateRect(g_hWndApp, NULL, TRUE);
			return 0;
		}
		break;

	case WM_MOUSEMOVE:			// マウスが動いた
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

	case WM_LBUTTONDOWN:		// マウス左ボタンが押された
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

	case WM_PAINT:				// 描画
		hdc = BeginPaint(hWnd, &ps);
		PatBlt(g_hMemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITENESS);
		DrawGrid();					// グリッドを描画
		DrawPolygon();				// 多角形を描画
		if (g_bOnInfo) DispInfo();	// 情報表示
		BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, g_hMemDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		if (g_BSPnode) DeleteBSPtree(g_BSPnode);	// ＢＳＰツリーを削除
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
// グリッド表示モジュール
//---------------------------------------------------------------------------------------
void DrawGrid()
{
	// 枠を描画
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

	// 紛らわしいため軸表示なし
/*
	SelectObject(g_hMemDC, PenGrid1);
	MoveToEx(g_hMemDC, 0, AXIS_Y_OFFSET, NULL);
	LineTo(g_hMemDC, SCREEN_WIDTH, AXIS_Y_OFFSET);
	MoveToEx(g_hMemDC, AXIS_X_OFFSET, 0, NULL);
	LineTo(g_hMemDC, AXIS_X_OFFSET, SCREEN_HEIGHT);
*/
}

//---------------------------------------------------------------------------------------
// 多角形描画モジュール
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

			// 法線を描画
			SelectObject(g_hMemDC, PenNormal);
			MoveToEx(g_hMemDC, nx0, ny0, NULL);
			LineTo(g_hMemDC, nx1, ny1);

			// 辺を描画
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

			// 辺の端点を描画
			SelectObject(g_hMemDC, PenDot);
			Ellipse(g_hMemDC, px0 - 3, py0 - 3, px0 + 3, py0 + 3);
			Ellipse(g_hMemDC, px1 - 3, py1 - 3, px1 + 3, py1 + 3);

			// インデックスを描画
			len = _stprintf(str, _T("%s"), g_Face[g_FaceIndex[i]].name);
			TextOut(g_hMemDC, (px1 + px0) / 2, (py1 + py0) / 2, str, len);
		}
	}

	SelectObject(g_hMemDC, oldpen);
}

//---------------------------------------------------------------------------------------
// 情報表示モジュール
//---------------------------------------------------------------------------------------
void DispInfo()
{
	TCHAR help[] = _T("Cキーで初期化");
	TextOut(g_hMemDC, 0, 0, help, _tcslen(help));

	g_DispTreeX = 0;
	g_DispTreeY = 120;
	DispBSPtree(g_BSPnode, 0, _T("　"));			// ＢＳＰツリーを描画
}


// 外積のZ成分
int cross_z(POINT* v1, POINT* v2)
{
	return v1->x * v2->y - v1->y * v2->x;
}

// 線分と直線の交差判定(分割線交差チェック用)
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
		// 平行なため交差しない
		return false;
	}
	v1.x = lp0->x - sp0->x;
	v1.y = lp0->y - sp0->y;
	cross2 = cross_z(&v2, &v1);
	
	// 交点が線分の外側にある場合は交差しないと判定
	float t = (float)cross2 / cross1;
	if(t < 0 || t > 1) return false;

	// 交点を計算
	// tをそのまま使いたいが、誤差が出るため計算し直し
	pc->x = (long)(sp0->x + (float)(sp1->x - sp0->x) * cross2 / cross1);
	pc->y = (long)(sp0->y + (float)(sp1->y - sp0->y) * cross2 / cross1);
	return true;
}

// 2点間の距離の2乗
int distance(POINT* p1, POINT* p2)
{
	return (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y);
}

//---------------------------------------------------------------------------------------
// ＢＳＰツリー構築
//
// in  : BSPnode *node 対象ノード
//       int *face_list ノードに含まれる面番号配列
//       int face_num ノードに含まれる面の数
//		 int index 分割面のインデックス
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

	// 新しい面リストを構築するためメモリを確保
	node->front_face = new int[face_num];
	node->back_face  = new int[face_num];
	node->front_num  = 0;					// 表面ノードは０個
	node->back_num   = 0;					// 裏面ノードは０個
	for (i = 0; i < face_num; i++) {
		node->front_face[i] = -1;
		node->back_face[i]  = -1;
	}

	// 分割面を決定（リストの先頭にある面）
	node->face = face_list[index];

	// 分割線の処理(表示用のため、内部処理には関係ない)
	POINT pc;
	int d, d0 = -1, d1 = -1;
	for(i = 0; i < g_nSep; i++)
	{
		// 分割線が交差する場合はそこで止める
		if(CheckCross(&g_ptSep[i][0], &g_ptSep[i][1], &g_Face[face_list[index]].p0, &g_Face[face_list[index]].p1, &pc))
		{
			// 壁のどちらの端側の交点か、ベクトル内積の符号でチェック
			POINT v1, v2;
			v1.x = g_Face[face_list[index]].p1.x - g_Face[face_list[index]].p0.x;
			v1.y = g_Face[face_list[index]].p1.y - g_Face[face_list[index]].p0.y;
			v2.x = pc.x - g_Face[face_list[index]].p0.x;
			v2.y = pc.y - g_Face[face_list[index]].p0.y;
			if(v1.x * v2.x + v1.y * v2.y <= 0)
			{
				// p0側の交点
				if((d = distance(&g_Face[face_list[index]].p0, &pc)) < d0 || d0 < 0)
				{
					// 最も近い交点で止める
					g_ptSep[g_nSep][0] = pc;
					d0 = d;
				}
			}
			else
			{
				// p1側の交点
				if((d = distance(&g_Face[face_list[index]].p1, &pc)) < d1 || d1 < 0)
				{
					// 最も近い交点で止める
					g_ptSep[g_nSep][1] = pc;
					d1 = d;
				}
			}
		}
	}
	g_nSep++;

	// リスト中にある面数だけループ
	for (i = 0; i < face_num; i++) {
		if(i == index) continue;

		// 基準面とリスト中の面とで表裏判定
		ans = CheckFrontBack(&g_Face[face_list[i]], &g_Face[face_list[index]]);
		switch (ans) {
		case 0:			// 表面に属するので表側リストに追加
			node->front_face[node->front_num++] = face_list[i];
			break;
		case 1:			// 裏面に属するので裏側リストに追加
			node->back_face[node->back_num++] = face_list[i];
			break;
			f0 = &g_Face[face_list[index]];
			f1 = &g_Face[face_list[i]];

			// 交点座標px,pyを求める
			t0 = (f0->p0.x - f1->p0.x) * f0->nx +
				 (f0->p0.y - f1->p0.y) * f0->ny;
			t1 = (f1->p1.x - f1->p0.x) * f0->nx +
				 (f1->p1.y - f1->p0.y) * f0->ny;
			px = (int)(f1->p0.x + (f1->p1.x - f1->p0.x) * (t0/t1));
			py = (int)(f1->p0.y + (f1->p1.y - f1->p0.y) * (t0/t1));

			// p0とp1の表裏判定用の内積計算（CheckFrontBack関数を参照）
			ans2 = (int)((f1->p0.x - f0->p0.x) * f0->nx + 
						 (f1->p0.y - f0->p0.y) * f0->ny);
			ans3 = (int)((f1->p1.x - f0->p0.x) * f0->nx + 
						 (f1->p1.y - f0->p0.y) * f0->ny);

			g_bFront = false;
			for (int j = 0; j < POLY_MAX * POLY_MAX; j++) {
				// 未使用の面か
				if (g_FaceIndex[j] == -1) {
					// まだ表面を生成できていないか
					if (g_bFront == false) {
						// 空きを見つけたので新しい表面を作成して追加
						g_FaceIndex[j] = j;
						// 交差している方向によって表側、裏側にある座標を特定
						if (ans2 > 0 || ans3 < 0) {
							// p0 が表側、p1 が裏側
							p0.x = g_Face[face_list[i]].p0.x;
							p0.y = g_Face[face_list[i]].p0.y;
							p1.x = px;
							p1.y = py;
						} else {
							// p0 が裏側、p1 が表側
							p0.x = px;
							p0.y = py;
							p1.x = g_Face[face_list[i]].p1.x;
							p1.y = g_Face[face_list[i]].p1.y;
						}
						// 生成した面の座標を設定
						g_Face[j].p0.x = p0.x;
						g_Face[j].p0.y = p0.y;
						g_Face[j].p1.x = p1.x;
						g_Face[j].p1.y = p1.y;
						_tcscpy(g_Face[j].name, f1->name);
						_tcscat(g_Face[j].name, _T("1"));
						node->front_face[node->front_num++] = j;
						g_bFront = true;
					} else {
						// 空きを見つけたので新しい裏面を作成して追加
						g_FaceIndex[j] = j;
						// 交差している方向によって表側、裏側にある座標を特定
						if(ans2 > 0 || ans3 < 0) {
							// p0 が表側、p1 が裏側
							p0.x = px;
							p0.y = py;
							p1.x = g_Face[face_list[i]].p1.x;
							p1.y = g_Face[face_list[i]].p1.y;
						} else {
							// p0 が裏側、p1 が表側
							p0.x = g_Face[face_list[i]].p0.x;
							p0.y = g_Face[face_list[i]].p0.y;
							p1.x = px;
							p1.y = py;
						}
						// 生成した面の座標を設定
						g_Face[j].p0.x = p0.x;
						g_Face[j].p0.y = p0.y;
						g_Face[j].p1.x = p1.x;
						g_Face[j].p1.y = p1.y;
						_tcscpy(g_Face[j].name, f1->name);
						_tcscat(g_Face[j].name, _T("2"));
						node->back_face[node->back_num++] = j;
						j = POLY_MAX * POLY_MAX;			// ループ脱出
					}
				}
			}
			// この面は分割されたというフラグを設定
			g_FaceIndex[face_list[i]] = -2;
			break;
		}
	}

	CalcNormal();
}

//---------------------------------------------------------------------------------------
// ＢＳＰツリー削除
//---------------------------------------------------------------------------------------
void DeleteBSPtree(BSPnode *node)
{
	if (node->front != NULL) DeleteBSPtree(node->front);	// 表側にノードがあればもぐって削除
	if (node->back  != NULL) DeleteBSPtree(node->back );	// 裏側にノードがあればもぐって削除
	// 面リストメモリを解放
	if (node->front_face) delete[] node->front_face;
	if (node->back_face ) delete[] node->back_face;
	// ノード自身を解放
	delete node;
}

//---------------------------------------------------------------------------------------
// ＢＳＰツリー表示
//---------------------------------------------------------------------------------------
void DispBSPtree(BSPnode *node, int indent, const TCHAR* prefix)
{
	TCHAR str[256];

	if(node == NULL) return;

	if(node->back != NULL)
	{
		DispBSPtree(node->back, indent + 40, _T("／"));
	}
	else
	{
		if(node->back_num > 0)
		{
			_tcscpy(str, _T("／"));
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
		DispBSPtree(node->front, indent + 40, _T("＼"));
	}
	else
	{
		if(node->front_num > 0)
		{
			_tcscpy(str, _T("＼"));
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
// 初期化
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
// 未分類面を検索し、属しているノードを返す
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
// 法線計算モジュール
//---------------------------------------------------------------------------------------
//=======================================================================================
void CalcNormal()
{
	// ベクトルv：壁のベクトル
	int vx, vy, vz;
	Vector wall;;
	// ベクトルw：仮想Z軸ベクトル
	int wx, wy, wz;
	Vector vZ = {0,0,-1};
	// ベクトルの長さ
	float mg;
	
	//すべての未処理の壁に対して処理を行う
	for (int i = 0; i < POLY_MAX * POLY_MAX; i++) {
		if (g_FaceIndex[i] > -1) 
		{
			// 壁のベクトルを作ってvへコピーする　Zは0としておく
			wall.x = g_Face[g_FaceIndex[i]].p1.x - g_Face[g_FaceIndex[i]].p0.x;
			wall.y = g_Face[g_FaceIndex[i]].p1.y - g_Face[g_FaceIndex[i]].p0.y;
			wall.z = 0;

			// wにZ軸に平行でZ成分が負のベクトルを入れておく（0,0,-1）
\
			// vとwの外積を計算（法線方向のベクトルができる）
			Vector normal = wall.Cross(vZ);
			//法線を正規化する
			normal.Normalize();

			g_Face[g_FaceIndex[i]].nx = normal.x;//floatへキャストして代入
			g_Face[g_FaceIndex[i]].ny = normal.y;//　　　　//
			//.nzは0なので省略

			// 上で作った法線を正規化するので、まず長さを求める
			//mg = sqrtf(g_Face[g_FaceIndex[i]].nx * g_Face[g_FaceIndex[i]].nx +g_Face[g_FaceIndex[i]].ny * g_Face[g_FaceIndex[i]].ny);
			
			// 各ベクトルの要素を長さで割る＝正規化
			//[                          ];
			//[                          ];
			//.nzは0なので省略
		}
	}
}

//=======================================================================================
//---------------------------------------------------------------------------------------
// 線分同士の表裏位置判定モジュール
//
// in  : FACE *face0 判定するノード
//       FACE *face1 判定のベースとなるノード（このノードに対して表か裏かを判定）
// out : int 0 = face0 は face1 の表側
//           1 = face0 は face1 の裏側
//           2 = face0 は face1 と交差
//
//---------------------------------------------------------------------------------------
//=======================================================================================
int CheckFrontBack(FACE *face0, FACE *face1)
{
	int	ans0 = 0;
	int ans1 = 0;

	// face1のP0点からface0のP0へのベクトル v0 = face0->p0 - face1->p0
	int v0x = face0->p0.x - face1->p0.x;
	int v0y = face0->p0.y - face1->p0.y;

	Vector v0 = { (float)v0x,(float)v0y,0 };
	Vector n = { face1->nx,face1->ny,0 };
	// v0とface1の法線との内積ans0 = v0・face1.n
	ans0 = (int)v0.Dot(n);

	// face1のP0点からface0のP1へのベクトル v1 = face0->p1 - face1->p0
	int v1x = face0->p1.x - face1->p0.x;
	int v1y = face0->p1.y - face1->p0.y;

	Vector v1 = { (float)v1x,(float)v1y,0 };
	// v1とface1の法線ベクトルとの内積 ans1 = v1・face1->n
	ans1 = (int)v1.Dot(n);

	// 上記性質より、下記判定が成り立つ
	if ((ans0 * ans1) < 0) return 2;	// ans0とans1の符号が異なる場合は壁が交差←今回は無し
	if (ans0 > 0 || ans1 > 0) return 0;	// 双方が正の場合は表側
	else return 1;						// 双方が0以下の場合は裏側
		//（双方0の場合は両線が完全に重なっていることを示し今回はこの場合も裏側と見なす）
}

/*
	とりあえずサンプルのマップでうまくできたら
	マップを作り変えてみよう。
	配布のエクセルでデザインしてから座標を割り出して配列に並べる。
	壁の途中で壁が交差しないように必ず壁の一辺を５０の線で作る。

*/
//=======================================================================================
//	end of file
//=======================================================================================
