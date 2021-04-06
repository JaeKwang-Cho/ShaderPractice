#include "d3_init.h"

// D3D ����
LPDIRECT3D9             gpD3D = NULL;				// D3D
LPDIRECT3DDEVICE9       gpD3DDevice = NULL;				// D3D ��ġ

const char* gAppName = "���̽� �ڵ�";

//-----------------------------------------------------------------------
// ���α׷� ������/�޽��� ����
//-----------------------------------------------------------------------

// ������
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// 1. ������ Ŭ������ ����Ѵ�.
	// ���α׷��� â�� �����ϱ� ���ؼ��� ���� ���� ������ Ŭ������ ����ؾ��Ѵ�.
	// 1) Ŭ���� ũ��, 2) ��Ÿ�� 3) ������ ���ν��� ������ 4) Ŭ���� �߰� �޸� 5) ������ �߰��޸�
	// 6) �ڵ鷯 7) ������ ���ҽ� ������ 8) Ŀ�� ���ҽ� ������ 9) �귯�� �ڵ� 10) �޴� �̸� ������
	// 11) Ŭ���� �̸� ������ 12) ������ ������
	// ��... �� �ϳ��ϳ��� �ñ��ϸ� ã�ƺ���
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  gAppName, NULL };
	RegisterClassEx(&wc);

	// 2. ���α׷� â�� �����Ѵ�.
	// ������ Ŭ������ �ν��Ͻ��� �����Ѵ�.
	// ���� � ���ڸ� �ѱ���� �ñ��ϸ� ã�ƺ���
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	HWND hWnd = CreateWindow(gAppName, gAppName,
		style, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// 3. Client Rect ũ�Ⱑ WIN_WIDTH, WIN_HEIGHT�� ������ ũ�⸦ �����Ѵ�.
	POINT ptDiff;
	RECT rcClient, rcWindow;

	// ���� �������� ������ ������ Client Rect�̴�.
	GetClientRect(hWnd, &rcClient);
	GetWindowRect(hWnd, &rcWindow);
	// ������ Ʋ�� ©���� �κ��� ��ǥ�� ���ϰ�
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
	// �����Ѵ�.
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, WIN_WIDTH + ptDiff.x, WIN_HEIGHT + ptDiff.y, TRUE);

	// 4. ���� â�� �����ش�.
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);

	// 5. D3D�� ����� ��� ���� �ʱ�ȭ�Ѵ�.
	if (!InitEverything(hWnd))
		PostQuitMessage(1);

	// 6. �޽��� ����
	MSG msg;
	// ������ ��ũ�� �Լ� �޸𸮸� 0�� �ʱ�ȭ
	ZeroMemory(&msg, sizeof(msg));
	// WM_QUIT �ޱ� ������ ��� ������.
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

	// �̰͵� ���� ��ũ���Լ��̰�, ������ Ŭ������ ����� �����Ѵ�.
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
