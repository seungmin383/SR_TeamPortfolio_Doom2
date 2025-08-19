#include "pch.h"
#include "CBulletBase.h"
#include "CManagement.h"
#include <CDynamicCamera.h>

CBulletBase::CBulletBase(LPDIRECT3DDEVICE9 pGraphicDev)
	:CGameObject(pGraphicDev),
	m_iDamage(0), m_vDir(_vec3{0.f,0.f,0.f}),
	m_BulletType(MONSTER_BULLET_TYPE::BULLET_END)
{
}

CBulletBase::CBulletBase(const CBulletBase& rhs)
	:CGameObject(rhs),
	m_iDamage(rhs.m_iDamage), m_vDir(_vec3{ 0.f,0.f,0.f }),
	m_BulletType(rhs.m_BulletType)
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

HRESULT CBulletBase::Ready_GameObject(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos)
{
	m_vDir = vDir;
	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CBulletBase::Update_GameObject(const _float& fTimeDelta)
{


	return 0;
}

void CBulletBase::Create(_vec3 vDir, MONSTER_BULLET_TYPE eBulletType, _vec3 vPos)
{

}

void CBulletBase::Free()
{
	CGameObject::Free();
}