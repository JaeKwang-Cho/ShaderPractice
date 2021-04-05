#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#define WIN_WIDTH		800
#define WIN_HEIGHT		600

// 메시지 처리기 관련
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcessInput(HWND hWnd, WPARAM keyPress);

// 자원할당 관련
bool Setup();

// 초기화 관련
bool InitEverything(HWND hWnd);
bool InitD3D(HWND hWnd);
/*
bool InitD3D(HINSTANCE hInstance,		//  앱 인스턴스
	int width, int height,		// 후면 버퍼크기
	bool windowed,				// 윈도우 true, 전체화면 false

	D3DDEVTYPE deviceType,		// HAL 또는 REF
	IDirect3DDevice9** device);	// 출력, 만들어진 장치
	*/
int EnterMsgLoop(
	bool (*ptr_display) (float timeDelta)
);

// 뒷정리 관련
void Cleanup();

namespace d3d
{
	template <class T> void Release(T t)
	{
		if (t)
		{
			t->Release();
			t = 0;
		}
	}
	template <class T> void Delete(T t)
	{
		if (t)
		{
			delete t;
			t = 0;
		}
}

}