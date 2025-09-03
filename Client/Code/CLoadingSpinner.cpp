#include "pch.h"
#include "CLoadingSpinner.h"
#include "CRenderer.h"

CLoadingSpinner::CLoadingSpinner(LPDIRECT3DDEVICE9 pGraphicDev)
	: CUIObject(pGraphicDev)
{
}

CLoadingSpinner::CLoadingSpinner(const CLoadingSpinner& rhs)
	: CUIObject(rhs)
{
}

CLoadingSpinner::~CLoadingSpinner()
{
}

HRESULT CLoadingSpinner::Ready_GameObject()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Animation();

	return S_OK;
}

_int CLoadingSpinner::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return 0;

	CUIObject::Update_GameObject(fTimeDelta);

	CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

	return 0;
}

void CLoadingSpinner::LateUpdate_GameObject(const _float& fTimeDelta)
{
	if (m_bIsActive == false)
		return;

	m_pAnimatorCom->Play_Animation(0, 0, fTimeDelta);

	CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CLoadingSpinner::Render_GameObject(const _float& fTimeDelta)
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

	m_pSpriteBuffer->Render_Buffer();

	// 렌더 상태 복원
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

HRESULT CLoadingSpinner::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pSpriteBuffer = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectSpriteTex"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<Engine::CTexture*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_LoadingSpinnerTexture"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CLoadingSpinner::Ready_Animation()
{
	// Pistol Animation
	m_pAnimatorCom->Add_Animation(0, m_pTextureCom, m_pSpriteBuffer, 8, 1, 0, 7, 0.1f, true);
}

CLoadingSpinner* CLoadingSpinner::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CLoadingSpinner* pLoadingSpinner = new CLoadingSpinner(pGraphicDev);

	if (FAILED(pLoadingSpinner->Ready_GameObject()))
	{
		Safe_Release(pLoadingSpinner);
		MSG_BOX("LoadingSpinner Create Failed");
		return nullptr;
	}

	return pLoadingSpinner;
}

CGameObject* CLoadingSpinner::Clone()
{
	return new CLoadingSpinner(*this);
}

void CLoadingSpinner::Free()
{
	Engine::CGameObject::Free();
}
