#include "pch.h"
#include "CRenderer.h"
#include "CMainMenuBackground.h"

CMainMenuBackground::CMainMenuBackground(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CMainMenuBackground::CMainMenuBackground(const CMainMenuBackground& rhs)
	: CUIObject(rhs)
{
}

CMainMenuBackground::~CMainMenuBackground()
{
}

HRESULT CMainMenuBackground::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

_int CMainMenuBackground::Update_GameObject(const _float& fTimeDelta)
{
	CUIObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CMainMenuBackground::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMainMenuBackground::Render_GameObject(const _float& fTimeDelta)
{
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

HRESULT CMainMenuBackground::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_BackGroundTexture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

CMainMenuBackground* CMainMenuBackground::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMainMenuBackground* pMainMenuBackground = new CMainMenuBackground(pGraphicDev);

	if (FAILED(pMainMenuBackground->Ready_GameObject()))
	{
		Safe_Release(pMainMenuBackground);
		MSG_BOX("MainMenuBackground Create Failed");
		return nullptr;
	}

	return pMainMenuBackground;
}

CGameObject* CMainMenuBackground::Clone()
{
	return new CMainMenuBackground(*this);
}

void CMainMenuBackground::Free()
{
	Engine::CGameObject::Free();
}
