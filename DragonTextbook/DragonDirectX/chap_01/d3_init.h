#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#define WIN_WIDTH		800
#define WIN_HEIGHT		600

// �޽��� ó���� ����
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcessInput(HWND hWnd, WPARAM keyPress);

// �ڿ��Ҵ� ����
bool Setup();

// �ʱ�ȭ ����
bool InitEverything(HWND hWnd);
bool InitD3D(HWND hWnd);
/*
bool InitD3D(HINSTANCE hInstance,		//  �� �ν��Ͻ�
	int width, int height,		// �ĸ� ����ũ��
	bool windowed,				// ������ true, ��üȭ�� false

	D3DDEVTYPE deviceType,		// HAL �Ǵ� REF
	IDirect3DDevice9** device);	// ���, ������� ��ġ
	*/
int EnterMsgLoop(
	bool (*ptr_display) (float timeDelta)
);

// ������ ����
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