//=======================================================================================
//
//	  [ 3D_LINE_TRIANGLE ] : ３Ｄ空間における線分と三角形との交差判定（演習用）
//
//=======================================================================================

//-------- インクルードヘッダ
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define STRICT
//-------- ライブラリリンク
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>		// Windowsプログラムにはこれを付ける
#include <tchar.h>			// 汎用テキスト マッピング
#include <stdio.h>
#include <math.h>			// 三角関数で使用
#include <mmsystem.h>		// 時間管理で使用
#include <d3d9.h>			// DirectX Graphics 関連のヘッダ
#include <d3dx9.h>			// DirectX Graphics 関連のヘッダ

#include "Collision.h"


//-------- マクロ
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if(x){ x->Release(); x=NULL; }
#endif

//-------- 定数定義
#define CLASS_NAME		_T("3D_LINE_TRIANGLE")											// ウィンドウクラス名
//#define CAPTION_NAME	_T("[ 3D_LINE_TRIANGLE ] ３Ｄ空間における線分と三角形との交差判定")	// キャプション名
#define CAPTION_NAME	_T("AT14C184_06_70732_オウイットウ")	// キャプション名

#define SCREEN_WIDTH	640									// スクリーンの幅
#define SCREEN_HEIGHT	480									// スクリーンの高さ
#define FRAME_RATE		1000.0f/60.0f						// フレームレート
#define ASPECT_RATIO	(float)SCREEN_WIDTH/SCREEN_HEIGHT	// アスペクト比
#define NEAR_CLIP		100.0f								// ニアクリップを行う距離
#define FAR_CLIP		30000.0f							// ファークリップを行う距離
#define LINE_MOVE_SPEED	0.2f								// ライン移動速度

//-------- 頂点フォーマット
typedef struct {
	D3DXVECTOR3	pos;
	DWORD		diffuse;
} VERTEX;

#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

//-------- 変数
LPDIRECT3D9				g_pD3D;					// Direct3D9 オブジェクト
LPDIRECT3DDEVICE9		g_pD3DDevice;			// Direct3DDevice9 オブジェクト
D3DPRESENT_PARAMETERS	g_D3DPresentParam;		// PRESENT PARAMETERS
LPD3DXFONT				g_pD3DXFont;			// D3DXFont オブジェクト
HWND					g_hWnd;					// ウィンドウハンドル
TCHAR					g_szDebug[1024];		// デバッグ用ストリングデータ

int						g_MousePosX   , g_MousePosY;	// マウスによる回転用
int						g_MouseOldPosX, g_MouseOldPosY;
int						g_MouseSpeedX , g_MouseSpeedY;
bool					g_MouseButtonL;
float					g_CameraDeg;

D3DXMATRIX				g_MatView;						// ビューマトリックス

VERTEX					g_VertexTriangle[3];			// 三角形の頂点
VERTEX					g_VertexLine[2];				// 線分の頂点

bool					g_bOnHit;					// 当たりチェックフラグ

//-------- プロトタイプ宣言
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT InitializeGraphics(HWND hWnd, bool bWindow);
HRESULT CleanupGraphics();
HRESULT RenderGraphics();
void ExecMain();
void DrawMain();
bool HitCheck();

//---------------------------------------------------------------------------------------
//	メイン
//---------------------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int iCmdShow)
{
	MSG		msg;
	DWORD	dwExecLastTime;
	DWORD	dwCurrentTime;

	// ウィンドウクラスの登録
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc,
		0, 0, hInstance, LoadIcon(hInstance, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL, CLASS_NAME, NULL};
	if (RegisterClassEx(&wc) == 0) return false;	// ウィンドウクラスを登録

	// ウィンドウを作成
	g_hWnd = CreateWindow(
		CLASS_NAME, CAPTION_NAME, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		SCREEN_WIDTH  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,
		SCREEN_HEIGHT + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
		NULL, NULL, hInstance, NULL);
	if (g_hWnd == NULL) return false;
	ShowWindow(g_hWnd, iCmdShow);		// ウィンドウを表示
	UpdateWindow(g_hWnd);				// ウィンドウをアップデート

	// グラフィック環境の初期化
	bool	bWindow = false;
	if (IDYES == MessageBox(g_hWnd, _T("ウィンドウモードで実行しますか？"), _T("画面モード"), MB_YESNO))
		bWindow = true;
	if (FAILED(InitializeGraphics(g_hWnd, bWindow))) return false;		// ここで初期化

	// 各種初期化
	timeBeginPeriod(1);									// タイマーの分解能を最小にセット
	dwExecLastTime = timeGetTime();						// 現在のシステムタイマーを取得
	g_MouseButtonL = false;
	g_CameraDeg = 0.0f;

	// メッセージループ
	msg.message = WM_NULL;
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {		// メッセージをチェック
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			dwCurrentTime = timeGetTime();						// 現在のタイマー値を取得
			if (dwCurrentTime - dwExecLastTime > FRAME_RATE) {
				ExecMain();										// メイン処理
				RenderGraphics();								// レンダリング処理	
			}
		}
		Sleep(1);					// いったんWindowsに制御を戻す
	}

	// グラフィック環境のクリーンアップ
	CleanupGraphics();
	timeEndPeriod(1);				// タイマーの分解能を元に戻す

	return msg.wParam;
}

