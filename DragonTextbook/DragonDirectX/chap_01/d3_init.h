#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#define WIN_WIDTH		800
#define WIN_HEIGHT		600

// 메시지 처리기 관련
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcessInput(HWND hWnd, WPARAM keyPress);

// 초기화 관련
bool InitEverything(HWND hWnd);
bool InitD3D(HWND hWnd);

// 뒷정리 관련
void Cleanup();