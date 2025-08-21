#include "CBullet.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev)
{
}

CBullet::CBullet(const CBullet& rhs)
	:CGameObject(rhs)
{
}

CBullet::~CBullet()
{
}


void CBullet::OnCollision(CCollider* _pOther)
{
}

void CBullet::OnCollisionEnter(CCollider* _pOther)
{
}

void CBullet::OnCollisionExit(CCollider* _pOther)
{
}

HRESULT CBullet::Add_Component()
{
	CComponent* pComponent = NULL;

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

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	//Collider
	pComponent = m_pColliderCom = dynamic_cast<Engine::CCollider*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Collider"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	//Animation
	pComponent = m_pAnimCom = dynamic_cast<Engine::CAnimator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Animator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

void CBullet::Free()
{
	CGameObject::Free();
}
