#include "pch.h"
#include "CMonsterBase.h"

CMonsterBase::CMonsterBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCharacter(pGraphicDev), m_fSightAngle(0.f), m_fSightDistance(0.f)
{
}

CMonsterBase::CMonsterBase(const CMonsterBase& rhs)
	:CCharacter(rhs), m_fSightAngle(m_fSightAngle), m_fSightDistance(m_fSightDistance)
{
}

CMonsterBase::~CMonsterBase()
{
}

void CMonsterBase::Debug_Line(_vec3 _Pos, _vec3 _Point1, _vec3 _Point2)
{
	/*VTXCOL pVertex[3] = {
	{ _Pos, D3DCOLOR_XRGB(255, 255, 255) },
	{ _Point1, D3DCOLOR_XRGB(255, 255, 255)},
	{ _Point2, D3DCOLOR_XRGB(255, 255, 255)}
	};*/

	//VTXCOL pVertex[3] = {
	//	{ _vec3{ -1.0f, -1.0f, 5.0f }, D3DCOLOR_XRGB(255, 255, 255) },
	//	{ _vec3{  1.0f, -1.0f, 5.0f }, D3DCOLOR_XRGB(255, 255, 255) },
	//	{ _vec3{  0.0f,  1.0f, 5.0f }, D3DCOLOR_XRGB(255, 255, 255) }
	//};
	//m_pGraphicDev->SetFVF(FVF_COL); // 정점 포맷 설정
	//D3DXMATRIX matWorld;
	//D3DXMatrixIdentity(&matWorld);
	//m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pVertex, sizeof(VTXCOL));
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	//m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// 1. 삼각형 정점
	VTXCOL pVertex[3] = {
		{ _vec3{ -1.f, -1.f, 5.f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  1.f, -1.f, 5.f }, D3DCOLOR_XRGB(255, 255, 255) },
		{ _vec3{  0.f,  1.f, 5.f }, D3DCOLOR_XRGB(255, 255, 255) }
	};

	// 2. 기본 행렬
	D3DXMATRIX matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);


	// 3. 렌더 상태
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 4. 정점 포맷 및 그리기
	m_pGraphicDev->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	m_pGraphicDev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pVertex, sizeof(VTXCOL));
}

void CMonsterBase::Free()
{
	Engine::CCharacter::Free();
}