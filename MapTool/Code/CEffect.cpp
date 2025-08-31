#include "pch.h"
#include "CEffect.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CEffect::CEffect(const CGameObject& rhs)
	:CGameObject(rhs)
{
}

CEffect::~CEffect()
{
}

HRESULT CEffect::Ready_GameObject()
{
    return E_NOTIMPL;
}

_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
    return _int();
}

void CEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
}

void CEffect::Render_GameObject(const _float& fTimeDelta)
{
}

CGameObject* CEffect::Clone()
{
    return nullptr;
}

HRESULT CEffect::Add_Component()
{
	CComponent* pComponent = nullptr;

	//Buffer
    pComponent = m_pBufferCom = dynamic_cast<Engine::CRectSpriteTex*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_RectSpriteTex"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	//Transform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));

	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[ID_STATIC].insert({ L"Com_Transform", pComponent });

	//Animator
	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));

	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CEffect::Free()
{
}
