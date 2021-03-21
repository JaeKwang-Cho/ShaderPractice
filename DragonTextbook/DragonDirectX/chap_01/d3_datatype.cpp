#include <d3d9.h>
#include <d3dx9.h>

/* 표면 (Surface) */
// 표면 정보
D3DSURFACE_DESC surfaceDESC; 
// IDirect3DSurface9 인터페이스에서 ->GetDesc로 얻는다.

// 표면 픽셀 데이터로의 포인터를 얻는다.
D3DLOCKED_RECT lockedRect;
// IDirect3DSurface9 인터페이스에서 -> LockRect()로 얻는다.

/* 멀티 셈플링 */
D3DMULTISAMPLE_TYPE multiSamType;
// none 그리고 1부터 16까지 레벨을 지정할 수 있다.
// IDirect3D9::CheckDeviceMultiSampleType(); 으로 그래픽 카드에서 지원하는 멀티 샘플링 타입과 품질 레벨을 확인한다.

/* 픽셀 포맷 */
D3DFORMAT format; \
// 24비트, 32비트 (알파), 64비트, 128 등등 많이 있다
// 궁금하면 찾아보자.

/* 메모리 풀 */
D3DPOOL pool1 = D3DPOOL_DEFAULT;  // 반드시 IDirect3DDevice9::reset 이전에 해제 해야 한다. 그리고 다시 초기화 한다.
D3DPOOL pool2 = D3DPOOL_MANAGED;
D3DPOOL pool3 = D3DPOOL_SYSTEMMEM;
D3DPOOL pool4 = D3DPOOL_SCRATCH;

/* 스왑 체인 */
// IDirect3DSwapChain9 인터페이스: 쓸일 없다.

/* 깊이 버퍼 */

/* 버텍스 프로세싱 */
 
/* 장치 특성 */
D3DCAPS9 devcaps;
bool supportsHardwareVertexProcessing = D3DDEVCAPS_HWTRANSFORMANDLIGHT; 
// 켜져있으면 지원한다.

