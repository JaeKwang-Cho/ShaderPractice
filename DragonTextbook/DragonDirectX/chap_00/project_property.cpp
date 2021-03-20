
#include "project_property.h"
#include <stdio.h>


void type_practice()
{
	/* Vector */
	D3DXVECTOR3 u(1.0f, 0.0f, 1.0f);
	D3DXVECTOR3 v(0.0f, 1.0f, 0.0f);

	// +=, -=, *=, /=, +, -,*, /, == !=
	// Internally use Epsilon
	
	float vec_length = D3DXVec3Length(&u);

	D3DXVECTOR3 nu;
	D3DXVec3Normalize(&nu, &u);
	D3DXVECTOR3 nv;
	D3DXVec3Normalize(&nv, &v);

	float dot = D3DXVec3Dot(&nu, &nv);
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, &nu, &nv);

	/* Matrix */
	D3DXMATRIX A(1.0, 2.0, 3.0, 0.0, 2.0, 3.0, 4.0, 0.0, 3.0, 4.0, 5.0, 0.0, 4.0, 5.0, 6.0, 0.0); // 초기화1
	float arrB[16] = { 1.0 };
	D3DXVECTOR4 B(arrB); // 초기화 2
	D3DXMATRIX I;
	D3DXMatrixIdentity(&I); //d3dx9math.h

	D3DXMATRIX PA;
	D3DXMatrixTranspose(&PA, &A); // 전치 행렬

	D3DXVECTOR3 p1(2.0f, 4.0f, 2.0f);
	D3DXMATRIX tm;
	D3DXMatrixTranslation(&tm, 0.5f, 0.5f, 0.5f); // 이동행렬

	float pi = D3DX_PI;

	// 각 축으로 회전하는 행렬
	D3DXMATRIX XR;
	D3DXMatrixRotationX(&XR, pi / 4.0f);
	D3DXMATRIX YR;
	D3DXMatrixRotationY(&YR, pi / 4.0f);
	D3DXMATRIX ZR;
	D3DXMatrixRotationZ(&ZR, pi / 4.0f);

	// 크기 변형 행렬
	D3DXMATRIX SM;
	D3DXMatrixScaling(&SM, 2.0f, 2.0f, 2.0f);

	/* 중요 */
	D3DXVECTOR3 p2;
	D3DXVec3TransformCoord(&p2, &p1, &tm); // 벡터를 점으로 사용 정보 사용
	D3DXVECTOR3 p3;
	D3DXVec3TransformNormal(&p3, &p1, &tm); // 벡터를 벡터로 사용

	// 사실 이걸 훨씬 많이 쓸것 같다.
	// ->	D3DXVec3TransformCoordArray(); 
	// ->	D3DXVec3TransformNormalArray();

	// 평면
	D3DXPLANE Q(1.0f, 1.0f, 1.0f, 3.0f); // 법선 벡터와 d 값
	// 법선 벡터가 단위 길이면 -> (평면과 임의의 점까지의 거리) = (법선 벡터) dot (점 벡터) + (d)

	// 공간과 관계
	D3DXPLANE P(0.0f, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 V(3.0f, 5.0f, 2.0f);

	float x = D3DXPlaneDotCoord(&P, &V);
	if ((x - 0.0f) <= FLT_EPSILON);
	if (x > 0.0f);
	if (x < 0.0f);

	// 평면 생성
	D3DXPLANE plane1;
	D3DXVECTOR3 normal(1.0f, 0.0f, 1.0f);
	D3DXVECTOR3 pointInPlane(2.0f, 1.0f, 3.0f);
	D3DXPlaneFromPointNormal(&plane1, &normal, &pointInPlane);

	D3DXPLANE plane2;
	D3DXVECTOR3 point1(1.0f,0.0f,0.0f);
	D3DXVECTOR3 point2(0.0f,1.0f,0.0f);
	D3DXVECTOR3 point3(0.0f,0.0f,1.0f);
	D3DXPlaneFromPoints(&plane2, &point1, &point2, &point3);

	// 평면의 정규화
	D3DXPLANE norPlane;
	D3DXPlaneNormalize(&norPlane, &plane1);

	// 평면 변환 (이건 궁금하면 찾아보자)
	// 일단 X축으로 회전하는 행렬 가져옴
	D3DXMATRIX invXR;
	D3DXMATRIX invTransXR;

	D3DXMatrixInverse(&invXR, 0, &XR);
	D3DXMatrixTranspose(&invTransXR, &XR);

	D3DXPlaneNormalize(&plane2, &plane2);

	D3DXPlaneTransform(&plane2, &plane2, &invTransXR);

}





