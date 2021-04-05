#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#define WIN_WIDTH		800
#define WIN_HEIGHT		600

// ���ؽ� ����ü�� ����ü�� ������ ���ؽ� ���� ����
struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z)
	{
		_x = x; _y = y; _z = z;
	}
	float _x, _y, _z;
	// �ϴ��� ��ġ������ �����Ѵ�.
	static const DWORD FVF;
};

// �޽��� ó���� ����
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcessInput(HWND hWnd, WPARAM keyPress);
int EnterMsgLoop(bool (*ptr_display)(float timeDelta));

// �ڿ��Ҵ� ����
bool Setup();

// �ʱ�ȭ ����
bool InitEverything(HWND hWnd);
bool InitD3D(HWND hWnd);

// ������
bool Display(float timeDelta);

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