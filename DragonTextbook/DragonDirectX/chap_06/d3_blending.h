#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#define WIN_WIDTH		800
#define WIN_HEIGHT		600

struct Vertex
{
	Vertex(float x, float y, float z, float nx, float ny, float nz)
	{
		_x = x; _y = y; _z = z; _nx = nx; _ny = ny; _nz = nz;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	// 일단은 위치정보만 저장한다.
	static const DWORD FVF;
};

namespace d3light
{
	// 색깔
	const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
	const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
	const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
	const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
	const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
	const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
	const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
	const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));

	// 재질
	D3DMATERIAL9 InitMtrl(D3DXCOLOR ambient, D3DXCOLOR diffuse, D3DXCOLOR specular, D3DXCOLOR emissive, float power);

	const D3DMATERIAL9 WHITE_MTRL = d3light::InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
	const D3DMATERIAL9 RED_MTRL = d3light::InitMtrl(RED, RED, RED, BLACK, 2.0f);
	const D3DMATERIAL9 GREEN_MTRL = d3light::InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
	const D3DMATERIAL9 BLUE_MTRL = d3light::InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
	const D3DMATERIAL9 YELLOW_MTRL = d3light::InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

	// 빛
	D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
	D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
	D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);
}


// 메시지 처리기 관련
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcessInput(HWND hWnd, WPARAM keyPress);

// 자원할당 관련
bool Setup();

// 초기화 관련
bool InitEverything(HWND hWnd);
bool InitD3D(HWND hWnd);

// 렌더링
bool Display(float timeDelta);

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

