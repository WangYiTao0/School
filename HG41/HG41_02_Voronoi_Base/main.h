#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <assert.h>

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "xaudio2.lib")


#define SCREEN_WIDTH	(1600)			// ウインドウの幅
#define SCREEN_HEIGHT	(900)			// ウインドウの高さ


HWND GetWindow();
