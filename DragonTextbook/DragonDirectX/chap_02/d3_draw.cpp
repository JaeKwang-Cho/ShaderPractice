#include "d3_draw.h"

// D3D ����
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D ��ġ

// �Թ�ü ���ؽ�, �ε��� 
IDirect3DVertexBuffer9* VB = NULL;
IDirect3DIndexBuffer9* IB = NULL;



const char* gAppName = "������ �޽� �׷�����";

//-----------------------------------------------------------------------
// ���α׷� ������/�޽��� ����
//-----------------------------------------------------------------------

// ������
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  gAppName, NULL };
	RegisterClassEx(&wc);

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindow(gAppName, gAppName,
		style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);
	POINT ptDiff;
	RECT rcClient, rcWindow;

	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);

	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);


	if (!InitEverything(hWnd))
		PostQuitMessage(1);

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	static float lastTime = (float)timeGetTime();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;

			Display(timeDelta);

			lastTime = currTime;
		}
	}
	
	UnregisterClass(gAppName, wc.hInstance);
	return 0;
}

// �޽��� ó����
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		ProcessInput(hWnd, wParam);
		break;

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}
	// ó������ �ʴ� �޽����� default �⺻ ó��
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Ű���� �Է�ó��
void ProcessInput(HWND hWnd, WPARAM keyPress)
{
	switch (keyPress)
	{
		// ESC Ű�� ������ ���α׷��� �����Ѵ�.
	case VK_ESCAPE:
		// ������ �ڵ鿡�� �޽����� ������ ��ũ���Լ�
		PostMessage(hWnd, WM_DESTROY, 0L, 0L);
		break;
	}
}

bool Display(float timeDelta)
{
	if (gpD3DDevice)
	{
		// �Թ�ü�� ȸ�� ��Ų��.
		D3DXMATRIX Rx, Ry;

		// x ������ 45�� ȸ�� ��Ų��.
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

		// �� �����Ӵ� y ȸ���� ������Ų��.
		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);
		y += timeDelta;

		// ������ 2*PI�� �̸��� 0���� �ʱ�ȭ �Ѵ�.
		if (y >= 6.28f)
		{
			y = 0.0f;
		}
		// ȸ���� �����Ѵ�.
		D3DXMATRIX p = Rx * Ry;

		gpD3DDevice->SetTransform(D3DTS_WORLD, &p);

		// ����� �׸���.
		gpD3DDevice->Clear(0, 0,
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xffffffff, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
		gpD3DDevice->SetIndices(IB);
		gpD3DDevice->SetFVF(Vertex::FVF);
		gpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0, 0, 8, 0, 12);
		// 1. �⺻���� 2.�ε��� ���� ���� ���� 3.	���� ���� ���� ���� 4. ������ ��
		//  5. �ε��� �����Ҷ� ������ �����ϴ� ��ġ 6. �⺻ ������ ��


		gpD3DDevice->EndScene();
		gpD3DDevice->Present(0, 0, 0, 0);

	}
	return true;
}


int EnterMsgLoop(bool (*ptr_display)(float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;

			ptr_display(timeDelta);

			lastTime = currTime;
		}
	}
	return msg.wParam;
}
