#include "pch.h"
#include "CBulletBase.h"
#include "CManagement.h"
#include <CDynamicCamera.h>

CBulletBase::CBulletBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	m_iDamage(0)
{
}

CBulletBase::CBulletBase(const CBulletBase& rhs)
	:CGameObject(rhs)
{
}

CBulletBase::~CBulletBase()
{

}

HRESULT CBulletBase::Add_Component()
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

	//MoveTransform
	pComponent = m_pTransformCom = dynamic_cast<Engine::CTransform*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_DYNAMIC].insert({ L"Com_MoveTransform", pComponent });

	//Calculator
	pComponent = m_pCalculatorCom = dynamic_cast<Engine::CCalculator*>
		(CPrototypeManager::GetInstance()->Clone_ComPrototype(L"Proto_Calculator"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

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

void CBulletBase::OnCollision(CCollider* _pOther)
{
}

void CBulletBase::OnCollisionEnter(CCollider* _pOther)
{
	if (_pOther == CManagement::GetInstance()->Get_Player()->GetCollider())
		dynamic_cast<CDynamicCamera*>(_pOther->Get_Owner())->Take_Damage(m_iDamage);
}

void CBulletBase::OnCollisionExit(CCollider* _pOther)
{
}

void CBulletBase::Free()
{
	CGameObject::Free();
}