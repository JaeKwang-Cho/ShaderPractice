#pragma once

#include <d3dx9.h>

namespace d3d
{
	bool InitD3D(
		HINSTANCE hInstance,		//  �� �ν��Ͻ�
		int width, int height,		// �ĸ� ����ũ��
		bool windowed,				// ������ true, ��üȭ�� false

		D3DDEVTYPE deviceType,		// HAL �Ǵ� REF
		IDirect3DDevice9** device);	// ���, ������� ��ġ

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