//---------------------------------------------------------------------------------------
//	ウィンドウプロシージャ
//---------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		return 0;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			DestroyWindow(hWnd);		// ウィンドウを破棄するよう指示する
			return 0;
		}
		break;
	case WM_MOUSEMOVE:				//----- マウスカーソルが移動した
		if (g_MouseButtonL) {
			g_MousePosX = LOWORD(lParam);
			g_MousePosY = HIWORD(lParam);
		}
		return 0;
	case WM_LBUTTONDOWN:			//----- マウスの左ボタンが押された
		g_MouseButtonL = true;
		g_MouseOldPosX = g_MousePosX = LOWORD(lParam);
		g_MouseOldPosY = g_MousePosY = HIWORD(lParam);
		return 0;
	case WM_LBUTTONUP:				//----- マウスの左ボタンが離された
		g_MouseButtonL = false;
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//---------------------------------------------------------------------------------------
// グラフィック環境の初期化
//---------------------------------------------------------------------------------------
HRESULT InitializeGraphics(HWND hWnd, bool bWindow)
{
	// デバイス情報テーブル
	struct TCreateDevice {
		D3DDEVTYPE type;			// デバイスの種類
		DWORD      behavior;		// デバイスの動作
	};
	const int c_nMaxDevice = 3;
	const TCreateDevice device[c_nMaxDevice] = {
		{ D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING },	// ハードウェアシェーダーを使う
		{ D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING },	// HAL
		{ D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING },	// REF
	};
	int		nDev;			// デバイス種別カウンタ

	// Direct3Dオブジェクトを生成
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
		MessageBox(hWnd, _T("Direct3Dオブジェクトの作成に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
	// Direct3D 初期化パラメータの設定
	ZeroMemory(&g_D3DPresentParam, sizeof(D3DPRESENT_PARAMETERS));
	g_D3DPresentParam.BackBufferWidth        = SCREEN_WIDTH;			// ゲーム画面サイズ
	g_D3DPresentParam.BackBufferHeight       = SCREEN_HEIGHT;
	g_D3DPresentParam.Windowed               = bWindow;					// ウィンドウモードか?
	g_D3DPresentParam.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	g_D3DPresentParam.EnableAutoDepthStencil = TRUE;					// Ｚバッファ有効
	g_D3DPresentParam.AutoDepthStencilFormat = D3DFMT_D24S8;			// Ｚバッファフォーマット
	if (bWindow) {
		// ウィンドウモード
		g_D3DPresentParam.BackBufferFormat           = D3DFMT_UNKNOWN;				// バックバッファ
		g_D3DPresentParam.FullScreen_RefreshRateInHz = 0;							// リフレッシュレート
		g_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;	// 垂直同期を待たない
	} else {
		// フルスクリーンモード
		g_D3DPresentParam.BackBufferFormat           = D3DFMT_X8R8G8B8;				// バックバッファ
		g_D3DPresentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
		g_D3DPresentParam.PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;	// 垂直同期を待つ
	}

	// デバイスオブジェクトを作成
	for (nDev = 0; nDev < c_nMaxDevice; nDev++) {
		if (SUCCEEDED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, device[nDev].type, hWnd,	// デバイスを作成
			device[nDev].behavior, &g_D3DPresentParam, &g_pD3DDevice))) break;
	}
	if (nDev >= c_nMaxDevice) {
		MessageBox(hWnd, _T("デバイスの作成に失敗しました"), _T("error"), MB_OK | MB_ICONERROR);
		return E_FAIL;
	}

	// 情報表示用フォントを設定
	D3DXCreateFont(g_pD3DDevice, 18, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Terminal"), &g_pD3DXFont);

	// プロジェクションマトリックス設定
	D3DXMATRIX	matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(30), ASPECT_RATIO, NEAR_CLIP, FAR_CLIP);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// ワールドマトリックス設定
	D3DXMATRIX	matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// レンダリングステートを設定
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		// 光源計算をＯＦＦ
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// カリングをしない
	
	// 頂点バッファ、頂点フォーマットフラグを設定
	g_pD3DDevice->SetFVF(FVF_VERTEX);

	// オブジェクトの初期化
	// 三角形：頂点バッファに頂点値を設定
	g_VertexTriangle[0].pos = D3DXVECTOR3(-100.0f,  100.0f, 100.0f);
	g_VertexTriangle[1].pos = D3DXVECTOR3( 100.0f,  100.0f,  50.0f);
	g_VertexTriangle[2].pos = D3DXVECTOR3(-100.0f, -100.0f,   0.0f);
	
	// 線分：頂点バッファに頂点値を設定
	g_VertexLine[0].pos     = D3DXVECTOR3(-200.0f,  100.0f, -200.0f);
	g_VertexLine[1].pos     = D3DXVECTOR3(-200.0f, -100.0f,    0.0f);
	g_VertexLine[0].diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	g_VertexLine[1].diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	return S_OK;
}

