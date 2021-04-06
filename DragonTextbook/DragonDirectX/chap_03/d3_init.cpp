#include "d3_color.h"

// D3D ����
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D ��ġ

// ���� ��ȯ�� �ﰢ�� ���ؽ�������
D3DXMATRIX				World;
IDirect3DVertexBuffer9* Triangle = NULL;



const char* gAppName = "��ĥ�غ���";

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
		gpD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0xffffffff, 1.0f, 0);
		gpD3DDevice->BeginScene();

		gpD3DDevice->SetFVF(ColorVertex::FVF);
		gpD3DDevice->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));

		// �÷� ���̵����� ȭ�� ���ʿ� �ﰢ���� �׸���.
		D3DXMatrixTranslation(&World, -1.25f, 0.0f, 0.0f);
		gpD3DDevice->SetTransform(D3DTS_WORLD, &World);

		gpD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// �׶��� ���̵����� ȭ�� �����ʿ� �ﰢ���� �׸���.
		D3DXMatrixTranslation(&World, 1.25f, 0.0f, 0.0f);
		gpD3DDevice->SetTransform(D3DTS_WORLD, &World);

		gpD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		gpD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);


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
