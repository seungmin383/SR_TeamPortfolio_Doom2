#include "pch.h"
#include "CLoadingBackground.h"
#include "CRenderer.h"

CLoadingBackground::CLoadingBackground(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CLoadingBackground::CLoadingBackground(const CLoadingBackground& rhs)
	: CUIObject(rhs)
{
}

CLoadingBackground::~CLoadingBackground()
{
}

HRESULT CLoadingBackground::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Animation();

	return S_OK;
}

_int CLoadingBackground::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CUIObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CLoadingBackground::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CLoadingBackground::Render_GameObject(const _float& fTimeDelta)
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

HRESULT CLoadingBackground::Add_Component()
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
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_LoadingBackground"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	return S_OK;
}

void CLoadingBackground::Ready_Animation()
{
}

CLoadingBackground* CLoadingBackground::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoadingBackground* pLoadingBackground = new CLoadingBackground(pGraphicDev);

	if (FAILED(pLoadingBackground->Ready_GameObject()))
	{
		Safe_Release(pLoadingBackground);
		MSG_BOX("LoadingBackground Create Failed");
		return nullptr;
	}

	return pLoadingBackground;
}

CGameObject* CLoadingBackground::Clone()
{
	return new CLoadingBackground(*this);
}

void CLoadingBackground::Free()
{
	Engine::CGameObject::Free();
}
