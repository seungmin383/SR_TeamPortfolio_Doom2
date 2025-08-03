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

void CRenderer::Render_GameObejct(LPDIRECT3DDEVICE9& pGraphicDev)
{
	Render_Priority(pGraphicDev);
	Render_NonAlpha(pGraphicDev);
	Render_Reflect(pGraphicDev);
	Render_Alpha(pGraphicDev);
	Render_UI(pGraphicDev);

	Clear_RenderGroup();
}

void CRenderer::Clear_RenderGroup()
{
	for (size_t i = 0; i < RENDER_END; i++)
	{
		for_each(m_RenderGroup[i].begin(), m_RenderGroup[i].end(), CDeleteObj());
		m_RenderGroup[i].clear();
	}
}

void CRenderer::Render_Priority(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_PRIORITY])
		pObj->Render_GameObject();
}

void CRenderer::Render_NonAlpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	for (auto& pObj : m_RenderGroup[RENDER_NONALPHA])
		pObj->Render_GameObject();
}

void CRenderer::Render_Reflect(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	for (auto& pObj : m_RenderGroup[RENDER_REFLECT])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void CRenderer::Render_Alpha(LPDIRECT3DDEVICE9& pGraphicDev)
{
	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& pObj : m_RenderGroup[RENDER_ALPHA])
		pObj->Render_GameObject();

	pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void CRenderer::Render_UI(LPDIRECT3DDEVICE9& pGraphicDev)
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

		pObj->Render_GameObject();

		// z-buffer 다시 활성화
		pGraphicDev->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	// Render 끝나고 복사해둔 뷰, 투영 행렬 복원해주기
	pGraphicDev->SetTransform(D3DTS_VIEW, &matPrevView);
	pGraphicDev->SetTransform(D3DTS_PROJECTION, &matPrevProj);
}

void CRenderer::Free()
{
	Clear_RenderGroup();
}
