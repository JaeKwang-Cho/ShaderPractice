#pragma once

#include <d3dx9.h>

namespace d3d
{
	bool InitD3D(
		HINSTANCE hInstance,		//  앱 인스턴스
		int width, int height,		// 후면 버퍼크기
		bool windowed,				// 윈도우 true, 전체화면 false

		D3DDEVTYPE deviceType,		// HAL 또는 REF
		IDirect3DDevice9** device);	// 출력, 만들어진 장치

	int EnterMsgLoop(
		bool (*ptr_display) (float timeDelta)
	);

	LRESULT CALLBACK WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam
	);

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