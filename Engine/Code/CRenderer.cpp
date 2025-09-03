#include "CRenderer.h"

IMPLEMENT_SINGLETON(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
	Free();
}

void CRenderer::Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	if (RENDER_END <= eType || nullptr == pGameObject)
		return;

	m_RenderGroup[eType].push_back(pGameObject);
	pGameObject->Add_Ref();
}

void CRenderer::Render_GameObejct(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	Render_Priority(pGraphicDev, fTimeDelta);
	Render_NonAlpha(pGraphicDev, fTimeDelta);
	Render_Reflect(pGraphicDev, fTimeDelta);
	Render_Alpha(pGraphicDev, fTimeDelta);
	Render_UI(pGraphicDev, fTimeDelta);
	Render_Proj_UI(pGraphicDev, fTimeDelta);

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	//for (size_t i = 0; i < RENDER_END; i++)
	//{
	//	for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
	//	m_RenderGroup[i].clear();
	//}

	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto j = m_RenderGroup[i].begin(); j != m_RenderGroup[i].end();)
		{
			Safe_Release(*j);
			j++;
		}

		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	for (auto& pObj : m_RenderGroup[RENDER_PRIORITY])
		pObj->Render_GameObject(fTimeDelta);
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA])
		pObj->Render_GameObject(fTimeDelta);
}

void CRenderer::Render_Reflect(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	for (auto& pObj : m_RenderGroup[RENDER_REFLECT])
		pObj->Render_GameObject(fTimeDelta);

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	/*pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);*/

	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 100);
	pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); /* 통과조건 == 그리는 조건 */
	// z-sorting
	// 투명 객체 목록을 받아와서 정렬해야함.
	// 정렬 기준은 카메라와의 거리.
	// 거리 계산은 뷰 공간의 z 값을 사용하는데 뷰 공간에서 카메라의 위치는 원점이므로, 객체의 z 좌표 자체가 카메라로부터의 상대적인 깊이값이다.
	// 먼저 뷰 공간 행렬 가져오기
	_matrix matView;
	pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	vector<CGameObject*> vecAlpha(m_RenderGroup[RENDER_ALPHA].begin(), m_RenderGroup[RENDER_ALPHA].end());

	sort(vecAlpha.begin(), vecAlpha.end(), [&](CGameObject* pA, CGameObject* pB)
		{
			// 람다식으로 z 값을 받아와서 정렬
			_vec3 vPosA, vPosB;
			pA->Get_Transform()->Get_Pos(&vPosA);
			pB->Get_Transform()->Get_Pos(&vPosB);

			// 객체의 좌표를 뷰 행렬과 행렬 곱 연산을 하여 월드 좌표 -> 뷰 좌표로 변환
			_vec3 vViewPosA, vViewPosB;
			D3DXVec3TransformCoord(&vViewPosA, &vPosA, &matView);
			D3DXVec3TransformCoord(&vViewPosB, &vPosB, &matView);

			return vViewPosA.z > vViewPosB.z;
		});

	// 정렬된 순서로 렌더링
	for (auto& pObj : vecAlpha)
	{
		pObj->Render_GameObject(fTimeDelta);
	}

	//for (auto& pObj : m_RenderGroup[RENDER_ALPHA]) {
	//	pObj->Render_GameObject(fTimeDelta);
	//}	

	// 렌더 상태 복구하기
	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	//pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	// 기존의 뷰, 투영 행렬을 저장
	_matrix matPrevView, matPrevProj;
	pGraphicDev->GetTransform(D3DTS_VIEW, &matPrevView);
	pGraphicDev->GetTransform(D3DTS_PROJECTION, &matPrevProj);

	// 직교 투영 행렬 만들기
	_matrix matView, matProj;
	D3DXMatrixIdentity(&matView);	// UI는 카메라 움직임과 상관이 없어서 단위 행렬 써야하나?
	D3DXMatrixOrthoLH(&matProj, WINCX, WINCY, 0.f, 1.f);	// 화면 크기와 똑같은 직교 투영

	pGraphicDev->SetTransform(D3DTS_VIEW, &matView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matProj);

	for (auto& pObj : m_RenderGroup[RENDER_UI])
	{
		// z-buffer 비활성화
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

		pObj->Render_GameObject(fTimeDelta);

		// z-buffer 다시 활성화
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	// 혹시 몰라서 Render_PLAYERUI 라는 enum 값을 추가함(08/05)
	for (auto& pObj : m_RenderGroup[RENDER_PLAYERUI])
	{
		// z-buffer 비활성화
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

		pObj->Render_GameObject(fTimeDelta);

		// z-buffer 다시 활성화
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	// Render 끝나고 복사해둔 뷰, 투영 행렬 복원해주기
	pGraphicDev->SetTransform(D3DTS_VIEW, &matPrevView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matPrevProj);
}

void CRenderer::Render_Proj_UI(LPDIRECT3DDEVICE9& pGraphicDev, const _float& fTimeDelta)
{
	// ---- 상태 백업 ----
	//DWORD oldAlpha, oldLight, oldZ;
	//pGraphicDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlpha);
	//pGraphicDev->GetRenderState(D3DRS_LIGHTING, &oldLight);
	//pGraphicDev->GetRenderState(D3DRS_ZENABLE, &oldZ);

	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	//pGraphicDev->SetRenderState(D3DRS_ZENABLE, FALSE);

	for (auto& pObj : m_RenderGroup[RENDER_PROJ_UI])
		pObj->Render_GameObject(fTimeDelta);

	// ---- 상태 복구 ----
	//pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlpha);
	//pGraphicDev->SetRenderState(D3DRS_LIGHTING, oldLight);
	//pGraphicDev->SetRenderState(D3DRS_ZENABLE, oldZ);
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
