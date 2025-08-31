#include "pch.h"
#include "CMainMenuHover.h"
#include "CRenderer.h"

CMainMenuHover::CMainMenuHover(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CMainMenuHover::CMainMenuHover(const CMainMenuHover& rhs)
	: CUIObject(rhs)
{
}

CMainMenuHover::~CMainMenuHover()
{
}

HRESULT CMainMenuHover::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CMainMenuHover::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CUIObject::Update_GameObject(fTimeDelta);

	//CRenderer::GetInstance()->Add_RenderGroup(RENDER_PROJ_UI, this);
	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CMainMenuHover::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMainMenuHover::Render_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

	m_pTextureCom->Set_Texture();

	// 알파블렌딩 설정
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pBufferCom->Render_Buffer();

	// 렌더 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

HRESULT CMainMenuHover::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_MainMenuUI_Hover"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CMainMenuHover* CMainMenuHover::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMainMenuHover* pMainMenuHover = new CMainMenuHover(pGraphicDev);

	if (FAILED(pMainMenuHover->Ready_GameObject()))
	{
		Safe_Release(pMainMenuHover);
		MSG_BOX("MainMenuHover Create Failed");
		return nullptr;
	}

	return pMainMenuHover;
}

CGameObject* CMainMenuHover::Clone()
{
	return new CMainMenuHover(*this);
}

void CMainMenuHover::Free()
{
	Engine::CGameObject::Free();
}
