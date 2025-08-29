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
	CUIObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_PRIORITY, this);

	return 0;
}

void CMainMenuHover::LateUpdate_GameObject(const _float& fTimeDelta)
{
	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CMainMenuHover::Render_GameObject(const _float& fTimeDelta)
{
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMainMenuHover::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pBufferCom = dynamic_cast<Engine::CRectTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

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