//---------------------------------------------------------------------------------------
// グラフィック環境のクリーンアップ
//---------------------------------------------------------------------------------------
HRESULT CleanupGraphics()
{
	// オブジェクトの後始末
	SAFE_RELEASE(g_pD3DXFont);			// D3DXFont オブジェクトを解放
	SAFE_RELEASE(g_pD3DDevice);			// Direct3DDevice オブジェクトを解放
	SAFE_RELEASE(g_pD3D);				// Direct3D オブジェクトを解放
	return S_OK;
}

//---------------------------------------------------------------------------------------
// グラフィック環境のレンダリング
//---------------------------------------------------------------------------------------
HRESULT RenderGraphics()
{
	// ビューポートをクリア
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xff,0xff,0xff), 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene())) {	// シーン開始

		DrawMain();									// 描画処理

		g_pD3DDevice->EndScene();					// シーン終了
	}

	// バックバッファに描画した内容を表示
	if (FAILED(g_pD3DDevice->Present(NULL, NULL, NULL, NULL))) {
		g_pD3DDevice->Reset(&g_D3DPresentParam);
	}
	return S_OK;
}


//---------------------------------------------------------------------------------------
// メイン処理（メインループからコールされる）
//---------------------------------------------------------------------------------------
void ExecMain()
{
	TCHAR		str[256];

	g_szDebug[0] = _T('\0');		// デバッグストリング初期化

	_stprintf(str, _T("マウスの左右でカメラ回転\n"));
	lstrcat(g_szDebug, str);
	_stprintf(str, _T("カーソルキーでラインの移動（ＸＺ方向）\n"));
	lstrcat(g_szDebug, str);

	//-------- 直線をカーソルで移動
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

	//----- マウス移動でカメラを動かす
	g_MouseSpeedX = g_MousePosX - g_MouseOldPosX;
	g_MouseSpeedY = g_MousePosY - g_MouseOldPosY;
	g_MouseOldPosX = g_MousePosX;
	g_MouseOldPosY = g_MousePosY;
	g_CameraDeg -= (float)g_MouseSpeedX * 0.01f;

	//----- 当たり判定！
	g_bOnHit = HitCheck();
}

//---------------------------------------------------------------------------------------
// 描画処理（RenderGraphics からコールされる）
//---------------------------------------------------------------------------------------
void DrawMain()
{
	// ビューマトリックス設定
	float px, pz;
	px =  sinf(g_CameraDeg) * 1000.0f;
	pz = -cosf(g_CameraDeg) * 1000.0f;
	D3DXMatrixLookAtLH(&g_MatView,
						&D3DXVECTOR3(   px,  0.0,     pz),		// 視点座標
						&D3DXVECTOR3(  0.0,  0.0,    0.0),		// 注視点座標
						&D3DXVECTOR3(  0.0,  1.0,    0.0));		// アップベクトル
	g_pD3DDevice->SetTransform(D3DTS_VIEW, &g_MatView);

	// あたり判定フラグによって三角形のカラーを変化させる
	DWORD	color;
	if (g_bOnHit) color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	else		  color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	g_VertexTriangle[0].diffuse = color;
	g_VertexTriangle[1].diffuse = color;
	g_VertexTriangle[2].diffuse = color;

	// 三角形、線分の描画を実行
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, &g_VertexTriangle[0], sizeof(VERTEX));	// 三角形
	g_pD3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, &g_VertexLine[0], sizeof(VERTEX));			// 線分
	
	// デバッグストリング描画
	RECT	strRect = {0, 0, 640, 480};
	g_pD3DXFont->DrawText(NULL, g_szDebug, -1, &strRect, DT_LEFT, D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0x00));
}

//---------------------------------------------------------------------------------------
// 交差判定チェックモジュール
// in  : ---
//　VERTEX　g_VertexTriangle[3];	// 三角形の頂点
//　VERTEX　g_VertexLine[2];		// 線分の頂点
//
//
// out : bool true = ヒットした , false = ヒットしなかった
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

	ここに交差判定のプログラムを追加する

	必要となるローカル変数は自由に作る


	（Phase 1)
	・三角形の法線ベクトル（正規化する）を求める
	・法線ベクトルに対して直線を投影した結果を求める（２点存在。内積を使う）
	・三角形の頂点座標を投影して上記の直線を投影した範囲内かどうか調査する
	・範囲外なら分離面が存在するとして「衝突していない」として終了

	（Phase 2)
	・三角形のエッジを表すベクトルと線分に直交するベクトルを外積で求める（正規化する）
	・直交ベクトルに三角形の３頂点を投影する（内積を使う）
	・投影した３点から最小値、最大値を求めて「範囲」を求める
	・線分の頂点（どちらか一つ）が範囲内かどうか調査する
	・範囲外なら分離面が存在するとして「衝突していない」として終了
	　＜Phase2の処理をエッジ３つ分について繰り返す＞
	  
	（補足）
	・ヒットしたら true 、ヒットしなかったら false でリターンする

	*************************************************************************/

	//return false;		// 仮
}

//=======================================================================================
//	End of Files
//=======================================================================================